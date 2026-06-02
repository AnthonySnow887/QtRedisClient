#include "QtRedisContextSsl.h"
#include "QtRedisParser.h"
#include <QSslConfiguration>
#include <QSslKey>
#include <QFile>

//!
//! \brief Конструктор класса
//! \param host IP-адрес
//! \param port Порт
//!
QtRedisContextSsl::QtRedisContextSsl(const QString &host, const uint port)
    : QtRedisContext(host, port)
    , _socket(new QSslSocket(this))
{
    _socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(_socket, &QSslSocket::connected, this, &QtRedisContext::connected);
    connect(_socket, &QSslSocket::disconnected, this, &QtRedisContext::disconnected);
    connect(_socket, &QSslSocket::readyRead, this, &QtRedisContext::readyRead);
}

//!
//! \brief Деструктор класса
//!
QtRedisContextSsl::~QtRedisContextSsl()
{
    if (_socket) {
        this->disconnectFromServer();
        delete _socket;
        _socket = nullptr;
    }
}

//!
//! \brief Подключиться к серверу
//! \param msecs Время ожидания мсек
//! \return
//!
bool QtRedisContextSsl::connectToServer(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;
    if (_socket && _socket->isOpen())
        return true;
    if (_host.isEmpty() || _port == 0)
        return false;

    // check ssl
    if (!QSslSocket::supportsSsl()) {
        qCritical() << "[QtRedisContextSsl][connectToServer] SSL not supported!";
        return false;
    }
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_3OrLater);

    // 1. Загружаем КЛИЕНТСКИЙ СЕРТИФИКАТ (client.crt)
    QFile certFile("/home/user/Projects/QtRedisClient/testRedis/redis-cli.crt");
    if (certFile.open(QIODevice::ReadOnly)) {
        QSslCertificate clientCert(&certFile, QSsl::Pem);
        sslConfig.setLocalCertificate(clientCert);
        certFile.close();
    } else {
        qDebug() << "Не удалось открыть файл сертификата клиента!";
        return false;
    }

    // 2. Загружаем КЛИЕНТСКИЙ ЗАКРЫТЫЙ КЛЮЧ (client.key)
    QFile keyFile("/home/user/Projects/QtRedisClient/testRedis/redis-cli.key");
    if (keyFile.open(QIODevice::ReadOnly)) {
        // Укажите QSsl::Rsa или QSsl::Ec в зависимости от типа вашего ключа.
        // Если ключ запаролен, передайте пароль вторым аргументом.
        QSslKey clientKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
        sslConfig.setPrivateKey(clientKey);
        keyFile.close();
    } else {
        qDebug() << "Не удалось открыть файл ключа клиента!";
    }

    // 3. (Опционально) Добавляем корневой сертификат (ca.crt), чтобы доверять серверу
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    QList<QSslCertificate> caCerts = QSslCertificate::fromPath("/home/user/Projects/QtRedisClient/testRedis/ca.crt");
    sslConfig.setCaCertificates(caCerts);

    // Если сертификаты самоподписанные и проверка сервера не нужна:
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

    _socket->setSslConfiguration(sslConfig);

    int buffMsecs = 30 * 1000;
    if (msecs > 0)
        buffMsecs = msecs;

    _socket->connectToHostEncrypted(_host, _port);
    return _socket->waitForEncrypted(buffMsecs);
}

//!
//! \brief Переподключиться к серверу
//! \param msecs Время ожидания мсек
//! \return
//!
bool QtRedisContextSsl::reconnectToServer(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;

    _socket->abort();
    _socket->close();
    lock.unlock();
    return this->connectToServer(msecs);
}

//!
//! \brief Отключиться от сервера
//!
void QtRedisContextSsl::disconnectFromServer()
{
    QMutexLocker lock(&_mutex);
    if (_socket) {
        _socket->abort();
        _socket->close();
    }
}

//!
//! \brief Подключен ли к серверу
//! \return
//!
bool QtRedisContextSsl::isConnected()
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;
    if (_socket->state() == QAbstractSocket::ConnectedState)
        return true;

    return false;
}

//!
//! \brief Доступны ли данные для чтения
//! \return
//!
bool QtRedisContextSsl::canReadRawData() const
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;
    if (_socket->bytesAvailable() > 0)
        return true;

    return false;
}

//!
//! \brief Количество байт, доступных для чтения
//! \return
//!
qint64 QtRedisContextSsl::bytesAvailable() const
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return 0;

    return _socket->bytesAvailable();
}

//!
//! \brief Записать данные
//! \param data Данные
//! \return
//!
qint64 QtRedisContextSsl::writeRawData(const QByteArray &data)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return 0;
    if (data.isEmpty())
        return 0;

    QByteArray buffData = data;
    char* charData = buffData.data();
    qint64 total = 0;
    qint64 writed = 0;

    while (total < data.size()) {
        writed = _socket->write(charData + total, data.size() - total);
        total += writed;
    }
    _socket->flush();
    return total;
}

//!
//! \brief Прочитать данные
//! \return
//!
QByteArray QtRedisContextSsl::readRawData()
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return QByteArray();

    return _socket->readAll();
}

//!
//! \brief Ожидать поступления данных
//! \param msecs Время ожидания мсек
//! \return
//!
bool QtRedisContextSsl::waitForReadyRead(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;

    return _socket->waitForReadyRead(msecs);
}

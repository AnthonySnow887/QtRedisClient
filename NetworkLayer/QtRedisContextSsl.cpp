#include "QtRedisContextSsl.h"
#include "QtRedisParser.h"

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
//    _socket->setSslConfiguration(QSslConfiguration::defaultConfiguration());

//    QList<QSslCertificate> trustedCas = conf.sslCaCertificates();
//    if (!trustedCas.empty()) {
//        m_socket->addCaCertificates(trustedCas);
//    }

//    QString privateKey = conf.sslPrivateKeyPath();
//    if (!privateKey.isEmpty()) {
//        m_socket->setPrivateKey(privateKey);
//    }

//    QString localCert = conf.sslLocalCertPath();
//    if (!localCert.isEmpty()) {
//        m_socket->setLocalCertificate(localCert);
//    }

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

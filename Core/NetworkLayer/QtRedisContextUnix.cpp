#include "QtRedisContextUnix.h"
#include "QtRedisParser.h"

//!
//! \brief Конструктор класса
//! \param host IP-адрес
//! \param port Порт
//!
QtRedisContextUnix::QtRedisContextUnix(const QString &sockPath)
    : QtRedisContext(sockPath, -1)
    , _socket(new QLocalSocket(this))
{
    connect(_socket, &QLocalSocket::connected, this, &QtRedisContext::connected);
    connect(_socket, &QLocalSocket::disconnected, this, &QtRedisContext::disconnected);
    connect(_socket, &QLocalSocket::readyRead, this, &QtRedisContext::readyRead);
}

//!
//! \brief Деструктор класса
//!
QtRedisContextUnix::~QtRedisContextUnix()
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
bool QtRedisContextUnix::connectToServer(const int msecs, QString &error)
{
    QMutexLocker lock(&_mutex);
    error.clear();
    if (!_socket) {
        error = QString("Invalid socket object (is nullptr)!");
        return false;
    }
    if (_socket && _socket->isOpen())
        return true;
    if (_host.isEmpty()) {
        error = QString("Invalid host (empty)!");
        return false;
    }

    int buffMsecs = 30 * 1000;
    if (msecs > 0)
        buffMsecs = msecs;

    _socket->connectToServer(_host);
    const bool isConnected = _socket->waitForConnected(buffMsecs);
    if (!isConnected) {
        const QString socketErr = _socket->errorString();
        error = !socketErr.isEmpty() ? socketErr : "QLocalSocket: waitForConnected failed!";
    }
    return isConnected;
}

//!
//! \brief Переподключиться к серверу
//! \param msecs Время ожидания мсек
//! \return
//!
bool QtRedisContextUnix::reconnectToServer(const int msecs, QString &error)
{
    QMutexLocker lock(&_mutex);
    error.clear();
    if (!_socket) {
        error = QString("Invalid socket object (is nullptr)!");
        return false;
    }

    _socket->abort();
    _socket->close();
    lock.unlock();
    return this->connectToServer(msecs, error);
}

//!
//! \brief Отключиться от сервера
//!
void QtRedisContextUnix::disconnectFromServer()
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
bool QtRedisContextUnix::isConnected()
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;
    if (_socket->state() == QLocalSocket::ConnectedState)
        return true;

    return false;
}

//!
//! \brief Доступны ли данные для чтения
//! \return
//!
bool QtRedisContextUnix::canReadRawData() const
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
qint64 QtRedisContextUnix::bytesAvailable() const
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
qint64 QtRedisContextUnix::writeRawData(const QByteArray &data)
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
QByteArray QtRedisContextUnix::readRawData()
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
bool QtRedisContextUnix::waitForReadyRead(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;

    return _socket->waitForReadyRead(msecs);
}

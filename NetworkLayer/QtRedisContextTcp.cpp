#include "QtRedisContextTcp.h"
#include "QtRedisParser.h"

#include <QtNetwork/QHostAddress>

//!
//! \brief Конструктор класса
//! \param host Хост
//! \param port Порт
//!
QtRedisContextTcp::QtRedisContextTcp(const QString &host, const uint port)
    : QtRedisContext(host, port)
    , _socket(new QTcpSocket(this))
{
}

//!
//! \brief Деструктор класса
//!
QtRedisContextTcp::~QtRedisContextTcp()
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
bool QtRedisContextTcp::connectToServer(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;
    if (_socket && _socket->isOpen())
        return true;
    if (_host.isEmpty() || _port == 0)
        return false;

    int buffMsecs = 30 * 1000;
    if (msecs > 0)
        buffMsecs = msecs;

    _socket->connectToHost(QHostAddress(_host), _port);
    return _socket->waitForConnected(buffMsecs);
}

//!
//! \brief Переподключиться к серверу
//! \param msecs Время ожидания мсек
//! \return
//!
bool QtRedisContextTcp::reconnectToServer(const int msecs)
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
void QtRedisContextTcp::disconnectFromServer()
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
bool QtRedisContextTcp::isConnected()
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
bool QtRedisContextTcp::canReadRawData() const
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
qint64 QtRedisContextTcp::bytesAvailable() const
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
qint64 QtRedisContextTcp::writeRawData(const QByteArray &data)
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
QByteArray QtRedisContextTcp::readRawData()
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
bool QtRedisContextTcp::waitForReadyRead(const int msecs)
{
    QMutexLocker lock(&_mutex);
    if (!_socket)
        return false;

    return _socket->waitForReadyRead(msecs);
}

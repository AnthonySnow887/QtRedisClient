#include "QtRedisTransporter.h"
#include "QtRedisParser.h"
#include "QtRedisContextTcp.h"
#include "QtRedisContextSsl.h"
#include "QtRedisContextUnix.h"

#include <QDebug>

//!
//! \brief Конструктор класса
//!
QtRedisTransporter::QtRedisTransporter()
    : QObject()
{}

//!
//! \brief Деструктор класса
//!
QtRedisTransporter::~QtRedisTransporter()
{
    this->clearTransporter();
}

//!
//! \brief Тип
//! \return
//!
QtRedisTransporter::TransporterType QtRedisTransporter::type() const
{
    QMutexLocker lock(&_mutex);
    return _type;
}

//!
//! \brief Выполнена ли инициализация
//! \return
//!
bool QtRedisTransporter::isInit() const
{
    QMutexLocker lock(&_mutex);
    if (_context)
        return true;

    return false;
}

//!
//! \brief Хост
//! \return
//!
QString QtRedisTransporter::host() const
{
    QMutexLocker lock(&_mutex);
    if (!_context)
        return QString();

    return _context->host();
}

//!
//! \brief Порт
//! \return
//!
int QtRedisTransporter::port() const
{
    QMutexLocker lock(&_mutex);
    if (!_context)
        return -1;

    return _context->port();
}

//!
//! \brief Текущий индекс БД
//! \return
//!
int QtRedisTransporter::currentDbIndex() const
{
    QMutexLocker lock(&_mutex);
    if (!_context)
        return -1;

    return _context->currentDbIndex();
}

//!
//! \brief Выполнить инициализацию
//! \param type Тип
//! \param host Хост
//! \param port Порт
//! \return
//!
bool QtRedisTransporter::initTransporter(const TransporterType &type,
                                         const QString &host,
                                         const int port)
{
    QMutexLocker lock(&_mutex);
    if (_context) {
        qCritical() << "[QtRedisTransporter][initTransporter] Already initialyzed!";
        return false;
    }

    _type = type;
    switch (_type) {
        case TransporterType::Tcp: {
            _context = new QtRedisContextTcp(host, port);
            break;
        }
        case TransporterType::Ssl: {
            _context = new QtRedisContextSsl(host, port);
            break;
        }
        case TransporterType::Unix: {
            _context = new QtRedisContextUnix(host);
            break;
        }
        default: {
            _context = new QtRedisContextTcp(host, port);
            _type = TransporterType::Tcp;
            break;
        }
    }
    return true;
}

//!
//! \brief Очистить объект и разорвать соединение
//!
void QtRedisTransporter::clearTransporter()
{
    QMutexLocker lock(&_mutex);
    _type = TransporterType::NoType;
    _timeoutMSec = 0;
    if (_context) {
        delete _context;
        _context = nullptr;
    }
}

//!
//! \brief Подключиться к серверу
//! \param timeoutMSec Время ожидания мсек
//! \return
//!
bool QtRedisTransporter::connectToServer(const int timeoutMSec)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][connectToServer] Is not initialyzed!";
        lock.unlock();
        return false;
    }
    if (timeoutMSec > 0
        && _timeoutMSec != timeoutMSec)
        _timeoutMSec = timeoutMSec;

    _context->setCurrentDbIndex(0); // clear db index
    return _context->connectToServer(_timeoutMSec);
}

//!
//! \brief Переподключиться к серверу
//! \param timeoutMSec Время ожидания мсек
//! \return
//!
bool QtRedisTransporter::reconnectToServer(const int timeoutMSec)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][reconnectToServer] Is not initialyzed!";
        return false;
    }
    if (timeoutMSec > 0
        && _timeoutMSec != timeoutMSec)
        _timeoutMSec = timeoutMSec;

    _context->setCurrentDbIndex(0); // clear db index
    return _context->reconnectToServer(_timeoutMSec);
}

//!
//! \brief Отключиться от сервера
//!
void QtRedisTransporter::disconnectFromServer()
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][disconnectFromServer] Is not initialyzed!";
        return;
    }
    _context->disconnectFromServer();
    _context->setCurrentDbIndex(0); // clear db index
}

//!
//! \brief Подключен ли к серверу
//! \return
//!
bool QtRedisTransporter::isConnected()
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
//        qCritical() << "[QtRedisTransporter][isConnected] Is not initialyzed!";
        return false;
    }
    return _context->isConnected();
}

//!
//! \brief Отправить команду и получить ответ от сервера
//! \param command Команда и ее аргументы
//! \return
//!
QtRedisReply QtRedisTransporter::sendCommand(const QStringList &command)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][sendCommand] Is not initialyzed!";
        return QtRedisReply();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendCommand] Command is Empty!";
        return QtRedisReply();
    }
    _context->writeRawData(QtRedisParser::createRawData(command));
    if (!_context->waitForReadyRead())
        return QtRedisReply();

    QtRedisReply reply;
    QByteArray replyData;
    while (true) {
        replyData += _context->readRawData();
        if (!replyData.isEmpty()) {
            bool isOk = false;
            reply = QtRedisParser::parseRawData(replyData, &isOk);
            if (isOk)
                break;
        }
        if (!_context->waitForReadyRead())
            return QtRedisReply();
    }
    this->checkCommandResult(command, reply);
    return reply;
}

//!
//! \brief Отправить команду и получить ответ от сервера
//! \param command Команда и ее аргументы
//! \return
//!
QtRedisReply QtRedisTransporter::sendCommand(const QVariantList &command)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][sendCommand] Is not initialyzed!";
        return QtRedisReply();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendCommand] Command is Empty!";
        return QtRedisReply();
    }
    _context->writeRawData(QtRedisParser::createRawData(command));
    if (!_context->waitForReadyRead())
        return QtRedisReply();

    QtRedisReply reply;
    QByteArray replyData;
    while (true) {
        replyData += _context->readRawData();
        bool isOk = false;
        reply = QtRedisParser::parseRawData(replyData, &isOk);
        if (isOk)
            break;
        if (!_context->waitForReadyRead())
            return QtRedisReply();
    }
    this->checkCommandResult(command, reply);
    return reply;
}

//!
//! \brief Проверка команды и ее результата
//! \param command Команда
//! \param reply Результат
//!
//! Данный метод нужен для проверки системных команд (смена текущей БД и пр.)
//! и отслеживания их результата.
//!
void QtRedisTransporter::checkCommandResult(const QStringList &command, const QtRedisReply &reply)
{
    if (command.isEmpty())
        return;

    // check select db
    if (command.size() == 2
        && command[0].toUpper() == QString("SELECT")
        && reply.type() == QtRedisReply::ReplyType::Status
        && reply.strValue() == QString("OK")) {
        _context->setCurrentDbIndex(command[1].toInt());
    }
}

//!
//! \brief Проверка команды и ее результата
//! \param command Команда
//! \param reply Результат
//!
//! Данный метод нужен для проверки системных команд (смена текущей БД и пр.)
//! и отслеживания их результата.
//!
void QtRedisTransporter::checkCommandResult(const QVariantList &command, const QtRedisReply &reply)
{
    if (command.isEmpty())
        return;

    // check select db
    if (command.size() == 2
        && command[0].toString().toUpper() == QString("SELECT")
        && reply.type() == QtRedisReply::ReplyType::Status
        && reply.strValue() == QString("OK")) {
        _context->setCurrentDbIndex(command[1].toInt());
    }
}

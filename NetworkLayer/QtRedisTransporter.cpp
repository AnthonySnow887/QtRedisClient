#include "QtRedisTransporter.h"
#include "QtRedisParser.h"
#include "QtRedisContextTcp.h"
#include "QtRedisContextSsl.h"
#include "QtRedisContextUnix.h"

#include <QMetaType>
#include <QDebug>

//!
//! \brief Конструктор класса
//!
QtRedisTransporter::QtRedisTransporter()
    : QObject()
{
    qRegisterMetaType<QtRedisReply>("QtRedisReply");
}

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
//! \brief Подписан ли на события сервера
//! \return
//!
bool QtRedisTransporter::isSubscribed() const
{
    QMutexLocker lock(&_mutex);
    if (_contextSub && _contextSub->isConnected())
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
    _context = this->makeTransporter(type, host, port, false);
    return true;}

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
    if (_contextSub) {
        delete _contextSub;
        _contextSub = nullptr;
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
//! \brief Переподключиться к серверу (все соединения)
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
    bool isOk = _context->reconnectToServer(_timeoutMSec);
    if (_contextSub) {
        _contextSub->setCurrentDbIndex(0); // clear db index
        isOk = _contextSub->reconnectToServer(_timeoutMSec);
    }
    return isOk;
}

//!
//! \brief Подключить к серверу в режиме ожидания входящих сообщений
//! \param timeoutMSec Время ожидания мсек
//! \return
//!
bool QtRedisTransporter::subscribeToServer(const int timeoutMSec)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][subscribeToServer] Is not initialyzed!";
        return false;
    }
    if (!_contextSub) {
        _contextSub = this->makeTransporter(_type, _context->host(), _context->port(), true);
        connect(_contextSub, &QtRedisContext::readyRead,
                this, &QtRedisTransporter::onReadyReadSub,
                Qt::QueuedConnection);
    }
    if (timeoutMSec > 0
        && _timeoutMSec != timeoutMSec)
        _timeoutMSec = timeoutMSec;

    _contextSub->setCurrentDbIndex(0); // clear db index
    return _contextSub->connectToServer(_timeoutMSec);
}

//!
//! \brief Отключиться от сервера (от режима ожидания входящих сообщений)
//!
void QtRedisTransporter::unsubscribeFromServer()
{
    QMutexLocker lock(&_mutex);
    if (!_contextSub)
        return;
    _contextSub->disconnectFromServer();
    _contextSub->setCurrentDbIndex(0); // clear db index
}

//!
//! \brief Отключиться от сервера (все соединения)
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
    if (_contextSub) {
        _contextSub->disconnectFromServer();
        _contextSub->setCurrentDbIndex(0); // clear db index
    }
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
    return this->sendConextCommand(_context, command);
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
    return this->sendConextCommand(_context, command);
}

QtRedisReply QtRedisTransporter::sendSubscribeCommand(const QStringList &command)
{
    QMutexLocker lock(&_mutex);
    if (!_contextSub) {
        qCritical() << "[QtRedisTransporter][sendSubscribeCommand] Is not subscribed!";
        return QtRedisReply();
    }
    return this->sendConextCommand(_contextSub, command);
}

//!
//! \brief Создать объект контекста по работе с Redis-ом
//! \param type
//! \param host
//! \param port
//! \param supportSignals
//! \return
//!
QtRedisContext *QtRedisTransporter::makeTransporter(const TransporterType &type,
                                                    const QString &host,
                                                    const int port,
                                                    const bool supportSignals)
{
    _type = type;
    switch (_type) {
        case TransporterType::Tcp:
            return new QtRedisContextTcp(host, port, supportSignals);
        case TransporterType::Ssl:
            return new QtRedisContextSsl(host, port, supportSignals);
        case TransporterType::Unix:
            return new QtRedisContextUnix(host, supportSignals);
        default:
            break;
    }
    _type = TransporterType::Tcp;
    return new QtRedisContextTcp(host, port, supportSignals);
}

QtRedisReply QtRedisTransporter::sendConextCommand(QtRedisContext *context, const QStringList &command)
{
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendConextCommand] Is not initialyzed!";
        return QtRedisReply();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendConextCommand] Command is Empty!";
        return QtRedisReply();
    }
    context->writeRawData(QtRedisParser::createRawData(command));
    if (!context->waitForReadyRead())
        return QtRedisReply();

    QtRedisReply reply;
    QByteArray replyData;
    while (true) {
        replyData += context->readRawData();
        if (!replyData.isEmpty()) {
            bool isOk = false;
            reply = QtRedisParser::parseRawData(replyData, &isOk);
            if (isOk)
                break;
        }
        if (!context->waitForReadyRead())
            return QtRedisReply();
    }
    this->checkCommandResult(command, reply);
    return reply;
}

QtRedisReply QtRedisTransporter::sendConextCommand(QtRedisContext *context, const QVariantList &command)
{
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendConextCommand] Is not initialyzed!";
        return QtRedisReply();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendConextCommand] Command is Empty!";
        return QtRedisReply();
    }
    context->writeRawData(QtRedisParser::createRawData(command));
    if (!context->waitForReadyRead())
        return QtRedisReply();

    QtRedisReply reply;
    QByteArray replyData;
    while (true) {
        replyData += context->readRawData();
        if (!replyData.isEmpty()) {
            bool isOk = false;
            reply = QtRedisParser::parseRawData(replyData, &isOk);
            if (isOk)
                break;
        }
        if (!context->waitForReadyRead())
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

//!
//! \brief Слот чтения входящих сооющений от сервера Redis
//!
void QtRedisTransporter::onReadyReadSub()
{
    QtRedisReply reply;
    QByteArray replyData;
    const qint64 bytesAvailable = _contextSub->bytesAvailable();
    if (bytesAvailable <= 0)
        return;
    while (true) {
        replyData += _contextSub->readRawData();
        if (replyData.size() >= bytesAvailable)
            break;
    }
    if (replyData.isEmpty())
        return;
    bool isOk = false;
    reply = QtRedisParser::parseRawData(replyData, &isOk);
    if (isOk
        && reply.type() == QtRedisReply::ReplyType::Array
        && reply.arrayValueSize() == 3
        && reply.arrayValue()[0].strValue() == "message") {
        emit this->incomingChannelMessage(reply.arrayValue()[1].strValue(), reply.arrayValue()[2]);
    }
}

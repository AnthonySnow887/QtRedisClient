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
QtRedisTransporter::QtRedisTransporter(const TransporterChannelMode contextChannelMode)
    : QObject()
    , _channelMode(contextChannelMode)
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
//! \brief Тип соединения для pub/sub
//! \return
//!
QtRedisTransporter::TransporterChannelMode QtRedisTransporter::channelMode() const
{
    QMutexLocker lock(&_mutex);
    return _channelMode;
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
    _context = this->makeContext_unsafe(type, host, port);
    if (_channelMode == TransporterChannelMode::CurrentConnection)
        connect(_context, &QtRedisContext::readyRead,
                this, &QtRedisTransporter::onReadyReadSub,
                Qt::QueuedConnection);
    return true;
}

//!
//! \brief Очистить объект и разорвать соединение
//!
void QtRedisTransporter::clearTransporter()
{
    QMutexLocker lock(&_mutex);
    _type = TransporterType::NoType;
    _channelMode = TransporterChannelMode::CurrentConnection;
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
    QtRedisContext *context = nullptr;
    switch (_channelMode) {
        case TransporterChannelMode::CurrentConnection: {
            context = _context;
            break;
        }
        case TransporterChannelMode::SeparateConnection: {
            if (!_contextSub) {
                _contextSub = this->makeContext_unsafe(_type, _context->host(), _context->port());
                connect(_contextSub, &QtRedisContext::readyRead,
                        this, &QtRedisTransporter::onReadyReadSub,
                        Qt::QueuedConnection);
            }
            context = _contextSub;
            break;
        }
        default:
            break;
    }
    if (!context) {
        qCritical() << "[QtRedisTransporter][subscribeToServer] Not found context!";
        return false;
    }
    if (context->isConnected())
        return true;
    if (timeoutMSec > 0
        && _timeoutMSec != timeoutMSec)
        _timeoutMSec = timeoutMSec;

    context->setCurrentDbIndex(0); // clear db index
    return context->connectToServer(_timeoutMSec);
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
bool QtRedisTransporter::isConnected() const
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
//        qCritical() << "[QtRedisTransporter][isConnected] Is not initialyzed!";
        return false;
    }
    return _context->isConnected();
}

//!
//! \brief Подписан ли на события сервера
//! \return
//!
bool QtRedisTransporter::isSubscribed() const
{
    QMutexLocker lock(&_mutex);
    QtRedisContext *context = this->channelContext_unsafe();
    if (!context) {
//        qCritical() << "[QtRedisTransporter][isSubscribed] Not found context!";
        return false;
    }
    return context->isConnected();
}

//!
//! \brief Отправить команду и получить ответ от сервера (с разбором первого сообщения)
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
    return this->sendContextCommand(_context, command);
}

//!
//! \brief Отправить команду и получить ответ от сервера (с разбором первого сообщения)
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
    return this->sendContextCommand(_context, command);
}

//!
//! \brief Отправить команду и получить ответ от сервера (с разбором всех сообщений)
//! \param command Команда и ее аргументы
//! \return
//!
QList<QtRedisReply> QtRedisTransporter::sendCommand_lst(const QStringList &command)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][sendCommand_lst] Is not initialyzed!";
        return QList<QtRedisReply>();
    }
    return this->sendContextCommand_lst(_context, command);
}

//!
//! \brief Отправить команду и получить ответ от сервера (с разбором всех сообщений)
//! \param command Команда и ее аргументы
//! \return
//!
QList<QtRedisReply> QtRedisTransporter::sendCommand_lst(const QVariantList &command)
{
    QMutexLocker lock(&_mutex);
    if (!_context) {
        qCritical() << "[QtRedisTransporter][sendCommand_lst] Is not initialyzed!";
        return QList<QtRedisReply>();
    }
    return this->sendContextCommand_lst(_context, command);
}

//!
//! \brief Отправить команду по работе с каналами и получить ответ от сервера (с разбором первого сообщения)
//! \param command Команда и ее аргументы
//! \return
//!
QtRedisReply QtRedisTransporter::sendChannelCommand(const QStringList &command)
{
    QMutexLocker lock(&_mutex);
    QtRedisContext *context = this->channelContext_unsafe();
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendChannelCommand] Is not subscribed!";
        return QtRedisReply();
    }
    return this->sendContextCommand(context, command);
}

//!
//! \brief Отправить команду по работе с каналами и получить ответ от сервера (с разбором всех сообщений)
//! \param command Команда и ее аргументы
//! \return
//!
QList<QtRedisReply> QtRedisTransporter::sendChannelCommand_lst(const QStringList &command)
{
    QMutexLocker lock(&_mutex);
    QtRedisContext *context = this->channelContext_unsafe();
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendChannelCommand_lst] Is not subscribed!";
        return QList<QtRedisReply>();
    }
    return this->sendContextCommand_lst(context, command);
}

//!
//! \brief Создать объект контекста по работе с Redis-ом
//! \param type
//! \param host
//! \param port
//! \param supportSignals
//! \return
//!
QtRedisContext *QtRedisTransporter::makeContext_unsafe(const TransporterType &type,
                                                       const QString &host,
                                                       const int port)
{
    _type = type;
    switch (_type) {
        case TransporterType::Tcp:
            return new QtRedisContextTcp(host, port);
        case TransporterType::Ssl:
            return new QtRedisContextSsl(host, port);
        case TransporterType::Unix:
            return new QtRedisContextUnix(host);
        default:
            break;
    }
    _type = TransporterType::Tcp;
    return new QtRedisContextTcp(host, port);
}

//!
//! \brief Получить объект контекста по типу подписки
//! \return
//!
QtRedisContext *QtRedisTransporter::channelContext_unsafe() const
{
    switch (_channelMode) {
        case TransporterChannelMode::CurrentConnection:
            return _context;
        case TransporterChannelMode::SeparateConnection:
            return _contextSub;
        default:
            break;
    }
    return nullptr;
}

QtRedisReply QtRedisTransporter::sendContextCommand(QtRedisContext *context, const QStringList &command)
{
    return this->sendContextCommand_lst(context, command)[0];
}

QtRedisReply QtRedisTransporter::sendContextCommand(QtRedisContext *context, const QVariantList &command)
{
    return this->sendContextCommand_lst(context, command)[0];
}

QList<QtRedisReply> QtRedisTransporter::sendContextCommand_lst(QtRedisContext *context, const QStringList &command)
{
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendContextCommand_lst] Is not initialyzed!";
        return QList<QtRedisReply>();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendContextCommand_lst] Command is Empty!";
        return QList<QtRedisReply>();
    }
    context->writeRawData(QtRedisParser::createRawData(command));
    if (!context->waitForReadyRead())
        return QList<QtRedisReply>();

    QList<QtRedisReply> reply;
    QByteArray replyData;
    while (true) {
        replyData += context->readRawData();
        if (!replyData.isEmpty()) {
            bool isOk = false;
            reply = QtRedisParser::parseRawDataList(replyData, &isOk);
            if (isOk)
                break;
        }
        if (!context->waitForReadyRead())
            return QList<QtRedisReply>();
    }
    for (const QtRedisReply &r : qAsConst(reply))
        this->checkCommandResult(context, command, r);
    return reply;
}

QList<QtRedisReply> QtRedisTransporter::sendContextCommand_lst(QtRedisContext *context, const QVariantList &command)
{
    if (!context) {
        qCritical() << "[QtRedisTransporter][sendContextCommand_lst] Is not initialyzed!";
        return QList<QtRedisReply>();
    }
    if (command.isEmpty()) {
        qCritical() << "[QtRedisTransporter][sendContextCommand_lst] Command is Empty!";
        return QList<QtRedisReply>();
    }
    context->writeRawData(QtRedisParser::createRawData(command));
    if (!context->waitForReadyRead())
        return QList<QtRedisReply>();

    QList<QtRedisReply> reply;
    QByteArray replyData;
    while (true) {
        replyData += context->readRawData();
        if (!replyData.isEmpty()) {
            bool isOk = false;
            reply = QtRedisParser::parseRawDataList(replyData, &isOk);
            if (isOk)
                break;
        }
        if (!context->waitForReadyRead())
            return QList<QtRedisReply>();
    }
    for (const QtRedisReply &r : qAsConst(reply))
        this->checkCommandResult(context, command, r);
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
void QtRedisTransporter::checkCommandResult(QtRedisContext *context, const QStringList &command, const QtRedisReply &reply)
{
    if (command.isEmpty())
        return;

    // check select db
    if (command.size() == 2
        && command[0].toUpper() == QString("SELECT")
        && reply.type() == QtRedisReply::ReplyType::Status
        && reply.strValue() == QString("OK")) {
        context->setCurrentDbIndex(command[1].toInt());
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
void QtRedisTransporter::checkCommandResult(QtRedisContext *context, const QVariantList &command, const QtRedisReply &reply)
{
    if (command.isEmpty())
        return;

    // check select db
    if (command.size() == 2
        && command[0].toString().toUpper() == QString("SELECT")
        && reply.type() == QtRedisReply::ReplyType::Status
        && reply.strValue() == QString("OK")) {
        context->setCurrentDbIndex(command[1].toInt());
    }
}

//!
//! \brief Слот чтения входящих сооющений от сервера Redis
//!
void QtRedisTransporter::onReadyReadSub()
{
    QtRedisContext *context = qobject_cast<QtRedisContext*>(sender());
    if (!context)
        return;
    QByteArray replyData;
    const qint64 bytesAvailable = context->bytesAvailable();
    if (bytesAvailable <= 0)
        return;
    while (true) {
        replyData += context->readRawData();
        if (replyData.size() >= bytesAvailable)
            break;
    }
    if (replyData.isEmpty())
        return;
//    qDebug() << "===>" << replyData;
    bool isOk = false;
    const QList<QtRedisReply> replyList = QtRedisParser::parseRawDataList(replyData, &isOk);
    if (!isOk)
        return;
    for (const QtRedisReply &reply : replyList) {
        if (reply.type() == QtRedisReply::ReplyType::Array
            && reply.arrayValueSize() == 3
            && reply.arrayValue()[0].strValue() == "message")
            emit this->incomingChannelMessage(reply.arrayValue()[1].strValue(), reply.arrayValue()[2]);

        else if (reply.type() == QtRedisReply::ReplyType::Array
                 && reply.arrayValueSize() == 3
                 && reply.arrayValue()[0].strValue() == "smessage")
            emit this->incomingChannelShardMessage(reply.arrayValue()[1].strValue(), reply.arrayValue()[2]);

        else if (reply.type() == QtRedisReply::ReplyType::Array
                 && reply.arrayValueSize() == 4
                 && reply.arrayValue()[0].strValue() == "pmessage")
            emit this->incomingChannelPatternMessage(reply.arrayValue()[1].strValue(), reply.arrayValue()[2].strValue(), reply.arrayValue()[3]);
    }
}

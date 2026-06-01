#include "QtRedisTransaction.h"
#include <utility>

//!
//! \brief Конструктор класса
//! \param transporter Класс транспорта для работы с Redis
//! \param piped Флаг использования в режиме Pipeline
//!
QtRedisTransaction::QtRedisTransaction(std::shared_ptr<QtRedisTransporter> transporter, const bool piped)
    : QtRedisBase<QtRedisTransaction, bool>()
    , _transporter(transporter)
    , _piped(piped)
{
}

//!
//! \brief Деструктор класса
//!
QtRedisTransaction::~QtRedisTransaction()
{
    this->discardTransaction_unsafe();
}

//!
//! \brief Включить отслеживание ключа при условном выполнении транзакции
//! \param key Ключ
//! \return
//!
//! Redis command: WATCH
//!
//! Syntax
//!
//! WATCH key [key ...]
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1) for every key.
//! ACL categories:
//!     @fast, @transaction
//!
//! Note:
//! This command's behavior varies in clustered Redis environments. See the multi-key operations page for more information.
//!
//! Marks the given keys to be watched for conditional execution of a transaction.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisTransaction::watch(const QString &key)
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    // check is key already watching
    if (_watchList.contains(key.trimmed()))
        return true;
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(QtRedisCommand::fromString(QString("WATCH %1").arg(key).trimmed()), error, &isOk);
    if (!error.isEmpty())
        this->setLastError_safe(error);

    const bool res = QtRedisReply::replySimpleStringToBool(reply);
    if (res)
        _watchList.append(key.trimmed());
    return res;
}

//!
//! \brief Включить отслеживание списка ключей при условном выполнении транзакции
//! \param keys Список ключей
//! \return
//!
//! Redis command: WATCH
//!
//! Syntax
//!
//! WATCH key [key ...]
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1) for every key.
//! ACL categories:
//!     @fast, @transaction
//!
//! Note:
//! This command's behavior varies in clustered Redis environments. See the multi-key operations page for more information.
//!
//! Marks the given keys to be watched for conditional execution of a transaction.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisTransaction::watch(const QStringList &keys)
{
    QMutexLocker lock(&_mutex);
    if (keys.isEmpty()) {
        this->setLastError_safe("Invalid key list (Empty)!");
        return false;
    }
    QStringList tmpKeys;
    for (const QString &key : keys) {
        if (key.indexOf(" ") != -1) {
            this->setLastError_safe(QString("Invalid key (%1)!").arg(key));
            return false;
        }
        if (!_watchList.contains(key.trimmed()))
            tmpKeys.append(key.trimmed());
    }
    // check is keys already watching
    if (tmpKeys.isEmpty())
        return true;
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(QtRedisCommand::fromString(QString("WATCH %1").arg(tmpKeys.join(" ")).trimmed()), error, &isOk);
    if (!error.isEmpty())
        this->setLastError_safe(error);

    const bool res = QtRedisReply::replySimpleStringToBool(reply);
    if (res)
        _watchList.append(tmpKeys);
    return res;
}

//!
//! \brief Сборосить все ранее отслеживаемые ключи для данной транзакции
//! \return
//!
//! Redis command: UNWATCH
//!
//! Syntax
//!
//! UNWATCH
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast, @transaction
//!
//! Flushes all the previously watched keys for a transaction.
//!
//! If you call EXEC or DISCARD, there's no need to manually call UNWATCH.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisTransaction::unwatch()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(QtRedisCommand("UNWATCH"), error, &isOk);
    if (!error.isEmpty())
        this->setLastError_safe(error);

    _watchList.clear();
    return QtRedisReply::replySimpleStringToBool(reply);
}

//!
//! \brief Выполнить транзакцию
//! \return
//!
//! If piped == true:
//!     1. Open transaction by command "MULTI"
//!     2. Send append commands
//!     3. Execute transaction by command "EXEC"
//!
//! If piped == false:
//!     1. Execute transaction by command "EXEC"
//!
//!
//! Redis command: EXEC
//!
//! Syntax
//!
//! EXEC
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     Depends on commands in the transaction
//! ACL categories:
//!     @slow, @transaction
//!
//! Note:
//! This command's behavior varies in clustered Redis environments. See the multi-key operations page for more information.
//!
//! Executes all previously queued commands in a transaction and restores the connection state to normal.
//!
//! When using WATCH, EXEC will execute commands only if the watched keys were not modified, allowing for a check-and-set mechanism.
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Array reply: each element being the reply to each of the commands in the atomic transaction.
//!     Nil reply: the transaction was aborted because a WATCHed key was touched.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Array reply: each element being the reply to each of the commands in the atomic transaction.
//!     Null reply: the transaction was aborted because a WATCHed key was touched.
//!
QtRedisReply QtRedisTransaction::exec()
{
    QMutexLocker lock(&_mutex);
    if (_piped && _commandList.isEmpty()) {
        this->setLastError_safe("Commands list is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return QtRedisReply();
    }
    // open transaction
    if (!this->openTransaction_unsafe())
        return QtRedisReply();
    this->clearLastError_safe();
    // queued commands
    if (_piped) {
        QString error;
        bool isOk = false;
        const QtRedisReply reply = _transporter->sendCommands(_commandList, error, &isOk);
        _commandList.clear();
        if (!isOk) {
            this->setLastError_safe(error);
            this->discardTransaction_unsafe();
            return QtRedisReply();
        }
        if (!this->isReplyQueued(reply)) {
            this->setLastError_safe("Commands not queuede!");
            this->discardTransaction_unsafe();
            return QtRedisReply();
        }
    }
    // exec transaction
    QString error;
    bool isOk = false;
    const QtRedisReply replyExec = _transporter->sendCommand(QtRedisCommand("EXEC"), error, &isOk);
    if (!isOk)
        this->setLastError_safe(error);
    else if (replyExec.isNil())
        this->setLastError_safe(QString("Transaction aborted: Write collision detected via WATCH on keys [%1]! Optimistic lock failed!")
                                .arg(_watchList.join(", ")));
    else if (!error.isEmpty())
        this->setLastError_safe(error);

    // clear transaction flag
    _inTransaction = false;
    _watchList.clear();
    return replyExec;
}

//!
//! \brief Выполнить транзакцию
//! \return
//!
//! Note: This is a wrapper over function QtRedisTransaction::exec()
//!
bool QtRedisTransaction::execToBool()
{
    this->exec();
    return !this->hasLastError();
}

//!
//! \brief Отменить все внесенные изменения
//!
//! If piped == true:
//!     1. Clear commands list
//!     2. Clear watch list
//!
//! If piped == false:
//!     1. Clear commands list
//!     2. Clear watch list
//!     3. Discard transaction by command "DISCARD"
//!
void QtRedisTransaction::discard()
{
    QMutexLocker lock(&_mutex);
    _commandList.clear();
    _watchList.clear();
    if (!_piped)
        this->discardTransaction_unsafe();
}

//!
//! \brief Выполнить команду
//! \param command Команда
//! \return
//!
bool QtRedisTransaction::processCommand(const QtRedisCommand &command)
{
    if (!_piped) {
        if (!this->openTransaction_unsafe())
            return false;
        // queued command
        this->clearLastError_safe();
        QString error;
        bool isOk = false;
        const QtRedisReply reply = _transporter->sendCommand(command, error, &isOk);
        if (!isOk) {
            this->setLastError_safe(error);
            this->discardTransaction_unsafe();
            return false;
        }
        if (!this->isReplyQueued(reply)) {
            this->setLastError_safe("Commands not queuede!");
            this->discardTransaction_unsafe();
            return false;
        }
        return true;
    }
    // is piped -> add in command list
    _commandList.append(std::move(command));
    return true;
}

//!
//! \brief Проверка успешного добавления команды в очередь Redis
//! \param reply Ответ от сервера Redis
//! \return
//!
bool QtRedisTransaction::isReplyQueued(const QtRedisReply &reply)
{
    if (!reply.isStatus() && !reply.isArray())
        return false;

    // isStatus
    if (reply.isStatus())
        return (reply.strValue() == "QUEUED");
    // isArray
    for (const QtRedisReply &replyObj : reply.arrayValue_ref()) {
        if (!this->isReplyQueued(replyObj))
            return false;
    }
    return true;
}

//!
//! \brief Открыть транзацию в Redis командой "MULTI"
//! \return
//!
//! Redis command: MULTI
//!
//! Available since:
//!     Redis Open Source 1.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast, @transaction
//! Compatibility:
//!     Redis Software and Redis Cloud compatibility
//!
//! Note:
//! This command's behavior varies in clustered Redis environments. See the multi-key operations page for more information.
//!
//! Marks the start of a transaction block. Subsequent commands will be queued for atomic execution using EXEC.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisTransaction::openTransaction_unsafe()
{
    if (_inTransaction)
        return true;
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(QtRedisCommand("MULTI"), error, &isOk);
    if (!isOk)
        this->setLastError_safe(error);
    else if (QtRedisReply::replySimpleStringToBool(reply))
        _inTransaction = true;
    else if (!error.isEmpty())
        this->setLastError_safe(error);
    else
        this->setLastError_safe("Open transaction failed (invalid reply)!");

    return _inTransaction;
}

//!
//! \brief Отменить транзацию в Redis командой "DISCARD"
//! \return
//!
//! Redis command: DISCARD
//!
//! Available since:
//!     Redis Open Source 2.0.0
//! Time complexity:
//!     O(N), when N is the number of queued commands
//! ACL categories:
//!     @fast, @transaction
//! Compatibility:
//!     Redis Software and Redis Cloud compatibility
//!
//! Flushes all previously queued commands in a transaction and restores the connection state to normal.
//!
//! If WATCH was used, DISCARD unwatches all keys watched by the connection.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisTransaction::discardTransaction_unsafe()
{
    if (!_inTransaction)
        return true;
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(QtRedisCommand("DISCARD"), error, &isOk);
    if (!isOk)
        this->setLastError_safe(error);
    else if (QtRedisReply::replySimpleStringToBool(reply))
        _inTransaction = false;
    if (!error.isEmpty())
        this->setLastError_safe(error);
    else
        this->setLastError_safe("Discard transaction failed (invalid reply)!");

    return !_inTransaction;
}


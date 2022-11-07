#include "QtRedisClient.h"
#include <QDebug>

//!
//! \brief Деструктор класса
//!
QtRedisClient::~QtRedisClient()
{
    _lastError.clear();
    if (_transporter)
        delete _transporter;
}

QString QtRedisClient::libraryVersion()
{
    return QT_REDIS_CLIENT_VERSION_STR;
}

//!
//! \brief Последняя ошибка
//! \return
//!
QString QtRedisClient::lastError() const
{
    QMutexLocker lock(&_mutexErr);
    return _lastError;
}

//!
//! \brief Задана ли ошибка
//! \return
//!
bool QtRedisClient::hasLastError() const
{
    QMutexLocker lock(&_mutexErr);
    return !_lastError.isEmpty();
}

//!
//! \brief Установить последнюю ошибку
//! \param error Ошибка
//!
void QtRedisClient::setLastError(const QString &error)
{
    QMutexLocker lock(&_mutexErr);
    _lastError = error;
}

//!
//! \brief Очистить посленюю ошибку
//!
void QtRedisClient::clearLastError()
{
    QMutexLocker lock(&_mutexErr);
    _lastError.clear();
}

//!
//! \brief Проверить соединение с Redis
//! \return
//!
bool QtRedisClient::redisIsConnected()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return false;
    }
    return _transporter->isConnected();
}

//!
//! \brief Подключиться к серверу Redis
//! \param host IP-адрес
//! \param port Порт
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
//! Данный метод сипользует протокол TCP.
//!
bool QtRedisClient::redisConnect(const QString &host,
                                 const int port,
                                 const int timeOutMsec)
{
    QMutexLocker lock(&_mutex);
    if (host.isEmpty() || port == 0) {
        this->setLastError("Invalid host or port!");
        return false;
    }
    if (_transporter
        && _transporter->host() == host
        && _transporter->port() == port)
        return true;

    if (!_transporter)
        _transporter = new QtRedisTransporter();
    else
        _transporter->clearTransporter();

    _transporter->initTransporter(QtRedisTransporter::TransporterType::Tcp, host, port);
    return _transporter->connectToServer(timeOutMsec);
}

//!
//! \brief Подключиться к серверу Redis
//! \param host IP-адрес
//! \param port Порт
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
//! Данный метод сипользует протокол TCP-SSL.
//!
bool QtRedisClient::redisConnectEncrypted(const QString &host,
                                          const int port,
                                          const int timeOutMsec)
{
    QMutexLocker lock(&_mutex);
    if (host.isEmpty() || port == 0) {
        this->setLastError("Invalid host or port!");
        return false;
    }
    if (_transporter
        && _transporter->host() == host
        && _transporter->port() == port
        && _transporter->type() == QtRedisTransporter::TransporterType::Ssl)
        return true;

    if (!_transporter)
        _transporter = new QtRedisTransporter();
    else
        _transporter->clearTransporter();

    _transporter->initTransporter(QtRedisTransporter::TransporterType::Ssl, host, port);
    return _transporter->connectToServer(timeOutMsec);
}

#if defined(Q_OS_LINUX)
//!
//! \brief Подключиться к серверу Redis
//! \param sockPath Полный путь до unix-сокета
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
//! Данный метод сипользует UNIX-sockets.
//!
bool QtRedisClient::redisConnectUnix(const QString &sockPath, const int timeOutMsec)
{
    QMutexLocker lock(&_mutex);
    if (sockPath.isEmpty()) {
        this->setLastError("Invalid sockPath!");
        return false;
    }
    if (_transporter
        && _transporter->host() == sockPath
        && _transporter->type() == QtRedisTransporter::TransporterType::Unix)
        return true;

    if (!_transporter)
        _transporter = new QtRedisTransporter();
    else
        _transporter->clearTransporter();

    _transporter->initTransporter(QtRedisTransporter::TransporterType::Unix, sockPath, -1);
    return _transporter->connectToServer(timeOutMsec);
}
#endif

//!
//! \brief Переподключиться к серверу Redis
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
bool QtRedisClient::redisReconnect(const int timeOutMsec)
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return false;
    }
    return _transporter->reconnectToServer(timeOutMsec);
}

//!
//! \brief Отключиться от сервера Redis
//!
void QtRedisClient::redisDisconnect()
{
    QMutexLocker lock(&_mutex);
    if (_transporter)
        _transporter->clearTransporter();

    this->clearLastError();
}


// ------------------------------------------------------------------------
// -- BASE COMMANDS -------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Выпонить команду
//! \param command Команда
//! \return
//!
QtRedisReply QtRedisClient::redisExecCommand(const QString &command)
{
    QMutexLocker lock(&_mutex);
    if (command.isEmpty()) {
        this->setLastError("Command is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError("Client is not connected!");
        return QtRedisReply();
    }
    return _transporter->sendCommand(command.split(" "));
}

//!
//! \brief Выпонить команду (QByteArray)
//! \param command Команда
//! \return
//!
QtRedisReply QtRedisClient::redisExecCommand(const QByteArray &command)
{
    QMutexLocker lock(&_mutex);
    if (command.isEmpty()) {
        this->setLastError("Command is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError("Client is not connected!");
        return QtRedisReply();
    }
    QVariantList cmdList;
    const QList<QByteArray> tmpCmd = command.split(' ');
    for (const QByteArray &ba : tmpCmd)
        cmdList.append(ba);

    return _transporter->sendCommand(cmdList);
}

//!
//! \brief Выполнить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
QtRedisReply QtRedisClient::redisExecCommandArgv(const QStringList &commandArgv)
{
    QMutexLocker lock(&_mutex);
    if (commandArgv.isEmpty()) {
        this->setLastError("CommandList is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError("Client is not connected!");
        return QtRedisReply();
    }
    return _transporter->sendCommand(commandArgv);
}

//!
//! \brief Выполнить команду (QByteArray)
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
QtRedisReply QtRedisClient::redisExecCommandArgv(const QList<QByteArray> &commandArgv)
{
    QMutexLocker lock(&_mutex);
    if (commandArgv.isEmpty()) {
        this->setLastError("CommandList is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError("Client is not connected!");
        return QtRedisReply();
    }
    QVariantList cmdList;
    for (const QByteArray &ba : commandArgv)
        cmdList.append(ba);
    return _transporter->sendCommand(cmdList);
}

//!
//! \brief Проверить команду
//! \param command Команда
//! \return
//!
//! Данный метод проверяет тип результирующего объекта, и если он:
//! - replyType_Error   - return false
//! - replyType_Nil     - return false
//! - replyType_Status  - если ответ != "OK", то return false
//!
bool QtRedisClient::redisCheckCommand(const QString &command)
{
    QtRedisReply buffReply = this->redisExecCommand(command);
    if (buffReply.type() == QtRedisReply::ReplyType::Error
        || buffReply.type() == QtRedisReply::ReplyType::Nil)
        return false;
    if (buffReply.type() == QtRedisReply::ReplyType::Status
        && buffReply.strValue() != "OK")
        return false;

    return true;
}

//!
//! \brief Проверить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
//! Данный метод проверяет тип результирующего объекта, и если он:
//! - replyType_Error   - return false
//! - replyType_Nil     - return false
//! - replyType_Status  - если ответ != "OK", то return false
//!
bool QtRedisClient::redisCheckCommandArgv(const QStringList &commandArgv)
{
    QtRedisReply buffReply = this->redisExecCommandArgv(commandArgv);
    if (buffReply.type() == QtRedisReply::ReplyType::Error
        || buffReply.type() == QtRedisReply::ReplyType::Nil)
        return false;
    if (buffReply.type() == QtRedisReply::ReplyType::Status
        && buffReply.strValue() != "OK")
        return false;

    return true;
}


// ------------------------------------------------------------------------
// -- SERVER COMMANDS -----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Метод авторизации
//! \param password Пароль
//! \return
//!
//! Если авторизация не требуется - возвращает false и сообщение об ошибке, что авторизация не требуется.
//!
//! Request for authentication in a password-protected Redis server. Redis can be instructed to require a password before allowing clients to execute commands.
//! This is done using the requirepass directive in the configuration file.
//!
//! If password matches the password in the configuration file, the server replies with the OK status code and starts accepting commands.
//! Otherwise, an error is returned and the clients needs to try a new password.
//!
//! Note: because of the high performance nature of Redis, it is possible to try a lot of passwords in parallel in very short time,
//! so make sure to generate a strong and very long password so that this attack is infeasible.
//!
bool QtRedisClient::redisAuth(const QString &password)
{
    QStringList argv;
    argv << "AUTH" << password;
    return this->replySimpleStringToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Выпонить команду PING
//! \param msg Дополнительное сообщение
//! \return
//!
bool QtRedisClient::redisPing(const QString &msg)
{
    QStringList argv;
    argv << "PING";
    if (!msg.isEmpty())
        argv << msg;

    QtRedisReply buffReply = this->redisExecCommandArgv(argv);
    if (msg.isEmpty()) {
        if (buffReply.type() != QtRedisReply::ReplyType::Status) {
            this->setLastError("Invalid reply type!");
            return false;
        }
        if (buffReply.strValue() != "PONG") {
            this->setLastError("Invalid reply result!");
            return false;
        }
    } else {
        if (buffReply.type() != QtRedisReply::ReplyType::String) {
            this->setLastError("Invalid reply type!");
            return false;
        }
        if (buffReply.strValue() != msg) {
            this->setLastError("Invalid reply result!");
            return false;
        }
    }
    return true;
}

//!
//! \brief Выполнить команду ECHO
//! \param msg Сообщение
//! \return
//!
QtRedisReply QtRedisClient::redisEcho(const QString &msg)
{
    QStringList argv;
    argv << "ECHO" << msg;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Информация сервера
//! \return
//!
//! The INFO command returns information and statistics about the server in a format that is simple to parse by computers and easy to read by humans.
//!
//! The optional parameter can be used to select a specific section of information:
//! - server: General information about the Redis server
//! - clients: Client connections section
//! - memory: Memory consumption related information
//! - persistence: RDB and AOF related information
//! - stats: General statistics
//! - replication: Master/slave replication information
//! - cpu: CPU consumption statistics
//! - commandstats: Redis command statistics
//! - cluster: Redis Cluster section
//! - keyspace: Database related statistics
//!
//! It can also take the following values:
//! - all: Return all sections
//! - default: Return only the default set of sections
//!
QMap<QString, QVariant> QtRedisClient::redisInfo(const QString &section)
{
    if (!section.isEmpty()
        && section.toLower() != QString("server")
        && section.toLower() != QString("clients")
        && section.toLower() != QString("memory")
        && section.toLower() != QString("persistence")
        && section.toLower() != QString("stats")
        && section.toLower() != QString("replication")
        && section.toLower() != QString("cpu")
        && section.toLower() != QString("commandstats")
        && section.toLower() != QString("cluster")
        && section.toLower() != QString("keyspace")
        && section.toLower() != QString("all")
        && section.toLower() != QString("default")) {
        this->setLastError("Invalid section!");
        return QMap<QString, QVariant>();
    }

    QString command("INFO");
    if (!section.isEmpty())
        command += QString(" %1").arg(section.toLower());

    QtRedisReply buffReply = this->redisExecCommand(command.trimmed());
    if (buffReply.type() != QtRedisReply::ReplyType::String) {
        this->setLastError("Invalid reply type!");
        return QMap<QString, QVariant>();
    }

    QMap<QString, QVariant> buffInfo;
    QStringList buffReplyList = buffReply.strValue().split("\n");
    for (int i = 0; i < buffReplyList.size(); i++) {
        buffReplyList[i] = buffReplyList[i].trimmed();
        if (buffReplyList[i].isEmpty())
            continue;

        QStringList buffSplitValue = buffReplyList[i].split(":");
        if (buffSplitValue.size() < 2)
            continue;
        buffInfo.insert(buffSplitValue[0], buffSplitValue[1]);
    }
    return buffInfo;
}

//!
//! \brief Текущее время сервера
//! \return
//!
//! The TIME command returns the current server time as a two items lists:
//! a Unix timestamp and the amount of microseconds already elapsed in the current second.
//! 1) "1507794780"
//! 2) "374706"
//!
QtRedisReply QtRedisClient::redisTime()
{
    return this->redisExecCommand(QString("TIME"));
}

//!
//! \brief Выбрать текущую БД Redis
//! \param dbIndex Индекс БД
//! \return
//!
//! Select the Redis logical database having the specified zero-based numeric index. New connections always use the database 0.
//!
//! Redis different selectable databases are a form of namespacing: all the databases are anyway persisted together in the same RDB / AOF file.
//! However different databases can have keys having the same name, and there are commands available like FLUSHDB, SWAPDB or RANDOMKEY that work on specific databases.
//!
//! In practical terms, Redis databases should mainly used in order to, if needed, separate different keys belonging to the same application,
//! and not in order to use a single Redis instance for multiple unrelated applications.
//!
//! When using Redis Cluster, the SELECT command cannot be used, since Redis Cluster only supports database zero.
//! In the case of Redis Cluster, having multiple databases would be useless, and a worthless source of complexity, because anyway commands operating atomically
//! on a single database would not be possible with the Redis Cluster design and goals.
//!
//! Since the currently selected database is a property of the connection, clients should track the currently selected database and re-select it on reconnection.
//! While there is no command in order to query the selected database in the current connection, the CLIENT LIST output shows, for each client, the currently selected database.
//!
bool QtRedisClient::redisSelect(const int dbIndex)
{
    if (dbIndex < 0) {
        this->setLastError("Invalid db index!");
        return false;
    }
    return this->replySimpleStringToBool(this->redisExecCommand(QString("SELECT %1").arg(dbIndex)));
}

//!
//! \brief Индекс текущей БД
//! \return
//!
int QtRedisClient::redisSelectedDb()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError("QtRedisTransporter is NULL!");
        return -1;
    }
    if (!_transporter->isConnected()) {
        this->setLastError("Client is not connected!");
        return -1;
    }
    return _transporter->currentDbIndex();
}

//!
//! \brief Количество ключей в текущей БД
//! \return
//!
qlonglong QtRedisClient::redisDbSize()
{
    QtRedisReply buffReply = this->redisExecCommand(QString("DBSIZE"));
    if (buffReply.type() != QtRedisReply::ReplyType::Integer) {
        this->setLastError("Invalid reply type!");
        return -1;
    }
    return buffReply.intValue();
}

//!
//! \brief Удалить все ключи и их значения из всех БД
//! \param async
//! \return
//!
//! Delete all the keys of all the existing databases, not just the currently selected one. This command never fails.
//! The time-complexity for this operation is O(N), N being the number of keys in all existing databases.
//!
//! FLUSHALL ASYNC (Redis 4.0.0 or greater)
//! Redis is now able to delete keys in the background in a different thread without blocking the server.
//! An ASYNC option was added to FLUSHALL and FLUSHDB in order to let the entire dataset or a single database to be freed asynchronously.
//!
bool QtRedisClient::redisFlushAll(const bool async)
{
    QString command("FLUSHALL");
    if (async)
        command += QString(" ASYNC");
    return this->replySimpleStringToBool(this->redisExecCommand(command));
}

//!
//! \brief Удалить все ключи и их значения из текущей БД
//! \param async
//! \return
//!
//! Delete all the keys of the currently selected DB. This command never fails.
//! The time-complexity for this operation is O(N), N being the number of keys in the database.
//!
//! FLUSHDB ASYNC (Redis 4.0.0 or greater)
//! See FLUSHALL for documentation.
//!
bool QtRedisClient::redisFlushDb(const bool async)
{
    QString command("FLUSHDB");
    if (async)
        command += QString(" ASYNC");
    return this->replySimpleStringToBool(this->redisExecCommand(command));
}


// -- SAVE-methods --

//!
//! \brief Синхронное сохранение данных
//! \return
//!
//! The SAVE commands performs a synchronous save of the dataset producing a point in time snapshot of all the data inside the Redis instance, in the form of an RDB file.
//! You almost never want to call SAVE in production environments where it will block all the other clients. Instead usually BGSAVE is used.
//! However in case of issues preventing Redis to create the background saving child (for instance errors in the fork(2) system call),
//! the SAVE command can be a good last resort to perform the dump of the latest dataset.
//!
bool QtRedisClient::redisSave()
{
    return this->replySimpleStringToBool(this->redisExecCommand(QString("SAVE")));
}

//!
//! \brief Сохранение данных в фоновом режиме
//! \return
//!
//! Save the DB in background. The OK code is immediately returned. Redis forks, the parent continues to serve the clients, the child saves the DB on disk then exits.
//! A client may be able to check if the operation succeeded using the LASTSAVE command.
//!
QtRedisReply QtRedisClient::redisBgSave()
{
    return this->redisExecCommand(QString("BGSAVE"));
}

//!
//! \brief Время последнего сохранения (utc)
//! \return
//!
//! Return the UNIX TIME of the last DB save executed with success. A client may check if a BGSAVE command succeeded reading the LASTSAVE value,
//! then issuing a BGSAVE command and checking at regular intervals every N seconds if LASTSAVE changed.
//!
uint QtRedisClient::redisLastSave()
{
    QtRedisReply buffReply = this->redisExecCommand(QString("LASTSAVE"));
    if (buffReply.type() != QtRedisReply::ReplyType::Integer) {
        this->setLastError("Invalid reply type!");
        return 0;
    }
    return buffReply.intValue();
}


// -- CONFIG-methods --

//!
//! \brief Получить значения из конфигурационного файла
//! \param param Параметр (или строка поиска)
//! \return
//!
//! The CONFIG GET command is used to read the configuration parameters of a running Redis server. Not all the configuration parameters are supported in Redis 2.4,
//! while Redis 2.6 can read the whole configuration of a server using this command.
//! The symmetric command used to alter the configuration at run time is CONFIG SET.
//! CONFIG GET takes a single argument, which is a glob-style pattern. All the configuration parameters matching this parameter are reported as a list of key-value pairs.
//! Example:
//! redis> config get *max-*-entries*
//! 1) "hash-max-zipmap-entries"
//! 2) "512"
//! 3) "list-max-ziplist-entries"
//! 4) "512"
//! 5) "set-max-intset-entries"
//! 6) "512"
//!
QtRedisReply QtRedisClient::redisConfigGet(const QString &param)
{
    if (param.isEmpty() || param.indexOf(" ") != -1) {
        this->setLastError("Invalid param!");
        return QtRedisReply();
    }
    return this->redisExecCommand(QString("CONFIG GET %1").arg(param));
}

//!
//! \brief Задать новое значение конфигурационного параметра
//! \param param Параметр
//! \param value Значение
//! \return
//!
//! The CONFIG SET command is used in order to reconfigure the server at run time without the need to restart Redis.
//! You can change both trivial parameters or switch from one to another persistence option using this command.
//!
//! The list of configuration parameters supported by CONFIG SET can be obtained issuing a CONFIG GET * command, that is the symmetrical command used to obtain information about the
//! configuration of a running Redis instance.
//!
//! All the configuration parameters set using CONFIG SET are immediately loaded by Redis and will take effect starting with the next command executed.
//! All the supported parameters have the same meaning of the equivalent configuration parameter used in the redis.conf file, with the following important differences:
//! - In options where bytes or other quantities are specified, it is not possible to use the redis.conf abbreviated form (10k, 2gb ... and so forth),
//!   everything should be specified as a well-formed 64-bit integer, in the base unit of the configuration directive.
//!   However since Redis version 3.0 or greater, it is possible to use CONFIG SET with memory units for maxmemory, client output buffers, and replication backlog size.
//! - The save parameter is a single string of space-separated integers. Every pair of integers represent a seconds/modifications threshold.
//!
//! For instance what in redis.conf looks like:
//! save 900 1
//! save 300 10
//!
//! that means, save after 900 seconds if there is at least 1 change to the dataset, and after 300 seconds if there are at least 10 changes to the dataset,
//! should be set using CONFIG SET SAVE "900 1 300 10".
//!
//! It is possible to switch persistence from RDB snapshotting to append-only file (and the other way around) using the CONFIG SET command.
//! For more information about how to do that please check the persistence page.
//! In general what you should know is that setting the appendonly parameter to yes will start a background process to save the initial append-only file (obtained from the in memory data set),
//! and will append all the subsequent commands on the append-only file, thus obtaining exactly the same effect of a Redis server that started with AOF turned on since the start.
//! You can have both the AOF enabled with RDB snapshotting if you want, the two options are not mutually exclusive.
//!
bool QtRedisClient::redisConfigSet(const QString &param, const QString &value)
{
    if (param.isEmpty() || param.indexOf(" ") != -1) {
        this->setLastError("Invalid param!");
        return false;
    }
    QStringList argv;
    argv << "CONFIG SET" << param << value;
    return this->replySimpleStringToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Перезаписать redis.conf
//! \return
//!
//! The CONFIG REWRITE command rewrites the redis.conf file the server was started with, applying the minimal changes needed to make it reflect the configuration currently used by the server,
//! which may be different compared to the original one because of the use of the CONFIG SET command.
//!
//! The rewrite is performed in a very conservative way:
//! - Comments and the overall structure of the original redis.conf are preserved as much as possible.
//! - If an option already exists in the old redis.conf file, it will be rewritten at the same position (line number).
//! - If an option was not already present, but it is set to its default value, it is not added by the rewrite process.
//! - If an option was not already present, but it is set to a non-default value, it is appended at the end of the file.
//! - Non used lines are blanked. For instance if you used to have multiple save directives, but the current configuration has fewer or none as you disabled RDB persistence,
//!   all the lines will be blanked.
//!
//! CONFIG REWRITE is also able to rewrite the configuration file from scratch if the original one no longer exists for some reason.
//! However if the server was started without a configuration file at all, the CONFIG REWRITE will just return an error.
//!
bool QtRedisClient::redisConfigReWrite()
{
    return this->replySimpleStringToBool(this->redisExecCommand(QString("CONFIG REWRITE")));
}

//!
//! \brief Сбросить статистику сервера
//! \return
//!
//! Resets the statistics reported by Redis using the INFO command.
//! These are the counters that are reset:
//! - Keyspace hits
//! - Keyspace misses
//! - Number of commands processed
//! - Number of connections received
//! - Number of expired keys
//! - Number of rejected connections
//! - Latest fork(2) time
//! - The aof_delayed_fsync counter
//!
bool QtRedisClient::redisConfigResetStat()
{
    return this->replySimpleStringToBool(this->redisExecCommand(QString("CONFIG RESETSTAT")));
}


// -- CLIENT-methods --

//!
//! \brief Список подключенный к серверу клиентов
//! \return
//!
//! The CLIENT LIST command returns information and statistics about the client connections server in a mostly human readable format.
//! Here is the meaning of the fields:
//! - id: an unique 64-bit client ID (introduced in Redis 2.8.12).
//! - addr: address/port of the client
//! - fd: file descriptor corresponding to the socket
//! - age: total duration of the connection in seconds
//! - idle: idle time of the connection in seconds
//! - flags: client flags (see below)
//! - db: current database ID
//! - sub: number of channel subscriptions
//! - psub: number of pattern matching subscriptions
//! - multi: number of commands in a MULTI/EXEC context
//! - qbuf: query buffer length (0 means no query pending)
//! - qbuf-free: free space of the query buffer (0 means the buffer is full)
//! - obl: output buffer length
//! - oll: output list length (replies are queued in this list when the buffer is full)
//! - omem: output buffer memory usage
//! - events: file descriptor events (see below)
//! - cmd: last command played
//!
QVector<QtRedisClientInfo> QtRedisClient::redisClientList()
{
    QtRedisReply buffReply = this->redisExecCommand(QString("CLIENT LIST"));
    if (buffReply.type() != QtRedisReply::ReplyType::String) {
        this->setLastError("Invalid reply type!");
        return QVector<QtRedisClientInfo>();
    }

    QVector<QtRedisClientInfo> buffList;
    QStringList buffReplyList = buffReply.strValue().split("\n");
    for (int i = 0; i < buffReplyList.size(); i++) {
        buffReplyList[i] = buffReplyList[i].trimmed();
        if (buffReplyList[i].isEmpty())
            continue;

        QMap <QString, QVariant> buffInfo;
        QStringList buffSplitValue = buffReplyList[i].split(" ");
        for (const QString &str : buffSplitValue) {
            QStringList strSplit = str.split("=");
            if (strSplit.size() != 2)
                continue;
            buffInfo.insert(strSplit[0], strSplit[1]);
        }
        buffList.append(QtRedisClientInfo(buffInfo));
    }
    return buffList;
}

//!
//! \brief Задать имя для текущего соединения
//! \param connectionName Имя
//! \return
//!
//! The CLIENT SETNAME command assigns a name to the current connection.
//! The assigned name is displayed in the output of CLIENT LIST so that it is possible to identify the client that performed a given connection.
//! For instance when Redis is used in order to implement a queue, producers and consumers of messages may want to set the name of the connection according to their role.
//! There is no limit to the length of the name that can be assigned if not the usual limits of the Redis string type (512 MB).
//! However it is not possible to use spaces in the connection name as this would violate the format of the CLIENT LIST reply.
//!
//! It is possible to entirely remove the connection name setting it to the empty string, that is not a valid connection name since it serves to this specific purpose.
//! The connection name can be inspected using CLIENT GETNAME.
//! Every new connection starts without an assigned name.
//! Tip: setting names to connections is a good way to debug connection leaks due to bugs in the application using Redis.
//!
bool QtRedisClient::redisClientSetName(const QString &connectionName)
{
    if (connectionName.isEmpty() || connectionName.indexOf(" ") != -1) {
        this->setLastError("Invalid connection name!");
        return false;
    }
    return this->replySimpleStringToBool(this->redisExecCommand(QString("CLIENT SETNAME %1").arg(connectionName)));
}

//!
//! \brief Имя для текущего соединения (если задано)
//! \return
//!
//! The CLIENT GETNAME returns the name of the current connection as set by CLIENT SETNAME.
//! Since every new connection starts without an associated name, if no name was assigned a null bulk reply is returned.
//!
QString QtRedisClient::redisClientGetName()
{
    return this->replyToString(this->redisExecCommand(QString("CLIENT GETNAME")));
}

//!
//! \brief Закрыть соединение с клиентом
//! \param ip IP-адрес
//! \param port Порт
//! \return
//!
bool QtRedisClient::redisClientKill(const QString &ip, const uint port)
{
    if (ip.isEmpty()) {
        this->setLastError("Invalid IP!");
        return false;
    }
    if (port == 0) {
        this->setLastError("Invalid port!");
        return false;
    }
    QStringList argv;
    argv << "CLIENT" << "KILL" << "ADDR" << QString("%1:%2").arg(ip).arg(port);
    return this->replyIntToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Закрыть соединение с клиентом
//! \param id ID клиента
//! \return
//!
bool QtRedisClient::redisClientKill(const QString &id)
{
    if (id.isEmpty()) {
        this->setLastError("Invalid ID!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("CLIENT KILL ID %1").arg(id)));
}


// ------------------------------------------------------------------------
// -- KEY-VALUE COMMANDS --------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Список ключей в БД
//! \param arg Условие поиска
//! \return
//!
QStringList QtRedisClient::redisKeys(const QString &arg)
{
    return this->replyToArray(this->redisExecCommand(QString("KEYS %1").arg(arg)));
}

//!
//! \brief Случайный ключ из текущей БД
//! \return
//!
QString QtRedisClient::redisRandomKey()
{
    return this->replyToString(this->redisExecCommand(QString("RANDOMKEY")));
}

//!
//! \brief Проверка наличия ключей в БД
//! \param keyList Список ключей
//! \return
//!
//! Since Redis 3.0.3 it is possible to specify multiple keys instead of a single one. In such a case, it returns the total number of keys existing.
//! Note that returning 1 or 0 for a single key is just a special case of the variadic usage, so the command is completely backward compatible.
//!
//! The user should be aware that if the same existing key is mentioned in the arguments multiple times, it will be counted multiple times.
//! So if somekey exists, EXISTS somekey somekey will return 2.
//!
//! Integer reply, specifically:
//! - 1 if the key exists.
//! - 0 if the key does not exist.
//!
//! Since Redis 3.0.3 the command accepts a variable number of keys and the return value is generalized:
//! - The number of keys existing among the ones specified as arguments. Keys mentioned multiple times and existing are counted multiple times.
//!
qlonglong QtRedisClient::redisExists(const QStringList &keyList)
{
    if (keyList.isEmpty()) {
        this->setLastError("Invalid key list (Empty)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    return this->replyToLong(this->redisExecCommand(QString("EXISTS %1").arg(keyList.join(" ")).trimmed()));
}

//!
//! \brief Получить значение по ключу
//! \param key Ключ
//! \return
//!
//! Get the value of key. If the key does not exist the special value nil is returned.
//! An error is returned if the value stored at key is not a string, because GET only handles string values.
//!
QtRedisReply QtRedisClient::redisGet(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    return this->redisExecCommand(QString("GET %1").arg(key));
}

//!
//! \brief Получить подстроку значения в указанных диапазонах
//! \param key Ключ
//! \param startPos Начало
//! \param endPos Конец
//! \return
//!
//! Warning: this command was renamed to GETRANGE, it is called SUBSTR in Redis versions <= 2.0.
//! Returns the substring of the string value stored at key, determined by the offsets start and end (both are inclusive).
//! Negative offsets can be used in order to provide an offset starting from the end of the string. So -1 means the last character, -2 the penultimate and so forth.
//! The function handles out of range requests by limiting the resulting range to the actual length of the string.
//! Examples:
//! redis>  SET mykey "This is a string"
//! "OK"
//! redis>  GETRANGE mykey 0 3
//! "This"
//! redis>  GETRANGE mykey -3 -1
//! "ing"
//! redis>  GETRANGE mykey 0 -1
//! "This is a string"
//! redis>  GETRANGE mykey 10 100
//! "string"
//! redis>
//!
QString QtRedisClient::redisGetRange(const QString &key, const int startPos, const int endPos)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("GETRANGE %1 %2 %3").arg(key).arg(startPos).arg(endPos)));
}

//!
//! \brief Получить значение ключа и задать новое
//! \param key Ключ
//! \param value Новое значение
//! \return
//!
//! Atomically sets key to value and returns the old value stored at key. Returns an error when key exists but does not hold a string value.
//!
//! GETSET can be used together with INCR for counting with atomic reset. For example: a process may call INCR against the key mycounter every time some event occurs,
//! but from time to time we need to get the value of the counter and reset it to zero atomically.
//! This can be done using GETSET mycounter "0":
//! redis>  INCR mycounter
//! (integer) 1
//! redis>  GETSET mycounter "0"
//! "1"
//! redis>  GET mycounter
//! "0"
//! redis>
//!
QtRedisReply QtRedisClient::redisGetSet(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QStringList argv;
    argv << "GETSET" << key << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Добавить/задать значение ключу
//! \param key Ключ
//! \param appendValue Значение
//! \return
//!
//! If key already exists and is a string, this command appends the value at the end of the string.
//! If key does not exist it is created and set as an empty string, so APPEND will be similar to SET in this special case.
//!
//! Examples
//! redis>  EXISTS mykey
//! (integer) 0
//! redis>  APPEND mykey "Hello"
//! (integer) 5
//! redis>  APPEND mykey " World"
//! (integer) 11
//! redis>  GET mykey
//! "Hello World"
//! redis>
//!
qlonglong QtRedisClient::redisAppend(const QString &key, const QString &appendValue)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "APPEND" << key << appendValue;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Задать значение ключу
//! \param key Ключ
//! \param value Значение
//! \param exSec Время "жизни" объекта в сек.
//! \param pxMSec Время "жизни" объекта в мсек.
//! \param existFlag Флаг проверки существования объекта (NX|XX)
//! \return
//!
//! Если заданы exSec и(или) pxMSec, то по истечении времени объект будет автоматически удален.
//!
//! Set key to hold the string value. If key already holds a value, it is overwritten, regardless of its type. Any previous time to live associated with the key is discarded on successful SET operation.
//! Options
//!
//! Starting with Redis 2.6.12 SET supports a set of options that modify its behavior:
//! - EX seconds -- Set the specified expire time, in seconds.
//! - PX milliseconds -- Set the specified expire time, in milliseconds.
//! - NX -- Only set the key if it does not already exist.
//! - XX -- Only set the key if it already exist.
//!
bool QtRedisClient::redisSet(const QString &key,
                             const QString &value,
                             const uint exSec,
                             const uint pxMSec,
                             const QString existFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (!existFlag.isEmpty()
        && existFlag.toUpper() != QString("NX")
        && existFlag.toUpper() != QString("XX")) {
        this->setLastError("Invalid existFlag!");
        return false;
    }
    QStringList argv;
    argv << "SET" << key << value;
    if (exSec > 0)
        argv << "EX" << QString::number(exSec);
    if (pxMSec > 0)
        argv << "PX" << QString::number(pxMSec);
    if (!existFlag.isEmpty())
        argv << existFlag.toUpper();

    return this->replySimpleStringToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Перезаписать часть строки значения на новое, начиная с позиции
//! \param key Ключ
//! \param value Новое значение
//! \param offset Позиция
//! \return
//!
//! Overwrites part of the string stored at key, starting at the specified offset, for the entire length of value.
//! If the offset is larger than the current length of the string at key, the string is padded with zero-bytes to make offset fit.
//! Non-existing keys are considered as empty strings, so this command will make sure it holds a string large enough to be able to set value at offset.
//!
//! Note that the maximum offset that you can set is 229 -1 (536870911), as Redis Strings are limited to 512 megabytes.
//! If you need to grow beyond this size, you can use multiple keys.
//!
qlonglong QtRedisClient::redisSetRange(const QString &key, const QString &value, const int offset)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "SETRANGE" << key << QString::number(offset) << value;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Удалить ключи и их значения из БД
//! \param keyList Список ключей
//! \return
//!
//! Removes the specified keys. A key is ignored if it does not exist.
//! Return value: The number of keys that were removed.
//! Examples
//! redis>  SET key1 "Hello"
//! "OK"
//! redis>  SET key2 "World"
//! "OK"
//! redis>  DEL key1 key2 key3
//! (integer) 2
//! redis>
//!
qlonglong QtRedisClient::redisDel(const QStringList &keyList)
{
    if (keyList.isEmpty()) {
        this->setLastError("Invalid key list (Empty)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    return this->replyToLong(this->redisExecCommand(QString("DEL %1").arg(keyList.join(" ")).trimmed()));
}

//!
//! \brief Получить длину строки значения
//! \param key Ключ
//! \return
//!
//! Returns the length of the string value stored at key. An error is returned when key holds a non-string value.
//! Return value: the length of the string at key, or 0 when key does not exist.
//! Examples
//! redis>  SET mykey "Hello world"
//! "OK"
//! redis>  STRLEN mykey
//! (integer) 11
//! redis>  STRLEN nonexisting
//! (integer) 0
//! redis>
//!
qlonglong QtRedisClient::redisStrlen(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return 0;
    }
    return this->replyToLong(this->redisExecCommand(QString("STRLEN %1").arg(key)));
}

//!
//! \brief Задать время "жизни" объекта в сек.
//! \param key Ключ
//! \param sec Время в сек.
//! \return
//!
//! Set a timeout on key. After the timeout has expired, the key will automatically be deleted. A key with an associated timeout is often said to be volatile in Redis terminology.
//!
//! The timeout will only be cleared by commands that delete or overwrite the contents of the key, including DEL, SET, GETSET and all the *STORE commands.
//! This means that all the operations that conceptually alter the value stored at the key without replacing it with a new one will leave the timeout untouched.
//! For instance, incrementing the value of a key with INCR, pushing a new value into a list with LPUSH, or altering the field value of a hash with HSET are all operations
//! that will leave the timeout untouched.
//!
//! The timeout can also be cleared, turning the key back into a persistent key, using the PERSIST command.
//! If a key is renamed with RENAME, the associated time to live is transferred to the new key name.
//! If a key is overwritten by RENAME, like in the case of an existing key Key_A that is overwritten by a call like RENAME Key_B Key_A,
//! it does not matter if the original Key_A had a timeout associated or not, the new key Key_A will inherit all the characteristics of Key_B.
//!
//! Refreshing expires
//! It is possible to call EXPIRE using as argument a key that already has an existing expire set. In this case the time to live of a key is updated to the new value.
//! There are many useful applications for this, an example is documented in the Navigation session pattern section below.
//!
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXPIRE mykey 10
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 10
//! redis>  SET mykey "Hello World"
//! "OK"
//! redis>  TTL mykey
//! (integer) -1
//! redis>
//!
bool QtRedisClient::redisExpire(const QString &key, const uint sec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (sec == 0) {
        this->setLastError("Invalid sec!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("EXPIRE %1 %2").arg(key).arg(sec)));
}

//!
//! \brief Задать время "жизни" объекта в формате utc (сек)
//! \param key Ключ
//! \param utcSec Время в формате utc (сек)
//! \return
//!
//! EXPIREAT has the same effect and semantic as EXPIRE, but instead of specifying the number of seconds representing the TTL (time to live),
//! it takes an absolute Unix timestamp (seconds since January 1, 1970). A timestamp in the past will delete the key immediately.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXISTS mykey
//! (integer) 1
//! redis>  EXPIREAT mykey 1293840000
//! (integer) 1
//! redis>  EXISTS mykey
//! (integer) 0
//! redis>
//!
bool QtRedisClient::redisExpireAt(const QString &key, const uint utcSec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (utcSec == 0) {
        this->setLastError("Invalid utcSec!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("EXPIREAT %1 %2").arg(key).arg(utcSec)));
}

//!
//! \brief Задать время "жизни" объекта в мсек.
//! \param key Ключ
//! \param msec Время в мсек
//! \return
//!
//! This command works exactly like EXPIRE but the time to live of the key is specified in milliseconds instead of seconds.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  PEXPIRE mykey 1500
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 1
//! redis>  PTTL mykey
//! (integer) 1494
//! redis>
//!
bool QtRedisClient::redisPExpire(const QString &key, const uint msec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("PEXPIRE %1 %2").arg(key).arg(msec)));
}

//!
//! \brief Задать время "жизни" объекта в формате utc (мсек)
//! \param key Ключ
//! \param utcMsec Время в формате utc (мсек)
//! \return
//!
//! PEXPIREAT has the same effect and semantic as EXPIREAT, but the Unix time at which the key will expire is specified in milliseconds instead of seconds.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  PEXPIREAT mykey 1555555555005
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 47755408
//! redis>  PTTL mykey
//! (integer) 47755408215
//! redis>
//!
bool QtRedisClient::redisPExpireAt(const QString &key, const qint64 utcMsec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("PEXPIREAT %1 %2").arg(key).arg(utcMsec)));
}

//!
//! \brief Удалить заданное время "жизни" объекта
//! \param key Ключ
//! \return
//!
//! Remove the existing timeout on key, turning the key from volatile (a key with an expire set) to persistent (a key that will never expire as no timeout is associated).
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXPIRE mykey 10
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 10
//! redis>  PERSIST mykey
//! (integer) 1
//! redis>  TTL mykey
//! (integer) -1
//! redis>
//!
bool QtRedisClient::redisPersist(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("PERSIST %1").arg(key)));
}

//!
//! \brief Получить время "жизни" объекта в сек.
//! \param key Ключ
//! \return
//!
//! Returns the remaining time to live of a key that has a timeout. This introspection capability allows a Redis client to check how many seconds a given key will continue
//! to be part of the dataset.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//! - The command returns -2 if the key does not exist.
//! - The command returns -1 if the key exists but has no associated expire.
//!
qlonglong QtRedisClient::redisTtl(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("TTL %1").arg(key)));
}

//!
//! \brief Получить время "жизни" объекта в мсек.
//! \param key Ключ
//! \return
//!
//! Like TTL this command returns the remaining time to live of a key that has an expire set, with the sole difference that TTL returns the amount of remaining time
//! in seconds while PTTL returns it in milliseconds.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//! - The command returns -2 if the key does not exist.
//! - The command returns -1 if the key exists but has no associated expire.
//!
qlonglong QtRedisClient::redisPTtl(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("PTTL %1").arg(key)));
}

//!
//! \brief Уменьшить число, сохраненное в ключе на один
//! \param key Ключ
//! \return
//!
//! Decrements the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the decrement
//!
qlonglong QtRedisClient::redisDecr(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("DECR %1").arg(key)));
}

//!
//! \brief Уменьшить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param decr Значение
//! \return
//!
//! Decrements the number stored at key by decrement. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the decrement
//!
qlonglong QtRedisClient::redisDecrBy(const QString &key, const qint64 decr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("DECRBY %1 %2").arg(key).arg(decr)));
}

//!
//! \brief Увеличить число, сохраненное в ключе на один
//! \param key Ключ
//! \return
//!
//! Increments the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Note: this is a string operation because Redis does not have a dedicated integer type.
//!       The string stored at the key is interpreted as a base-10 64 bit signed integer to execute the operation.
//!
//! Redis stores integers in their integer representation, so for string values that actually hold an integer,
//! there is no overhead for storing the string representation of the integer.
//!
//! Return value: the value of key after the increment
//!
qlonglong QtRedisClient::redisIncr(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("INCR %1").arg(key)));
}

//!
//! \brief Увеличить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param incr Значение
//! \return
//!
//! Increments the number stored at key by increment. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the increment
//!
qlonglong QtRedisClient::redisIncrBy(const QString &key, const qint64 incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("INCRBY %1 %2").arg(key).arg(incr)));
}

//!
//! \brief Увеличить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param incr Значение
//! \return
//!
//! Increment the string representing a floating point number stored at key by the specified increment.
//! By using a negative increment value, the result is that the value stored at the key is decremented (by the obvious properties of addition).
//! If the key does not exist, it is set to 0 before performing the operation. An error is returned if one of the following conditions occur:
//!
//! The key contains a value of the wrong type (not a string).
//! The current key content or the specified increment are not parsable as a double precision floating point number.
//!
//! If the command is successful the new incremented value is stored as the new value of the key (replacing the old one), and returned to the caller as a string.
//!
//! Both the value already contained in the string key and the increment argument can be optionally provided in exponential notation,
//! however the value computed after the increment is stored consistently in the same format, that is, an integer number followed (if needed) by a dot,
//! and a variable number of digits representing the decimal part of the number. Trailing zeroes are always removed.
//!
//! The precision of the output is fixed at 17 digits after the decimal point regardless of the actual internal precision of the computation.
//!
//! Return value: the value of key after the increment
//!
qlonglong QtRedisClient::redisIncrByFloat(const QString &key, const float incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("INCRBYFLOAT %1 %2").arg(key).arg(incr)));
}

//!
//! \brief Переименовать ключ
//! \param key Ключ
//! \param newKey Новое имя ключа
//! \return
//!
//! Renames key to newkey. It returns an error when key does not exist. If newkey already exists it is overwritten, when this happens RENAME executes an implicit DEL operation,
//! so if the deleted key contains a very big value it may cause high latency even if RENAME itself is usually a constant-time operation.
//!
//! Note: Before Redis 3.2.0, an error is returned if source and destination names are the same.
//!
bool QtRedisClient::redisRename(const QString &key, const QString &newKey)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (newKey.isEmpty() || newKey.indexOf(" ") != -1) {
        this->setLastError("Invalid newKey!");
        return false;
    }
    return this->replySimpleStringToBool(this->redisExecCommand(QString("RENAME %1 %2").arg(key).arg(newKey)));
}

//!
//! \brief Переименовать ключ, если новое имя не существует
//! \param key Ключ
//! \param newKey Новое имя ключа
//! \return
//!
//! Renames key to newkey if newkey does not yet exist. It returns an error when key does not exist.
//!
//! Note: Before Redis 3.2.0, an error is returned if source and destination names are the same.
//!
bool QtRedisClient::redisRenameNx(const QString &key, const QString &newKey)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (newKey.isEmpty() || newKey.indexOf(" ") != -1) {
        this->setLastError("Invalid newKey!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("RENAMENX %1 %2").arg(key).arg(newKey)));
}

//!
//! \brief Получить тип значения ключа
//! \param key Ключ
//! \return
//!
//! Returns the string representation of the type of the value stored at key.
//! The different types that can be returned are: string, list, set, zset and hash.
//!
QString QtRedisClient::redisType(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("TYPE %1").arg(key)));
}

//!
//! \brief Установить ключи в соответствии со значениями
//! \param keyValue Список ключ-значение
//! \return
//!
//! Sets the given keys to their respective values. MSET replaces existing values with new values, just as regular SET.
//! See MSETNX if you don't want to overwrite existing values.
//! MSET is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
bool QtRedisClient::redisMSet(const QMap<QString, QString> &keyValue)
{
    if (keyValue.isEmpty()) {
        this->setLastError("Invalid key-value (Empty)!");
        return false;
    }
    QList<QString> keyList = keyValue.keys();
    for (const QString &key : keyList) {
        if (key.isEmpty() || key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return false;
        }
    }
    QStringList argv;
    argv << "MSET";
    QMapIterator<QString, QString> i (keyValue);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->replySimpleStringToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Установить ключи в соответствии со значениями, если ключи не существуют
//! \param keyValue Список ключ-значение
//! \return
//!
//! Sets the given keys to their respective values. MSETNX will not perform any operation at all even if just a single key already exists.
//! Because of this semantic MSETNX can be used in order to set different keys representing different fields of an unique logic object
//! in a way that ensures that either all the fields or none at all are set.
//!
//! MSETNX is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
bool QtRedisClient::redisMSetNx(const QMap<QString, QString> &keyValue)
{
    if (keyValue.isEmpty()) {
        this->setLastError("Invalid key-value (Empty)!");
        return false;
    }
    QList<QString> keyList = keyValue.keys();
    for (const QString &key : keyList) {
        if (key.isEmpty() || key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return false;
        }
    }
    QStringList argv;
    argv << "MSETNX";
    QMapIterator<QString, QString> i (keyValue);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->replyIntToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить значения ключей
//! \param keyList Список ключей
//! \return
//!
//! Returns the values of all specified keys. For every key that does not hold a string value or does not exist, the special value nil is returned.
//! Because of this, the operation never fails.
//! Examples
//! redis>  SET key1 "Hello"
//! "OK"
//! redis>  SET key2 "World"
//! "OK"
//! redis>  MGET key1 key2 nonexisting
//! 1) "Hello"
//! 2) "World"
//! 3) (nil)
//! redis>
//!
QtRedisReply QtRedisClient::redisMGet(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return QtRedisReply();
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return QtRedisReply();
        }
    }
    return this->redisExecCommand(QString("MGET %1").arg(keyList.join(" ")));
}

//!
//! \brief Переместить ключ с его значениями в другую БД
//! \param key Ключ
//! \param dbIndex Индекс БД
//! \return
//!
//! Move key from the currently selected database (see SELECT) to the specified destination database.
//! When key already exists in the destination database, or it does not exist in the source database, it does nothing.
//! It is possible to use MOVE as a locking primitive because of this.
//!
bool QtRedisClient::redisMove(const QString &key, const int dbIndex)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    if (dbIndex < 0) {
        this->setLastError("Invalid db index!");
        return false;
    }
    return this->replyIntToBool(this->redisExecCommand(QString("MOVE %1 %2").arg(key).arg(dbIndex)));
}

//!
//! \brief Преобразовать значение ключа в формат Redis-а
//! \param key Ключ
//! \return
//!
//! Для получения корректного значения пользуйтесь QtRedisReply::rawValue().
//!
//! Serialize the value stored at key in a Redis-specific format and return it to the user. The returned value can be synthesized back into a Redis key using the RESTORE command.
//!
//! The serialization format is opaque and non-standard, however it has a few semantic characteristics:
//!
//! - It contains a 64-bit checksum that is used to make sure errors will be detected. The RESTORE command makes sure to check the checksum before synthesizing a key using the serialized value.
//! - Values are encoded in the same format used by RDB.
//! - An RDB version is encoded inside the serialized value, so that different Redis versions with incompatible RDB formats will refuse to process the serialized value.
//!
//! The serialized value does NOT contain expire information. In order to capture the time to live of the current value the PTTL command should be used.
//!
//! If key does not exist a nil bulk reply is returned.
//! Return value: the serialized value.
//!
//! Examples
//! redis>  SET mykey 10
//! "OK"
//! redis>  DUMP mykey
//! "\u0000\xC0\n\b\u0000ײ\xBB\xFA\xA7\xB7\xE9\x83"
//! redis>
//!
QtRedisReply QtRedisClient::redisDump(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    return this->redisExecCommand(QString("DUMP %1").arg(key));
}


// ------------------------------------------------------------------------
// -- LIST COMMANDS -------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Получить элменет списка по индексу
//! \param key Ключ
//! \param index Индекс
//! \return
//!
//! Returns the element at index index in the list stored at key. The index is zero-based, so 0 means the first element,
//! 1 the second element and so on. Negative indices can be used to designate elements starting at the tail of the list.
//! Here, -1 means the last element, -2 means the penultimate and so forth.
//!
//! When the value at key is not a list, an error is returned.
//!
QString QtRedisClient::redisLIndex(const QString &key, const int index)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("LINDEX %1 %2").arg(key).arg(index)));
}

//!
//! \brief Вставить значение (value) в список до или после значения (pilot)
//! \param key Ключ
//! \param pilot Значение поиска
//! \param value Новое значение
//! \param insertFlag Тип вставки (BEFORE|AFTER)
//! \return
//!
//! Inserts value in the list stored at key either before or after the reference value pivot.
//! When key does not exist, it is considered an empty list and no operation is performed.
//! An error is returned when key exists but does not hold a list value.
//! Return value: the length of the list after the insert operation, or -1 when the value pivot was not found.
//!
qlonglong QtRedisClient::redisLInsert(const QString &key,
                                      const QString &pilot,
                                      const QString &value,
                                      const QString &insertFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (insertFlag.toUpper() != QString("BEFORE")
        && insertFlag.toUpper() != QString("AFTER")) {
        this->setLastError("Invalid insertFlag!");
        return -1;
    }
    QStringList argv;
    argv << "LINSERT" << key << insertFlag.toUpper() << pilot << value;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить размер списка
//! \param key Ключ
//! \return
//!
//! Returns the length of the list stored at key. If key does not exist, it is interpreted as an empty list and 0 is returned.
//! An error is returned when the value stored at key is not a list.
//! Return value: the length of the list at key.
//!
qlonglong QtRedisClient::redisLLen(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("LLEN %1").arg(key)));
}

//!
//! \brief Удалить и вернуть первый элменет списка
//! \param key Ключ
//! \return
//!
//! Removes and returns the first element of the list stored at key.
//! Return value: the value of the first element, or nil when key does not exist.
//!
QString QtRedisClient::redisLPop(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("LPOP %1").arg(key)));
}

//!
//! \brief Вставить значения в начало списка
//! \param key Ключ
//! \param valueList Значения
//! \return
//!
//! Insert all the specified values at the head of the list stored at key. If key does not exist, it is created as empty list before performing the push operations.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the head of the list, from the leftmost element to the rightmost element.
//! So for instance the command LPUSH mylist a b c will result into a list containing c as first element, b as second element and a as third element.
//! Return value: the length of the list after the push operations.
//!
//! Redis >= 2.4: Accepts multiple value arguments. In Redis versions older than 2.4 it was possible to push a single value per command.
//!
qlonglong QtRedisClient::redisLPush(const QString &key, const QStringList &valueList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (valueList.isEmpty()) {
        this->setLastError("Invalid valueList (Empty)!");
        return -1;
    }
    QStringList argv;
    argv << "LPUSH" << key;
    for (const QString &str : valueList)
        argv << str;

    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Вставить значение в начало списка, если ключ существует и он содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Inserts value at the head of the list stored at key, only if key already exists and holds a list. In contrary to LPUSH, no operation will be performed when key does not yet exist.
//! Return value: the length of the list after the push operation.
//!
qlonglong QtRedisClient::redisLPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "LPUSHX" << key << value;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить список значений ключа
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \return
//!
//! Returns the specified elements of the list stored at key. The offsets start and stop are zero-based indexes, with 0 being the first element of the list (the head of the list),
//! 1 being the next element and so on.
//!
//! These offsets can also be negative numbers indicating offsets starting at the end of the list. For example, -1 is the last element of the list, -2 the penultimate, and so on.
//!
//! Consistency with range functions in various programming languages:
//! Note that if you have a list of numbers from 0 to 100, LRANGE list 0 10 will return 11 elements, that is, the rightmost item is included.
//! This may or may not be consistent with behavior of range-related functions in your programming language of choice (think Ruby's Range.new, Array#slice or Python's range() function).
//!
//! Out-of-range indexes:
//! Out of range indexes will not produce an error. If start is larger than the end of the list, an empty list is returned.
//! If stop is larger than the actual end of the list, Redis will treat it like the last element of the list.
//!
//! Return value: list of elements in the specified range.
//!
QtRedisReply QtRedisClient::redisLRange(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    return this->redisExecCommand(QString("LRANGE %1 %2 %3").arg(key).arg(start).arg(stop));
}

//!
//! \brief Удалить первое найденное значение из списка начиная с заданного индекса
//! \param key Ключ
//! \param value Значение
//! \param count Индекс
//! \return
//!
//! Removes the first count occurrences of elements equal to value from the list stored at key. The count argument influences the operation in the following ways:
//! - count > 0: Remove elements equal to value moving from head to tail.
//! - count < 0: Remove elements equal to value moving from tail to head.
//! - count = 0: Remove all elements equal to value.
//!
//! For example, LREM list -2 "hello" will remove the last two occurrences of "hello" in the list stored at list.
//!
//! Note that non-existing keys are treated like empty lists, so when key does not exist, the command will always return 0.
//! Return value: the number of removed elements.
//!
qlonglong QtRedisClient::redisLRem(const QString &key, const QString &value, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "LREM" << key << QString::number(count) << value;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Установить новое знаяение в списке по индексу
//! \param key Ключ
//! \param value Новое значение
//! \param index Индекс
//! \return
//!
bool QtRedisClient::redisLSet(const QString &key, const QString &value, const int index)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    QStringList argv;
    argv << "LSET" << key << QString::number(index) << value;
    return this->replySimpleStringToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Обрезать список, чтобы он содержал тоько указанный диапазон элементов
//! \param key Ключ
//! \param start Начало диапазона
//! \param stop Конец диапазона
//! \return
//!
//! Trim an existing list so that it will contain only the specified range of elements specified. Both start and stop are zero-based indexes,
//! where 0 is the first element of the list (the head), 1 the next element and so on.
//!
//! For example: LTRIM foobar 0 2 will modify the list stored at foobar so that only the first three elements of the list will remain.
//!
//! start and end can also be negative numbers indicating offsets from the end of the list, where -1 is the last element of the list, -2 the penultimate element and so on.
//!
//! Out of range indexes will not produce an error: if start is larger than the end of the list, or start > end, the result will be an empty list (which causes key to be removed).
//! If end is larger than the end of the list, Redis will treat it like the last element of the list.
//!
//! A common use of LTRIM is together with LPUSH / RPUSH. For example:
//! LPUSH mylist someelement
//! LTRIM mylist 0 99
//!
//! This pair of commands will push a new element on the list, while making sure that the list will not grow larger than 100 elements.
//! This is very useful when using Redis to store logs for example. It is important to note that when used in this way LTRIM is an O(1) operation because in the average case
//! just one element is removed from the tail of the list.
//!
//! Examples
//! redis>  RPUSH mylist "one"
//! (integer) 1
//! redis>  RPUSH mylist "two"
//! (integer) 2
//! redis>  RPUSH mylist "three"
//! (integer) 3
//! redis>  LTRIM mylist 1 -1
//! "OK"
//! redis>  LRANGE mylist 0 -1
//! 1) "two"
//! 2) "three"
//! redis>
//!
bool QtRedisClient::redisLTrim(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    return this->replySimpleStringToBool(this->redisExecCommand(QString("LTRIM %1 %2 %3").arg(key).arg(start).arg(stop)));
}

//!
//! \brief Удалить и получить последний элемент списка, хранящегося в ключе
//! \param key Ключ
//! \return
//!
//! Removes and returns the last element of the list stored at key.
//!
QString QtRedisClient::redisRPop(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("RPOP %1").arg(key)));
}

//!
//! \brief Удалить и получить последний элемент списка (sourceKey), и поместить его в начало списка (destKey)
//! \param sourceKey Ключ-ичтоник
//! \param destKey Ключ-получатель
//! \return
//!
//! Atomically returns and removes the last element (tail) of the list stored at source, and pushes the element at the first element (head) of the list stored at destination.
//!
//! For example: consider source holding the list a,b,c, and destination holding the list x,y,z. Executing RPOPLPUSH results in source holding a,b and destination holding c,x,y,z.
//!
//! If source does not exist, the value nil is returned and no operation is performed. If source and destination are the same,
//! the operation is equivalent to removing the last element from the list and pushing it as first element of the list, so it can be considered as a list rotation command.
//!
//! Return value: the element being popped and pushed.
//!
QString QtRedisClient::redisRPopLPush(const QString &sourceKey, const QString &destKey)
{
    if (sourceKey.isEmpty() || sourceKey.indexOf(" ") != -1) {
        this->setLastError("Invalid sourceKey!");
        return QString();
    }
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1) {
        this->setLastError("Invalid destKey!");
        return QString();
    }
    return this->replyToString(this->redisExecCommand(QString("RPOPLPUSH %1 %2").arg(sourceKey).arg(destKey)));
}

//!
//! \brief Вставить значения в конец списка
//! \param key Ключ
//! \param valueList Значения
//! \return
//!
//! Insert all the specified values at the tail of the list stored at key. If key does not exist, it is created as empty list before performing the push operation.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the tail of the list, from the leftmost element to the rightmost element.
//! So for instance the command RPUSH mylist a b c will result into a list containing a as first element, b as second element and c as third element.
//!
//! Return value: the length of the list after the push operation.
//! Redis >= 2.4: Accepts multiple value arguments. In Redis versions older than 2.4 it was possible to push a single value per command.
//!
//!
qlonglong QtRedisClient::redisRPush(const QString &key, const QStringList &valueList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (valueList.isEmpty()) {
        this->setLastError("Invalid valueList (Empty)!");
        return -1;
    }
    QStringList argv;
    argv << "RPUSH" << key;
    for (const QString &str : valueList)
        argv << str;

    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Вставить значение в конец списка, если ключ существует и содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Inserts value at the tail of the list stored at key, only if key already exists and holds a list.
//! In contrary to RPUSH, no operation will be performed when key does not yet exist.
//! Return value: the length of the list after the push operation.
//!
qlonglong QtRedisClient::redisRPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "RPUSHX" << key << value;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}


// ------------------------------------------------------------------------
// -- STORED COMMANDS -----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Добавить значения в набор
//! \param key Ключ
//! \param memberList Значения
//! \return
//!
//! Если одно или несколько значений уже есть в наборе, то они не добавляются.
//!
//! Add the specified members to the set stored at key. Specified members that are already a member of this set are ignored.
//! If key does not exist, a new set is created before adding the specified members.
//!
//! An error is returned when the value stored at key is not a set.
//! Return value: the number of elements that were added to the set, not including all the elements already present into the set.
//! Redis >= 2.4: Accepts multiple member arguments. Redis versions before 2.4 are only able to add a single member per call.
//!
qlonglong QtRedisClient::redisSAdd(const QString &key, const QStringList &memberList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (memberList.isEmpty()) {
        this->setLastError("Invalid memberList (Empty)!");
        return -1;
    }
    QStringList argv;
    argv << "SADD" << key;
    for (const QString &str : memberList)
        argv << str;

    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить количество элементов в наборе
//! \param key Ключ
//! \return
//!
//! Returns the set cardinality (number of elements) of the set stored at key.
//! Return value: the cardinality (number of elements) of the set, or 0 if key does not exist.
//!
qlonglong QtRedisClient::redisSCard(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("SCARD %1").arg(key)));
}

//!
//! \brief Получить список элементов, полученный путем разницы между первым набором(keyList[0]) и остальными(keyList[1..N])
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the difference between the first set and all the successive sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SDIFF key1 key2 key3 = {b,d}
//!
//! Keys that do not exist are considered to be empty sets.
//!
QtRedisReply QtRedisClient::redisSDiff(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return QtRedisReply();
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return QtRedisReply();
        }
    }
    return this->redisExecCommand(QString("SDIFF %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученный путем разницы между первым набором(keyList[0]) и остальными(keyList[1..N]), и записать результат в новый набор
//! \param dest Название нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SDIFF, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
qlonglong QtRedisClient::redisSDiffStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1) {
        this->setLastError("Invalid dest!");
        return -1;
    }
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    return this->replyToLong(this->redisExecCommand(QString("SDIFFSTORE %1 %2").arg(dest).arg(keyList.join(" "))));
}

//!
//! \brief Получить список элементов, полученных путем пересечения наборов
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the intersection of all the given sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SINTER key1 key2 key3 = {c}
//!
//! Keys that do not exist are considered to be empty sets. With one of the keys being an empty set, the resulting set is also empty
//! (since set intersection with an empty set always results in an empty set).
//!
QtRedisReply QtRedisClient::redisSInter(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return QtRedisReply();
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return QtRedisReply();
        }
    }
    return this->redisExecCommand(QString("SINTER %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученных путем пересечения наборов, и записать результат в новый набор
//! \param dest Название нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SINTER, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
qlonglong QtRedisClient::redisSInterStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1) {
        this->setLastError("Invalid dest!");
        return -1;
    }
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    return this->replyToLong(this->redisExecCommand(QString("SINTERSTORE %1 %2").arg(dest).arg(keyList.join(" "))));
}

//!
//! \brief Входит ли элемент в набор
//! \param key Ключ набора
//! \param member Значение
//! \return
//!
bool QtRedisClient::redisSIsMember(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return false;
    }
    QStringList argv;
    argv << "SISMEMBER" << key << member;
    return this->replyIntToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить список всех элементов набора
//! \param key Ключ набора
//! \return
//!
QStringList QtRedisClient::redisSMembers(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QStringList();
    }
    return this->replyToArray(this->redisExecCommand(QString("SMEMBERS %1").arg(key)));
}

//!
//! \brief Переместить элемент из одного набора(sourceKey) в другой(destKey)
//! \param sourceKey Ключ набора источника
//! \param destKey Ключ набора получателя
//! \param member Значение
//! \return
//!
//! Move member from the set at source to the set at destination. This operation is atomic.
//! In every given moment the element will appear to be a member of source or destination for other clients.
//!
//! If the source set does not exist or does not contain the specified element, no operation is performed and 0 is returned.
//! Otherwise, the element is removed from the source set and added to the destination set.
//! When the specified element already exists in the destination set, it is only removed from the source set.
//!
//! An error is returned if source or destination does not hold a set value.
//!
bool QtRedisClient::redisSMove(const QString &sourceKey, const QString &destKey, const QString &member)
{
    if (sourceKey.isEmpty() || sourceKey.indexOf(" ") != -1) {
        this->setLastError("Invalid sourceKey!");
        return false;
    }
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1) {
        this->setLastError("Invalid destKey!");
        return false;
    }
    QStringList argv;
    argv << "SMOVE" << sourceKey << destKey << member;
    return this->replyIntToBool(this->redisExecCommandArgv(argv));
}

//!
//! \brief Удалить и вернуть один или несколько случайных элментов в наборе
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! Removes and returns one or more random elements from the set value store at key.
//!
//! This operation is similar to SRANDMEMBER, that returns one or more random elements from a set but does not remove it.
//!
//! The count argument is available since version 3.2.
//! Return value: the removed element, or nil when key does not exist.
//! Examples
//! redis>  SADD myset "one"
//! (integer) 1
//! redis>  SADD myset "two"
//! (integer) 1
//! redis>  SADD myset "three"
//! (integer) 1
//! redis>  SPOP myset
//! "three"
//! redis>  SMEMBERS myset
//! 1) "two"
//! 2) "one"
//! redis>  SADD myset "four"
//! (integer) 1
//! redis>  SADD myset "five"
//! (integer) 1
//! redis>  SPOP myset 3
//! 1) "four"
//! 2) "two"
//! 3) "one"
//! redis>  SMEMBERS myset
//! 1) "five"
//! redis>
//!
QtRedisReply QtRedisClient::redisSPop(const QString &key, const uint count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QStringList argv;
    argv << "SPOP" << key;
    if (count > 1)
        argv << QString::number(count);

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить один или несколько случайных элементов из набора
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! When called with just the key argument, return a random element from the set value stored at key.
//!
//! Starting from Redis version 2.6, when called with the additional count argument, return an array of count distinct elements if count is positive.
//! If called with a negative count the behavior changes and the command is allowed to return the same element multiple times.
//! In this case the number of returned elements is the absolute value of the specified count.
//!
//! When called with just the key argument, the operation is similar to SPOP, however while SPOP also removes the randomly selected element from the set,
//! SRANDMEMBER will just return a random element without altering the original set in any way.
//! Return value:
//! - String reply: without the additional count argument the command returns a Bulk Reply with the randomly selected element, or nil when key does not exist.
//! - Array reply: when the additional count argument is passed the command returns an array of elements, or an empty array when key does not exist.
//!
//! Examples
//! redis>  SADD myset one two three
//! (integer) 3
//! redis>  SRANDMEMBER myset
//! "two"
//! redis>  SRANDMEMBER myset 2
//! 1) "two"
//! 2) "one"
//! redis>  SRANDMEMBER myset -5
//! 1) "two"
//! 2) "three"
//! 3) "two"
//! 4) "three"
//! 5) "three"
//! redis>
//!
QtRedisReply QtRedisClient::redisSRandMember(const QString &key, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (count == 0 || count == 1)
        return this->redisExecCommand(QString("SRANDMEMBER %1").arg(key));

    return this->redisExecCommand(QString("SRANDMEMBER %1 %2").arg(key).arg(count));
}

//!
//! \brief Удалить элемент(ы) из набора
//! \param key Ключ набора
//! \param memberList Список элементов
//! \return
//!
//! Remove the specified members from the set stored at key. Specified members that are not a member of this set are ignored.
//! If key does not exist, it is treated as an empty set and this command returns 0.
//!
//! An error is returned when the value stored at key is not a set.
//! Return value: the number of members that were removed from the set, not including non existing members.
//! Redis >= 2.4: Accepts multiple member arguments. Redis versions older than 2.4 can only remove a set member per call.
//!
qlonglong QtRedisClient::redisSRem(const QString &key, const QStringList &memberList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (memberList.isEmpty()) {
        this->setLastError("Invalid memberList (Empty)!");
        return -1;
    }
    QStringList argv;
    argv << "SREM" << key;
    for (const QString &str : memberList)
        argv << str;

    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить список элементов, полученных путем объединения всех заданных наборов
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the union of all the given sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SUNION key1 key2 key3 = {a,b,c,d,e}
//!
//! Keys that do not exist are considered to be empty sets.
//!
QtRedisReply QtRedisClient::redisSUnion(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return QtRedisReply();
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return QtRedisReply();
        }
    }
    return this->redisExecCommand(QString("SUNION %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученных путем объединения всех заданных наборов, и записать в новый набор (dest)
//! \param dest Название ключа нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SUNION, but instead of returning the resulting set, it is stored in destination.
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
qlonglong QtRedisClient::redisSUnionStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1) {
        this->setLastError("Invalid dest!");
        return -1;
    }
    if (keyList.isEmpty() || keyList.size() < 2) {
        this->setLastError("Invalid keyList (Empty or size < 2)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    return this->replyToLong(this->redisExecCommand(QString("SUNIONSTORE %1 %2").arg(dest).arg(keyList.join(" "))));
}


// ------------------------------------------------------------------------
// -- SORTED STORED COMMANDS ----------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Добавить элементы со значениями в отсортированный набор
//! \param key Ключ
//! \param scoreMember Список элементов со значениями
//! \param updFlag Флаг обновления (NX|XX)
//! \param chFlag Флаг модификации
//! \param incrFlag Флаг инкремента
//! \return
//!
//! Adds all the specified members with the specified scores to the sorted set stored at key. It is possible to specify multiple score / member pairs.
//! If a specified member is already a member of the sorted set, the score is updated and the element reinserted at the right position to ensure the correct ordering.
//!
//! If key does not exist, a new sorted set with the specified members as sole members is created, like if the sorted set was empty. If the key exists but does not hold a sorted set,
//! an error is returned.
//!
//! The score values should be the string representation of a double precision floating point number. +inf and -inf values are valid values as well.
//!
//! ZADD options (Redis 3.0.2 or greater):
//! ZADD supports a list of options, specified after the name of the key and before the first score argument. Options are:
//! - XX: Only update elements that already exist. Never add elements.
//! - NX: Don't update already existing elements. Always add new elements.
//! - CH: Modify the return value from the number of new elements added, to the total number of elements changed (CH is an abbreviation of changed). Changed elements are new elements added and elements already existing for which the score was updated. So elements specified in the command line having the same score as they had in the past are not counted. Note: normally the return value of ZADD only counts the number of new elements added.
//! - INCR: When this option is specified ZADD acts like ZINCRBY. Only one score-element pair can be specified in this mode.
//!
//! Range of integer scores that can be expressed precisely:
//! Redis sorted sets use a double 64-bit floating point number to represent the score. In all the architectures we support, this is represented as an IEEE 754 floating point number,
//! that is able to represent precisely integer numbers between -(2^53) and +(2^53) included. In more practical terms,
//! all the integers between -9007199254740992 and 9007199254740992 are perfectly representable. Larger integers, or fractions, are internally represented in exponential form,
//! so it is possible that you get only an approximation of the decimal number, or of the very big integer, that you set as score.
//!
//! Sorted sets 101:
//! Sorted sets are sorted by their score in an ascending way. The same element only exists a single time, no repeated elements are permitted.
//! The score can be modified both by ZADD that will update the element score, and as a side effect, its position on the sorted set,
//! and by ZINCRBY that can be used in order to update the score relatively to its previous value.
//!
//! The current score of an element can be retrieved using the ZSCORE command, that can also be used to verify if an element already exists or not.
//! For an introduction to sorted sets, see the data types page on sorted sets.
//!
//! Elements with the same score:
//! While the same element can't be repeated in a sorted set since every element is unique, it is possible to add multiple different elements having the same score.
//! When multiple elements have the same score, they are ordered lexicographically (they are still ordered by score as a first key, however, locally,
//! all the elements with the same score are relatively ordered lexicographically).
//!
//! The lexicographic ordering used is binary, it compares strings as array of bytes.
//! If the user inserts all the elements in a sorted set with the same score (for example 0), all the elements of the sorted set are sorted lexicographically,
//! and range queries on elements are possible using the command ZRANGEBYLEX (Note: it is also possible to query sorted sets by range of scores using ZRANGEBYSCORE).
//!
//! Return value:
//! - The number of elements added to the sorted sets, not including elements already existing for which the score was updated.
//! - If the INCR option is specified, the return value will be Bulk string reply: the new score of member (a double precision floating point number), represented as string.
//!
//! Redis >= 2.4: Accepts multiple elements. In Redis versions older than 2.4 it was possible to add or update a single member per call.
//!
//!
QtRedisReply QtRedisClient::redisZAdd(const QString &key,
                                      const QMultiMap<QString, QString> scoreMember,
                                      const QString &updFlag,
                                      const bool chFlag,
                                      const bool incrFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (scoreMember.isEmpty()) {
        this->setLastError("Invalid scoreMember (Empty)!");
        return QtRedisReply();
    }
    if (!updFlag.isEmpty()
        && updFlag.toUpper() != QString("NX")
        && updFlag.toUpper() != QString("XX")) {
        this->setLastError("Invalid updFlag!");
        return QtRedisReply();
    }
    QStringList argv;
    argv << "ZADD" << key;
    if (!updFlag.isEmpty())
        argv << updFlag.toUpper();
    if (chFlag)
        argv << "CH";
    if (incrFlag)
        argv << "INCR";

    QMapIterator<QString, QString> i (scoreMember);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить размер отсортированного набора
//! \param key Ключ
//! \return
//!
qlonglong QtRedisClient::redisZCard(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("ZCARD %1").arg(key)));
}

//!
//! \brief Получить кличество элементов отсортированного набора, находящихся между min и max
//! \param key Ключ
//! \param min Мин.
//! \param max Макс.
//! \return
//!
//! Returns the number of elements in the sorted set at key with a score between min and max.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//!
//! For example:
//!
//! ZRANGEBYSCORE zset (1 5
//! Will return all elements with 1 < score <= 5 while:
//!
//! ZRANGEBYSCORE zset (5 (10
//! Will return all the elements with 5 < score < 10 (5 and 10 excluded).
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range. Because of this there is no need to do a work proportional to the size of the range.
//! Return value: the number of elements in the specified score range.
//!
qlonglong QtRedisClient::redisZCount(const QString &key, const QVariant &min, const QVariant &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (min.type() != QVariant::Invalid
        && min.type() != QVariant::Int
        && min.type() != QVariant::UInt
        && min.type() != QVariant::LongLong
        && min.type() != QVariant::ULongLong
        && min.type() != QVariant::Double) {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.type() != QVariant::Invalid
        && max.type() != QVariant::Int
        && max.type() != QVariant::UInt
        && max.type() != QVariant::LongLong
        && max.type() != QVariant::ULongLong
        && max.type() != QVariant::Double) {
        this->setLastError("Invalid max!");
        return -1;
    }
    QString buffMin("-inf");
    if (min.type() != QVariant::Invalid)
        buffMin = min.toString();

    QString buffMax("+inf");
    if (max.type() != QVariant::Invalid)
        buffMax = max.toString();

    return this->replyToLong(this->redisExecCommand(QString("ZCOUNT %1 %2 %3").arg(key).arg(buffMin).arg(buffMax)));
}

//!
//! \brief Увеличить значение отсортированного набора на величину incr
//! \param key Ключ
//! \param member Значение набора
//! \param incr Инкремент
//! \return
//!
//! Increments the score of member in the sorted set stored at key by increment. If member does not exist in the sorted set, it is added with increment as its score (as if its previous score was 0.0). If key does not exist, a new sorted set with the specified member as its sole member is created.
//!
//! An error is returned when key exists but does not hold a sorted set.
//!
//! The score value should be the string representation of a numeric value, and accepts double precision floating point numbers. It is possible to provide a negative value to decrement the score.
//! Return value: the new score of member (a double precision floating point number), represented as string.
//!
QtRedisReply QtRedisClient::redisZIncrBy(const QString &key, const QString &member, const qint64 incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QStringList argv;
    argv << "ZINCRBY" << key << QString::number(incr) << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вычислить пересечения отсортированных наборов и результат записать в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей
//! \param weightList Список значений
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Computes the intersection of numkeys sorted sets given by the specified keys, and stores the result in destination.
//! It is mandatory to provide the number of input keys (numkeys) before passing the input keys and the other (optional) arguments.
//!
//! By default, the resulting score of an element is the sum of its scores in the sorted sets where it exists.
//! Because intersection requires an element to be a member of every given sorted set, this results in the score of every element in the resulting sorted set to be equal
//! to the number of input sorted sets.
//!
//! For a description of the WEIGHTS and AGGREGATE options, see ZUNIONSTORE.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting sorted set at destination.
//! Examples
//! redis>  ZADD zset1 1 "one"
//! (integer) 1
//! redis>  ZADD zset1 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 1 "one"
//! (integer) 1
//! redis>  ZADD zset2 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 3 "three"
//! (integer) 1
//! redis>  ZINTERSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 2
//! redis>  ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "two"
//! 4) "10"
//! redis>
//!
qlonglong QtRedisClient::redisZInterStore(const QString &destKey,
                                          const QStringList &keyList,
                                          const QList<int> &weightList,
                                          const QString &aggregateFlag)
{
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1) {
        this->setLastError("Invalid destKey!");
        return -1;
    }
    if (keyList.isEmpty()) {
        this->setLastError("Invalid keyList (Empty)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    QString weightListStr;
    for (const int weight : weightList) {
        if (weight <= 0)
            return -1;
        weightListStr += QString("%1 ").arg(weight);
    }
    if (!aggregateFlag.isEmpty()
        && aggregateFlag.toUpper() != QString("SUM")
        && aggregateFlag.toUpper() != QString("MIN")
        && aggregateFlag.toUpper() != QString("MAX")) {
        this->setLastError("Invalid aggregateFlag!");
        return -1;
    }

    QString command = QString("ZINTERSTORE %1 %2 %3")
                      .arg(destKey)
                      .arg(keyList.size())
                      .arg(keyList.join(" "));

    if (!weightList.isEmpty())
        command += QString(" WEIGHTS %1").arg(weightListStr);
    if (!aggregateFlag.isEmpty())
        command += QString(" AGGREGATE %1").arg(aggregateFlag.toUpper());

    return this->replyToLong(this->redisExecCommand(command.trimmed()));
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает количество элементов в отсортированном наборе между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering, this command returns the number of elements
//! in the sorted set at key with a value between min and max.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range.
//! Because of this there is no need to do a work proportional to the size of the range.
//! Return value: the number of elements in the specified score range.
//!
qlonglong QtRedisClient::redisZLexCount(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return -1;
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("ZLEXCOUNT %1 %2 %3").arg(key).arg(min).arg(max)));
}

//!
//! \brief Получить диапазон элементов отсортированного набора
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \param withScores Если true - вернуть количество элементов вместе с элементами
//! \return
//!
//! Returns the specified range of elements in the sorted set stored at key. The elements are considered to be ordered from the lowest to the highest score.
//! Lexicographical order is used for elements with equal score.
//!
//! See ZREVRANGE when you need the elements ordered from highest to lowest score (and descending lexicographical order for elements with equal score).
//!
//! Both start and stop are zero-based indexes, where 0 is the first element, 1 is the next element and so on.
//! They can also be negative numbers indicating offsets from the end of the sorted set, with -1 being the last element of the sorted set, -2 the penultimate element and so on.
//!
//! start and stop are inclusive ranges, so for example ZRANGE myzset 0 1 will return both the first and the second element of the sorted set.
//!
//! Out of range indexes will not produce an error. If start is larger than the largest index in the sorted set, or start > stop, an empty list is returned.
//! If stop is larger than the end of the sorted set Redis will treat it like it is the last element of the sorted set.
//!
//! It is possible to pass the WITHSCORES option in order to return the scores of the elements together with the elements.
//! The returned list will contain value1,score1,...,valueN,scoreN instead of value1,...,valueN.
//! Client libraries are free to return a more appropriate data type (suggestion: an array with (value, score) arrays/tuples).
//! Return value: list of elements in the specified range (optionally with their scores, in case the WITHSCORES option is given).
//!
QtRedisReply QtRedisClient::redisZRange(const QString &key, const int start, const int stop, const bool withScores)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QString command = QString("ZREMRANGEBYRANK %1 %2 %3").arg(key).arg(start).arg(stop);
    if (withScores)
        command += QString(" WITHSCORES");

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает все элементы между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering, this command returns all the elements
//! in the sorted set at key with a value between min and max.
//!
//! If the elements in the sorted set have different scores, the returned elements are unspecified.
//!
//! The elements are considered to be ordered from lower to higher strings as compared byte-by-byte using the memcmp() C function.
//! Longer strings are considered greater than shorter strings if the common part is identical.
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! Keep in mind that if offset is large, the sorted set needs to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! How to specify intervals:
//! Valid start and stop must start with ( or [, in order to specify if the range item is respectively exclusive or inclusive.
//! The special values of + or - for start and stop have the special meaning or positively infinite and negatively infinite strings,
//! so for instance the command ZRANGEBYLEX myzset - + is guaranteed to return all the elements in the sorted set, if all the elements have the same score.
//!
//! Details on strings comparison:
//! Strings are compared as binary array of bytes. Because of how the ASCII character set is specified, this means that usually this also have the effect
//! of comparing normal ASCII characters in an obvious dictionary way. However this is not true if non plain ASCII strings are used (for example utf8 strings).
//!
//! However the user can apply a transformation to the encoded string so that the first part of the element inserted in the sorted set will compare as the user requires
//! for the specific application. For example if I want to add strings that will be compared in a case-insensitive way, but I still want to retrieve the real case when querying,
//! I can add strings in the following way:
//!
//! ZADD autocomplete 0 foo:Foo 0 bar:BAR 0 zap:zap
//!
//! Because of the first normalized part in every element (before the colon character), we are forcing a given comparison, however after the range is queries using ZRANGEBYLEX
//! the application can display to the user the second part of the string, after the colon.
//!
//! The binary nature of the comparison allows to use sorted sets as a general purpose index, for example the first part of the element can be a 64 bit big endian number:
//! since big endian numbers have the most significant bytes in the initial positions, the binary comparison will match the numerical comparison of the numbers.
//! This can be used in order to implement range queries on 64 bit values. As in the example below, after the first 8 bytes we can store the value of the element we are actually indexing.
//!
//! Return value: list of elements in the specified score range.
//!
//! Examples
//! redis>  ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis>  ZRANGEBYLEX myzset - [c
//! 1) "a"
//! 2) "b"
//! 3) "c"
//! redis>  ZRANGEBYLEX myzset - (c
//! 1) "a"
//! 2) "b"
//! redis>  ZRANGEBYLEX myzset [aaa (g
//! 1) "b"
//! 2) "c"
//! 3) "d"
//! 4) "e"
//! 5) "f"
//! redis>
//!
QtRedisReply QtRedisClient::redisZRangeByLex(const QString &key,
                                             const QString &min,
                                             const QString &max,
                                             const int offset,
                                             const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    QString command = QString("ZRANGEBYLEX %1 %2 %3").arg(key).arg(min).arg(max);
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Возвращает все элементы между min и max, включая min и max
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \param withScores Если true - вернуть количество элементов вместе с элементами
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! Returns all the elements in the sorted set at key with a score between min and max (including elements with score equal to min or max).
//! The elements are considered to be ordered from low to high scores.
//!
//! The elements having the same score are returned in lexicographical order (this follows from a property of the sorted set implementation in Redis and does not involve further computation).
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! Keep in mind that if offset is large, the sorted set needs to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! The optional WITHSCORES argument makes the command return both the element and its score, instead of the element alone. This option is available since Redis 2.0.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//!
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//! For example:
//!
//! ZRANGEBYSCORE zset (1 5
//! Will return all elements with 1 < score <= 5 while:
//!
//! ZRANGEBYSCORE zset (5 (10
//! Will return all the elements with 5 < score < 10 (5 and 10 excluded).
//!
//! Return value: list of elements in the specified score range (optionally with their scores).
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZRANGEBYSCORE myzset -inf +inf
//! 1) "one"
//! 2) "two"
//! 3) "three"
//! redis>  ZRANGEBYSCORE myzset 1 2
//! 1) "one"
//! 2) "two"
//! redis>  ZRANGEBYSCORE myzset (1 2
//! 1) "two"
//! redis>  ZRANGEBYSCORE myzset (1 (2
//! (empty list or set)
//! redis>
//!
QtRedisReply QtRedisClient::redisZRangeByScore(const QString &key,
                                               const QString &min,
                                               const QString &max,
                                               const bool withScores,
                                               const int offset,
                                               const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    QString command = QString("ZRANGEBYSCORE %1 %2 %3").arg(key).arg(min).arg(max);
    if (withScores)
        command += QString(" WITHSCORES");
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить ранг элемента отсортированного набора от мин к макс
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from low to high.
//! The rank (or index) is 0-based, which means that the member with the lowest score has rank 0.
//!
//! Use ZREVRANK to get the rank of an element with the scores ordered from high to low.
//! Return value:
//! - If member exists in the sorted set, Integer reply: the rank of member.
//! - If member does not exist in the sorted set or key does not exist, Bulk string reply: nil.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZRANK myzset "three"
//! (integer) 2
//! redis>  ZRANK myzset "four"
//! (nil)
//! redis>
//!
qlonglong QtRedisClient::redisZRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "ZRANK" << key << member;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Удалить элемент(ы) отсортированного набора
//! \param key Ключ
//! \param members Значения
//! \return
//!
//! Removes the specified members from the sorted set stored at key. Non existing members are ignored.
//!
//! An error is returned when key exists and does not hold a sorted set.
//! Return value: the number of members removed from the sorted set, not including non existing members.
//! Redis >= 2.4: Accepts multiple elements. In Redis versions older than 2.4 it was possible to remove a single member per call.
//!
qlonglong QtRedisClient::redisZRem(const QString &key, const QStringList &members)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (members.isEmpty()) {
        this->setLastError("Invalid members (Empty)!");
        return -1;
    }
    QStringList argv;
    argv << "ZREM" << key;
    for (const QString &member : members)
        argv << member;

    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, удаляет все элементы между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command removes all elements in the sorted set stored at key between the lexicographical range specified by min and max.
//!
//! The meaning of min and max are the same of the ZRANGEBYLEX command. Similarly, this command actually returns the same elements that ZRANGEBYLEX would return
//! if called with the same min and max arguments.
//!
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 0 aaaa 0 b 0 c 0 d 0 e
//! (integer) 5
//! redis>  ZADD myzset 0 foo 0 zap 0 zip 0 ALPHA 0 alpha
//! (integer) 5
//! redis>  ZRANGE myzset 0 -1
//! 1) "ALPHA"
//! 2) "aaaa"
//! 3) "alpha"
//! 4) "b"
//! 5) "c"
//! 6) "d"
//! 7) "e"
//! 8) "foo"
//! 9) "zap"
//! 10) "zip"
//! redis>  ZREMRANGEBYLEX myzset [alpha [omega
//! (integer) 6
//! redis>  ZRANGE myzset 0 -1
//! 1) "ALPHA"
//! 2) "aaaa"
//! 3) "zap"
//! 4) "zip"
//! redis>
//!
qlonglong QtRedisClient::redisZRemRangeByLex(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return -1;
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("ZREMRANGEBYLEX %1 %2 %3").arg(key).arg(min).arg(max)));
}

//!
//! \brief Удалить все элементе в отсортированном наборе между индексами start и stop
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \return
//!
//! Removes all elements in the sorted set stored at key with rank between start and stop.
//! Both start and stop are 0 -based indexes with 0 being the element with the lowest score.
//! These indexes can be negative numbers, where they indicate offsets starting at the element with the highest score.
//! For example: -1 is the element with the highest score, -2 the element with the second highest score and so forth.
//!
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREMRANGEBYRANK myzset 0 1
//! (integer) 2
//! redis>  ZRANGE myzset 0 -1 WITHSCORES
//! 1) "three"
//! 2) "3"
//! redis>
//!
qlonglong QtRedisClient::redisZRemRangeByRank(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("ZREMRANGEBYRANK %1 %2 %3").arg(key).arg(start).arg(stop)));
}

//!
//! \brief Удалить все элементе в отсортированном наборе между min и max, включая их.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! Removes all elements in the sorted set stored at key with a score between min and max (inclusive).
//!
//! Since version 2.1.6, min and max can be exclusive, following the syntax of ZRANGEBYSCORE.
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREMRANGEBYSCORE myzset -inf (2
//! (integer) 1
//! redis>  ZRANGE myzset 0 -1 WITHSCORES
//! 1) "two"
//! 2) "2"
//! 3) "three"
//! 4) "3"
//! redis>
//!
qlonglong QtRedisClient::redisZRemRangeByScore(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return -1;
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return -1;
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return -1;
    }
    return this->replyToLong(this->redisExecCommand(QString("ZREMRANGEBYSCORE %1 %2 %3").arg(key).arg(min).arg(max)));
}

//!
//! \brief Получить набор элементов в указанном диапазоне индексов start - stop
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \param withScores
//! \return
//!
//! Returns the specified range of elements in the sorted set stored at key. The elements are considered to be ordered from the highest to the lowest score.
//! Descending lexicographical order is used for elements with equal score.
//!
//! Apart from the reversed ordering, ZREVRANGE is similar to ZRANGE.
//! Return value: list of elements in the specified range (optionally with their scores).
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANGE myzset 0 -1
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis>  ZREVRANGE myzset 2 3
//! 1) "one"
//! redis>  ZREVRANGE myzset -2 -1
//! 1) "two"
//! 2) "one"
//! redis>
//!
QtRedisReply QtRedisClient::redisZRevRange(const QString &key, const int start, const int stop, const bool withScores)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QString command = QString("ZREVRANGE %1 %2 %3").arg(key).arg(start).arg(stop);
    if (withScores)
        command += QString(" WITHSCORES");

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает все элементы между min и max в обратном порядке.
//! \param key Ключ
//! \param max Макс
//! \param min Мин
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command returns all the elements in the sorted set at key with a value between max and min.
//!
//! Apart from the reversed ordering, ZREVRANGEBYLEX is similar to ZRANGEBYLEX.
//! Return value: list of elements in the specified score range.
//!
//! Examples
//! redis>  ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis>  ZREVRANGEBYLEX myzset [c -
//! 1) "c"
//! 2) "b"
//! 3) "a"
//! redis>  ZREVRANGEBYLEX myzset (c -
//! 1) "b"
//! 2) "a"
//! redis>  ZREVRANGEBYLEX myzset (g [aaa
//! 1) "f"
//! 2) "e"
//! 3) "d"
//! 4) "c"
//! 5) "b"
//! redis>
//!
QtRedisReply QtRedisClient::redisZRevRangeByLex(const QString &key,
                                                const QString &max,
                                                const QString &min,
                                                const int offset,
                                                const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    QString command = QString("ZREVRANGEBYLEX %1 %2 %3").arg(key).arg(max).arg(min);
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить все элементе в отсортированном наборе между min и max, включая их, в обратном порядке.
//! \param key Ключ
//! \param max Макс
//! \param min Мин
//! \param withScores
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! Returns all the elements in the sorted set at key with a score between max and min (including elements with score equal to max or min). In contrary to the default ordering of sorted sets, for this command the elements are considered to be ordered from high to low scores.
//!
//! The elements having the same score are returned in reverse lexicographical order.
//!
//! Apart from the reversed ordering, ZREVRANGEBYSCORE is similar to ZRANGEBYSCORE.
//! Return value
//!
//! Array reply: list of elements in the specified score range (optionally with their scores).
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANGEBYSCORE myzset +inf -inf
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis>  ZREVRANGEBYSCORE myzset 2 1
//! 1) "two"
//! 2) "one"
//! redis>  ZREVRANGEBYSCORE myzset 2 (1
//! 1) "two"
//! redis>  ZREVRANGEBYSCORE myzset (2 (1
//! (empty list or set)
//! redis>
//!
QtRedisReply QtRedisClient::redisZRevRangeByScore(const QString &key,
                                                  const QString &max,
                                                  const QString &min,
                                                  const bool withScores,
                                                  const int offset,
                                                  const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    if (min.isEmpty() || min.indexOf(" ") != -1) {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.isEmpty() || max.indexOf(" ") != -1) {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-") {
        this->setLastError("Invalid min!");
        return QtRedisReply();
    }
    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-") {
        this->setLastError("Invalid max!");
        return QtRedisReply();
    }
    QString command = QString("ZREVRANGEBYSCORE %1 %2 %3").arg(key).arg(max).arg(min);
    if (withScores)
        command += QString(" WITHSCORES");
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить ранг элемента отсортированного набора от макс к мин
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from high to low.
//! The rank (or index) is 0-based, which means that the member with the highest score has rank 0.
//!
//! Use ZRANK to get the rank of an element with the scores ordered from low to high.
//! Return value:
//! - If member exists in the sorted set, Integer reply: the rank of member.
//! - If member does not exist in the sorted set or key does not exist, Bulk string reply: nil.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANK myzset "one"
//! (integer) 2
//! redis>  ZREVRANK myzset "four"
//! (nil)
//! redis>
//!
qlonglong QtRedisClient::redisZRevRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return -1;
    }
    QStringList argv;
    argv << "ZREVRANK" << key << member;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief Получить счет элемента отсортированного набора
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the score of member in the sorted set at key.
//!
//! If member does not exist in the sorted set, or key does not exist, nil is returned.
//! Return value: the score of member (a double precision floating point number), represented as string.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZSCORE myzset "one"
//! "1"
//! redis>
//!
QtRedisReply QtRedisClient::redisZScore(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1) {
        this->setLastError("Invalid key!");
        return QtRedisReply();
    }
    QStringList argv;
    argv << "ZSCORE" << key << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вычислить объединение отсортированных наборов и сохранить результат в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей наборов
//! \param weightList Список "весов"
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Computes the union of numkeys sorted sets given by the specified keys, and stores the result in destination.
//! It is mandatory to provide the number of input keys (numkeys) before passing the input keys and the other (optional) arguments.
//!
//! By default, the resulting score of an element is the sum of its scores in the sorted sets where it exists.
//!
//! Using the WEIGHTS option, it is possible to specify a multiplication factor for each input sorted set.
//! This means that the score of every element in every input sorted set is multiplied by this factor before being passed to the aggregation function.
//! When WEIGHTS is not given, the multiplication factors default to 1.
//!
//! With the AGGREGATE option, it is possible to specify how the results of the union are aggregated.
//! This option defaults to SUM, where the score of an element is summed across the inputs where it exists.
//! When this option is set to either MIN or MAX, the resulting set will contain the minimum or maximum score of an element across the inputs where it exists.
//!
//! If destination already exists, it is overwritten.
//!
//! Return value: the number of elements in the resulting sorted set at destination.
//!
//! Examples
//! redis>  ZADD zset1 1 "one"
//! (integer) 1
//! redis>  ZADD zset1 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 1 "one"
//! (integer) 1
//! redis>  ZADD zset2 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 3 "three"
//! (integer) 1
//! redis>  ZUNIONSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 3
//! redis>  ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "three"
//! 4) "9"
//! 5) "two"
//! 6) "10"
//! redis>
//!
qlonglong QtRedisClient::redisZUnionStore(const QString &destKey,
                                          const QStringList &keyList,
                                          const QList<int> &weightList,
                                          const QString &aggregateFlag)
{
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1) {
        this->setLastError("Invalid destKey!");
        return -1;
    }
    if (keyList.isEmpty()) {
        this->setLastError("Invalid keyList (Empty)!");
        return -1;
    }
    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1) {
            this->setLastError(QString("Invalid key (%1)!").arg(key));
            return -1;
        }
    }
    QString weightListStr;
    for (const int weight : weightList) {
        if (weight <= 0)
            return -1;
        weightListStr += QString("%1 ").arg(weight);
    }
    if (!aggregateFlag.isEmpty()
        && aggregateFlag.toUpper() != QString("SUM")
        && aggregateFlag.toUpper() != QString("MIN")
        && aggregateFlag.toUpper() != QString("MAX")) {
        this->setLastError("Invalid aggregateFlag!");
        return -1;
    }

    QString command = QString("ZUNIONSTORE %1 %2 %3")
                      .arg(destKey)
                      .arg(keyList.size())
                      .arg(keyList.join(" "));

    if (!weightList.isEmpty())
        command += QString(" WEIGHTS %1").arg(weightListStr);
    if (!aggregateFlag.isEmpty())
        command += QString(" AGGREGATE %1").arg(aggregateFlag.toUpper());

    return this->replyToLong(this->redisExecCommand(command.trimmed()));
}


// ------------------------------------------------------------------------
// -- PUB/SUB COMMANDS ----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief QtRedisClient::redisPublish
//! \param channel
//! \param message
//! \return The number of clients that received the message.
//!
qlonglong QtRedisClient::redisPublish(const QString &channel, const QString &message)
{
    if (channel.trimmed().isEmpty() || message.trimmed().isEmpty()) {
        this->setLastError("Invalid input arguments!");
        return -1;
    }
    QStringList argv;
    argv << "PUBLISH" << channel.trimmed() << message.trimmed();
    return this->replyToLong(this->redisExecCommandArgv(argv));
}

//!
//! \brief QtRedisClient::redisPublish
//! \param channel
//! \param message
//! \return The number of clients that received the message.
//!
qlonglong QtRedisClient::redisPublish(const QString &channel, const QByteArray &message)
{
    if (channel.trimmed().isEmpty() || message.isEmpty()) {
        this->setLastError("Invalid input arguments!");
        return -1;
    }
    QList<QByteArray> argv;
    argv << "PUBLISH" << channel.trimmed().toUtf8() << message;
    return this->replyToLong(this->redisExecCommandArgv(argv));
}


// ------------------------------------------------------------------------
// -- TOOLS COMMANDS ------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Преобразовать ответ от сервера в qlonglong
//! \param reply Ответ от сервера
//! \return
//!
qlonglong QtRedisClient::replyToLong(const QtRedisReply &reply)
{
    if (reply.type() == QtRedisReply::ReplyType::Integer)
        return reply.intValue();

    this->setLastError("Invalid reply type!");
    return -1;
}

//!
//! \brief Преобразовать ответ от сервера в строку
//! \param reply Ответ от сервера
//! \return
//!
QString QtRedisClient::replyToString(const QtRedisReply &reply)
{
    if (reply.type() == QtRedisReply::ReplyType::String
        || reply.type() == QtRedisReply::ReplyType::Status)
        return reply.strValue();
    if (reply.type() == QtRedisReply::ReplyType::Error) {
        this->setLastError(reply.strValue());
        return reply.strValue();
    }

    this->setLastError("Invalid reply type!");
    return QString();
}

//!
//! \brief Преобразовать ответ от сервера в список
//! \param reply Ответ от сервера
//! \return
//!
QStringList QtRedisClient::replyToArray(const QtRedisReply &reply)
{
    if (reply.type() != QtRedisReply::ReplyType::Array) {
        this->setLastError("Invalid reply type!");
        return QStringList();
    }
    QStringList array;
    for (const QtRedisReply &replyObj : reply.arrayValue()) {
        if (replyObj.type() == QtRedisReply::ReplyType::String)
            array.append(replyObj.strValue());
    }
    return array;
}

//!
//! \brief Преобразовать ответ от сервера (replyType_Integer) в bool
//! \param reply Ответ от сервера
//! \return
//!
bool QtRedisClient::replyIntToBool(const QtRedisReply &reply)
{
    if (reply.type() != QtRedisReply::ReplyType::Integer) {
        this->setLastError("Invalid reply type!");
        return false;
    }
    if (reply.intValue() == 0)
        return false;

    return true;
}

//!
//! \brief Преобразовать ответ от сервера (replyType_Status|replyType_String) в bool
//! \param reply Ответ от сервера
//! \return
//!
bool QtRedisClient::replySimpleStringToBool(const QtRedisReply &reply)
{
    if (reply.type() != QtRedisReply::ReplyType::Status
        && reply.type() != QtRedisReply::ReplyType::String) {
        this->setLastError("Invalid reply type!");
        return false;
    }
    if (reply.strValue() == "OK")
        return true;

    return false;
}

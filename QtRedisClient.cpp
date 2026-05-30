#include "QtRedisClient.h"
#include <QDebug>

//!
//! \brief Конструктор класса
//!
QtRedisClient::QtRedisClient()
    : QObject()
    , QtRedisBase<QtRedisClient, QtRedisReply>()
{
}

//!
//! \brief Деструктор класса
//!
QtRedisClient::~QtRedisClient()
{
}

//!
//! \brief Версия библиотеки
//! \return
//!
QString QtRedisClient::libraryVersion()
{
    return QT_REDIS_CLIENT_VERSION_STR;
}


// ------------------------------------------------------------------------
// -- CONNECT/DISCONNECT FUNCTIONS ----------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Тип соединения с Redis
//! \return
//!
QtRedisTransporter::Type QtRedisClient::redisContextType()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisTransporter::Type::NoType;
    }
    return _transporter->type();
}

//!
//! \brief Тип работы в режиме подписки на каналы
//! \return
//!
QtRedisTransporter::ChannelMode QtRedisClient::redisContextChannelMode()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisTransporter::ChannelMode::CurrentConnection;
    }
    return _transporter->channelMode();
}

//!
//! \brief Проверить соединение с Redis
//! \return
//!
bool QtRedisClient::redisIsConnected()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
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
//! Данный метод использует протокол TCP.
//!
bool QtRedisClient::redisConnect(const QString &host,
                                 const int port,
                                 const int timeOutMsec,
                                 const QtRedisTransporter::ChannelMode contextChannelMode)
{
    QMutexLocker lock(&_mutex);
    if (host.isEmpty() || port == 0) {
        this->setLastError_safe("Invalid host or port!");
        return false;
    }
    if (_transporter
        && _transporter->host() == host
        && _transporter->port() == port)
        return true;

    if (!_transporter) {
        _transporter = std::make_shared<QtRedisTransporter>(contextChannelMode);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextConnected,
                         this, &QtRedisClient::contextConnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextDisconnected,
                         this, &QtRedisClient::contextDisconnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelMessage,
                         this, &QtRedisClient::incomingChannelMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelShardMessage,
                         this, &QtRedisClient::incomingChannelShardMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelPatternMessage,
                         this, &QtRedisClient::incomingChannelPatternMessage,
                         Qt::QueuedConnection);
    } else {
        _transporter->clearTransporter();
    }
    QString error;
    if (!_transporter->initTransporter(QtRedisTransporter::Type::Tcp, host, port, error)) {
        this->setLastError_safe(error);
        return false;
    }
    const bool isOk = _transporter->connectToServer(error, timeOutMsec);
    if (!isOk)
        this->setLastError_safe(error);
    return isOk;
}

//!
//! \brief Подключиться к серверу Redis
//! \param host IP-адрес
//! \param port Порт
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
//! Данный метод использует протокол TCP-SSL.
//!
bool QtRedisClient::redisConnectEncrypted(const QString &host,
                                          const int port,
                                          const int timeOutMsec,
                                          const QtRedisTransporter::ChannelMode contextChannelMode)
{
    QMutexLocker lock(&_mutex);
    if (host.isEmpty() || port == 0) {
        this->setLastError_safe("Invalid host or port!");
        return false;
    }
    if (_transporter
        && _transporter->host() == host
        && _transporter->port() == port
        && _transporter->type() == QtRedisTransporter::Type::Ssl)
        return true;

    if (!_transporter) {
        _transporter = std::make_shared<QtRedisTransporter>(contextChannelMode);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextConnected,
                         this, &QtRedisClient::contextConnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextDisconnected,
                         this, &QtRedisClient::contextDisconnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelMessage,
                         this, &QtRedisClient::incomingChannelMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelShardMessage,
                         this, &QtRedisClient::incomingChannelShardMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelPatternMessage,
                         this, &QtRedisClient::incomingChannelPatternMessage,
                         Qt::QueuedConnection);
    } else {
        _transporter->clearTransporter();
    }
    QString error;
    if (!_transporter->initTransporter(QtRedisTransporter::Type::Ssl, host, port, error)) {
        this->setLastError_safe(error);
        return false;
    }
    const bool isOk = _transporter->connectToServer(error, timeOutMsec);
    if (!isOk)
        this->setLastError_safe(error);
    return isOk;
}

#if defined(Q_OS_LINUX)
//!
//! \brief Подключиться к серверу Redis
//! \param sockPath Полный путь до unix-сокета
//! \param timeOutMsec Время ожидания в мсек
//! \return
//!
//! Данный метод использует UNIX-sockets.
//!
bool QtRedisClient::redisConnectUnix(const QString &sockPath,
                                     const int timeOutMsec,
                                     const QtRedisTransporter::ChannelMode contextChannelMode)
{
    QMutexLocker lock(&_mutex);
    if (sockPath.isEmpty()) {
        this->setLastError_safe("Invalid sockPath!");
        return false;
    }
    if (_transporter
        && _transporter->host() == sockPath
        && _transporter->type() == QtRedisTransporter::Type::Unix)
        return true;

    if (!_transporter) {
        _transporter = std::make_shared<QtRedisTransporter>(contextChannelMode);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextConnected,
                         this, &QtRedisClient::contextConnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::contextDisconnected,
                         this, &QtRedisClient::contextDisconnected,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelMessage,
                         this, &QtRedisClient::incomingChannelMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelShardMessage,
                         this, &QtRedisClient::incomingChannelShardMessage,
                         Qt::QueuedConnection);
        QObject::connect(_transporter.get(), &QtRedisTransporter::incomingChannelPatternMessage,
                         this, &QtRedisClient::incomingChannelPatternMessage,
                         Qt::QueuedConnection);
    } else {
        _transporter->clearTransporter();
    }
    QString error;
    if (!_transporter->initTransporter(QtRedisTransporter::Type::Unix, sockPath, -1, error)) {
        this->setLastError_safe(error);
        return false;
    }
    const bool isOk = _transporter->connectToServer(error, timeOutMsec);
    if (!isOk)
        this->setLastError_safe(error);
    return isOk;
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
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    QString error;
    const bool isOk = _transporter->reconnectToServer(error, timeOutMsec);
    if (!isOk)
        this->setLastError_safe(error);
    return isOk;
}

//!
//! \brief Отключиться от сервера Redis
//!
void QtRedisClient::redisDisconnect()
{
    QMutexLocker lock(&_mutex);
    if (_transporter)
        _transporter->clearTransporter();

    this->clearLastError_safe();
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(argv));
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

    const QtRedisReply buffReply = this->redisExecCommand(argv);
    if (msg.isEmpty()) {
        if (buffReply.type() != QtRedisReply::ReplyType::Status) {
            this->setLastError_safe("Invalid reply type!");
            return false;
        }
        if (buffReply.strValue() != "PONG") {
            this->setLastError_safe("Invalid reply result!");
            return false;
        }
    } else {
        if (buffReply.type() != QtRedisReply::ReplyType::String) {
            this->setLastError_safe("Invalid reply type!");
            return false;
        }
        if (buffReply.strValue() != msg) {
            this->setLastError_safe("Invalid reply result!");
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
    return this->redisExecCommand(argv);
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
        this->setLastError_safe("Invalid section!");
        return QMap<QString, QVariant>();
    }

    QString command("INFO");
    if (!section.isEmpty())
        command += QString(" %1").arg(section.toLower());

    const QtRedisReply buffReply = this->redisExecCommand(command.trimmed());
    if (buffReply.type() != QtRedisReply::ReplyType::String) {
        this->setLastError_safe("Invalid reply type!");
        return QMap<QString, QVariant>();
    }
    return QtRedisClient::redisInfoFromRawData(buffReply.rawValue());
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
        this->setLastError_safe("Invalid db index!");
        return false;
    }
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("SELECT %1").arg(dbIndex)));
}

//!
//! \brief Индекс текущей БД
//! \return
//!
int QtRedisClient::redisSelectedDb()
{
    QMutexLocker lock(&_mutex);
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return -1;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
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
    const QtRedisReply buffReply = this->redisExecCommand(QString("DBSIZE"));
    if (buffReply.type() != QtRedisReply::ReplyType::Integer) {
        this->setLastError_safe("Invalid reply type!");
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(command));
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(command));
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("SAVE")));
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
    const QtRedisReply buffReply = this->redisExecCommand(QString("LASTSAVE"));
    if (buffReply.type() != QtRedisReply::ReplyType::Integer) {
        this->setLastError_safe("Invalid reply type!");
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
        this->setLastError_safe("Invalid param!");
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
        this->setLastError_safe("Invalid param!");
        return false;
    }
    QStringList argv;
    argv << "CONFIG SET" << param << value;
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(argv));
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("CONFIG REWRITE")));
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
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("CONFIG RESETSTAT")));
}


// -- CLIENT-methods --

//!
//! \brief Список подключенных к серверу клиентов
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
QList<QtRedisClientInfo> QtRedisClient::redisClientList()
{
    const QtRedisReply buffReply = this->redisExecCommand(QString("CLIENT LIST"));
    if (buffReply.type() != QtRedisReply::ReplyType::String) {
        this->setLastError_safe("Invalid reply type!");
        return QList<QtRedisClientInfo>();
    }
    return QtRedisClientInfo::fromRawData(buffReply.rawValue_ref());
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
        this->setLastError_safe("Invalid connection name!");
        return false;
    }
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("CLIENT SETNAME %1").arg(connectionName)));
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
    return QtRedisReply::replyToString(this->redisExecCommand(QString("CLIENT GETNAME")));
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
        this->setLastError_safe("Invalid IP!");
        return false;
    }
    if (port == 0) {
        this->setLastError_safe("Invalid port!");
        return false;
    }
    QStringList argv;
    argv << "CLIENT" << "KILL" << "ADDR" << QString("%1:%2").arg(ip).arg(port);
    return QtRedisReply::replyIntToBool(this->redisExecCommand(argv));
}

//!
//! \brief Закрыть соединение с клиентом
//! \param id ID клиента
//! \return
//!
bool QtRedisClient::redisClientKill(const QString &id)
{
    if (id.isEmpty()) {
        this->setLastError_safe("Invalid ID!");
        return false;
    }
    return QtRedisReply::replyIntToBool(this->redisExecCommand(QString("CLIENT KILL ID %1").arg(id)));
}


// ------------------------------------------------------------------------
// -- PUB/SUB COMMANDS ----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Получить список каналов, на которые выполнена подписка
//! \param pattern
//! \return
//!
//! Syntax
//!
//! PUBSUB CHANNELS [pattern]
//!
//! Available since:
//!     2.8.0
//! Time complexity:
//!     O(N) where N is the number of active channels, and assuming constant time pattern matching (relatively short channels and patterns)
//! ACL categories:
//!     @pubsub, @slow
//!
//! Lists the currently active channels.
//!
//! An active channel is a Pub/Sub channel with one or more subscribers (excluding clients subscribed to patterns).
//!
//! If no pattern is specified, all the channels are listed, otherwise if pattern is specified only channels matching the specified glob-style pattern are listed.
//!
//! Cluster note: in a Redis Cluster clients can subscribe to every node, and can also publish to every other node.
//! The cluster will make sure that published messages are forwarded as needed.
//! That said, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
QStringList QtRedisClient::redisPubSubChannels(const QString &pattern)
{
    QStringList argv;
    argv << "PUBSUB" << "CHANNELS";
    if (!pattern.trimmed().isEmpty())
        argv << pattern.trimmed();
    return QtRedisReply::replyToStringList(this->redisExecCommand(argv));
}

//!
//! \brief Получить число уникальных шаблонов, на которые подписаны клиенты.
//! \return The number of unique patterns that are subscribed to by clients
//!
//! Syntax
//!
//! PUBSUB NUMPAT
//!
//! Available since:
//!     2.8.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @pubsub, @slow
//!
//! Returns the number of unique patterns that are subscribed to by clients (that are performed using the PSUBSCRIBE command).
//!
//! Note that this isn't the count of clients subscribed to patterns, but the total number of unique patterns all the clients are subscribed to.
//!
//! Cluster note: in a Redis Cluster clients can subscribe to every node, and can also publish to every other node.
//! The cluster will make sure that published messages are forwarded as needed.
//! That said, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
qlonglong QtRedisClient::redisPubSubNumPat()
{
    QStringList argv;
    argv << "PUBSUB" << "NUMPAT";
    return QtRedisReply::replyToLong(this->redisExecCommand(argv));
}

//!
//! \brief Получить число подписчиков на каждый канал
//! \param channel
//! \return
//!
//! Syntax
//!
//! PUBSUB NUMSUB [channel [channel ...]]
//!
//! Available since:
//!     2.8.0
//! Time complexity:
//!     O(N) for the NUMSUB subcommand, where N is the number of requested channels
//! ACL categories:
//!     @pubsub, @slow
//!
//! Returns the number of subscribers (exclusive of clients subscribed to patterns) for the specified channels.
//!
//! Note that it is valid to call this command without channels. In this case it will just return an empty list.
//!
//! Cluster note: in a Redis Cluster clients can subscribe to every node, and can also publish to every other node.
//! The cluster will make sure that published messages are forwarded as needed.
//! That said, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
QMap<QString, qlonglong> QtRedisClient::redisPubSubNumSub(const QString &channel)
{
    return this->redisPubSubNumSub(QStringList(channel));
}

//!
//! \brief Получить число подписчиков на каждый канал
//! \param channels
//! \return
//!
//! Syntax
//!
//! PUBSUB NUMSUB [channel [channel ...]]
//!
//! Available since:
//!     2.8.0
//! Time complexity:
//!     O(N) for the NUMSUB subcommand, where N is the number of requested channels
//! ACL categories:
//!     @pubsub, @slow
//!
//! Returns the number of subscribers (exclusive of clients subscribed to patterns) for the specified channels.
//!
//! Note that it is valid to call this command without channels. In this case it will just return an empty list.
//!
//! Cluster note: in a Redis Cluster clients can subscribe to every node, and can also publish to every other node.
//! The cluster will make sure that published messages are forwarded as needed.
//! That said, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
QMap<QString, qlonglong> QtRedisClient::redisPubSubNumSub(const QStringList &channels)
{
    // make valid list
    QStringList tmpChannels = channels;
    tmpChannels.removeAll(QString());
    // make args
    QStringList argv;
    argv << "PUBSUB" << "NUMSUB";
    if (!tmpChannels.isEmpty())
        argv << tmpChannels;
    const QtRedisReply reply = this->redisExecCommand(argv);
    if (reply.arrayValueSize() != tmpChannels.size() * 2)
        return QMap<QString, qlonglong>();
    // parse result
    QMap<QString, qlonglong> tmpRes;
    const QVector<QtRedisReply> replyArr = reply.arrayValue();
    for (int i = 0; i < replyArr.size(); i += 2) {
        const QtRedisReply chName = replyArr[i];
        const QtRedisReply chSubNum = replyArr[i + 1];
        tmpRes.insert(chName.strValue(), chSubNum.intValue());
    }
    return tmpRes;
}

//!
//! \brief Получить список каналов сегмента
//! \param pattern
//! \return
//!
//! Syntax
//!
//! PUBSUB SHARDCHANNELS [pattern]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of active shard channels, and assuming constant time pattern matching (relatively short shard channels).
//! ACL categories:
//!     @pubsub, @slow
//!
//! Lists the currently active shard channels.
//!
//! An active shard channel is a Pub/Sub shard channel with one or more subscribers.
//!
//! If no pattern is specified, all the channels are listed, otherwise if pattern is specified only channels matching the specified glob-style pattern are listed.
//!
//! The information returned about the active shard channels are at the shard level and not at the cluster level.
//! Examples
//!
//! > PUBSUB SHARDCHANNELS
//! 1) "orders"
//! > PUBSUB SHARDCHANNELS o*
//! 1) "orders"
//!
QStringList QtRedisClient::redisPubSubShardChannels(const QString &pattern)
{
    QStringList argv;
    argv << "PUBSUB" << "SHARDCHANNELS";
    if (!pattern.trimmed().isEmpty())
        argv << pattern;
    return QtRedisReply::replyToStringList(this->redisExecCommand(argv));
}

//!
//! \brief Получить число подписчиков на каждый канал сегмента
//! \param shardChannel
//! \return
//!
//! Syntax
//!
//! PUBSUB SHARDNUMSUB [shardchannel [shardchannel ...]]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) for the SHARDNUMSUB subcommand, where N is the number of requested shard channels
//! ACL categories:
//!     @pubsub, @slow
//!
//! Returns the number of subscribers for the specified shard channels.
//!
//! Note that it is valid to call this command without channels, in this case it will just return an empty list.
//!
//! Cluster note: in a Redis Cluster, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
//! Examples
//!
//! > PUBSUB SHARDNUMSUB orders
//! 1) "orders"
//! 2) (integer) 1
//!
QMap<QString, qlonglong> QtRedisClient::redisPubSubShardNumSub(const QString &shardChannel)
{
    return this->redisPubSubShardNumSub(QStringList(shardChannel));
}

//!
//! \brief Получить число подписчиков на каждый канал сегмента
//! \param shardChannels
//! \return
//!
//! Syntax
//!
//! PUBSUB SHARDNUMSUB [shardchannel [shardchannel ...]]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) for the SHARDNUMSUB subcommand, where N is the number of requested shard channels
//! ACL categories:
//!     @pubsub, @slow
//!
//! Returns the number of subscribers for the specified shard channels.
//!
//! Note that it is valid to call this command without channels, in this case it will just return an empty list.
//!
//! Cluster note: in a Redis Cluster, PUBSUB's replies in a cluster only report information from the node's Pub/Sub context, rather than the entire cluster.
//!
//! Examples
//!
//! > PUBSUB SHARDNUMSUB orders
//! 1) "orders"
//! 2) (integer) 1
//!
QMap<QString, qlonglong> QtRedisClient::redisPubSubShardNumSub(const QStringList &shardChannels)
{
    // make valid list
    QStringList tmpChannels = shardChannels;
    tmpChannels.removeAll(QString());
    // make args
    QStringList argv;
    argv << "PUBSUB" << "SHARDNUMSUB";
    if (!tmpChannels.isEmpty())
        argv << tmpChannels;
    const QtRedisReply reply = this->redisExecCommand(argv);
    if (reply.arrayValueSize() != tmpChannels.size() * 2)
        return QMap<QString, qlonglong>();
    // parse result
    QMap<QString, qlonglong> tmpRes;
    const QVector<QtRedisReply> replyArr = reply.arrayValue();
    for (int i = 0; i < replyArr.size(); i += 2) {
        const QtRedisReply chName = replyArr[i];
        const QtRedisReply chSubNum = replyArr[i + 1];
        tmpRes.insert(chName.strValue(), chSubNum.intValue());
    }
    return tmpRes;
}

//!
//! \brief Отправить сообщение в канал сообщений
//! \param channel Название канала
//! \param message Сообщение
//! \return The number of clients that received the message.
//!
//! Syntax
//!
//! PUBLISH channel message
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N+M) where N is the number of clients subscribed to the receiving channel and M is the total number of subscribed patterns (by any client).
//! ACL categories:
//!     @pubsub, @fast
//!
//! Posts a message to the given channel.
//!
//! In a Redis Cluster clients can publish to every node. The cluster makes sure that published messages are forwarded as needed,
//! so clients can subscribe to any channel by connecting to any one of the nodes.
//!
qlonglong QtRedisClient::redisPublish(const QString &channel, const QString &message)
{
    return this->redisPublish(channel, message.toUtf8());
}

//!
//! \brief Отправить сообщение в канал сообщений
//! \param channel Название канала
//! \param message Сообщение
//! \return The number of clients that received the message.
//!
//! Syntax
//!
//! PUBLISH channel message
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N+M) where N is the number of clients subscribed to the receiving channel and M is the total number of subscribed patterns (by any client).
//! ACL categories:
//!     @pubsub, @fast
//!
//! Posts a message to the given channel.
//!
//! In a Redis Cluster clients can publish to every node. The cluster makes sure that published messages are forwarded as needed,
//! so clients can subscribe to any channel by connecting to any one of the nodes.
//!
qlonglong QtRedisClient::redisPublish(const QString &channel, const QByteArray &message)
{
    if (channel.trimmed().isEmpty() || message.isEmpty()) {
        this->setLastError_safe("Invalid input arguments!");
        return -1;
    }
    QList<QByteArray> argv;
    argv << "PUBLISH" << channel.trimmed().toUtf8() << message;
    return QtRedisReply::replyToLong(this->redisExecCommand(argv));
}

//!
//! \brief Отправить сообщение в указанный канал сегмента.
//! \param shardChannel Название канала
//! \param message Сообщение
//! \return The number of clients that received the message.
//!
//! Syntax
//!
//! SPUBLISH shardchannel message
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of clients subscribed to the receiving shard channel.
//! ACL categories:
//!     @pubsub, @fast
//!
//! Posts a message to the given shard channel.
//!
//! In Redis Cluster, shard channels are assigned to slots by the same algorithm used to assign keys to slots.
//! A shard message must be sent to a node that own the slot the shard channel is hashed to.
//! The cluster makes sure that published shard messages are forwarded to all the node in the shard,
//! so clients can subscribe to a shard channel by connecting to any one of the nodes in the shard.
//!
//! For more information about sharded pubsub, see Sharded Pubsub (https://redis.io/topics/pubsub#sharded-pubsub).
//! Examples
//!
//! For example the following command publish to channel orders with a subscriber already waiting for message(s).
//!
//! > spublish orders hello
//! (integer) 1
//!
qlonglong QtRedisClient::redisSPublish(const QString &shardChannel, const QString &message)
{
    return this->redisSPublish(shardChannel, message.toUtf8());
}

//!
//! \brief Отправить сообщение в указанный канал сегмента.
//! \param shardChannel Название канала
//! \param message Сообщение
//! \return The number of clients that received the message.
//!
//! Syntax
//!
//! SPUBLISH shardchannel message
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of clients subscribed to the receiving shard channel.
//! ACL categories:
//!     @pubsub, @fast
//!
//! Posts a message to the given shard channel.
//!
//! In Redis Cluster, shard channels are assigned to slots by the same algorithm used to assign keys to slots.
//! A shard message must be sent to a node that own the slot the shard channel is hashed to.
//! The cluster makes sure that published shard messages are forwarded to all the node in the shard,
//! so clients can subscribe to a shard channel by connecting to any one of the nodes in the shard.
//!
//! For more information about sharded pubsub, see Sharded Pubsub (https://redis.io/topics/pubsub#sharded-pubsub).
//! Examples
//!
//! For example the following command publish to channel orders with a subscriber already waiting for message(s).
//!
//! > spublish orders hello
//! (integer) 1
//!
qlonglong QtRedisClient::redisSPublish(const QString &shardChannel, const QByteArray &message)
{
    if (shardChannel.trimmed().isEmpty() || message.isEmpty()) {
        this->setLastError_safe("Invalid input arguments!");
        return -1;
    }
    QList<QByteArray> argv;
    argv << "SPUBLISH" << shardChannel.trimmed().toUtf8() << message;
    return QtRedisReply::replyToLong(this->redisExecCommand(argv));
}

//!
//! \brief Подписаться на канал сообщений
//! \param channel Название канала
//! \return
//!
//! Syntax
//!
//! SUBSCRIBE channel [channel ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of channels to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the specified channels.
//!
//! Once the client enters the subscribed state it is not supposed to issue any other commands,
//! except for additional SUBSCRIBE, SSUBSCRIBE, PSUBSCRIBE, UNSUBSCRIBE, SUNSUBSCRIBE, PUNSUBSCRIBE, PING, RESET and QUIT commands.
//! However, if RESP3 is used (see HELLO) it is possible for a client to issue any commands while in subscribed state.
//!
bool QtRedisClient::redisSubscribe(const QString &channel)
{
    return this->redisSubscribe_safe("SUBSCRIBE", QStringList(channel));
}

//!
//! \brief Подписаться на канал сообщений
//! \param channels Названия каналов
//! \return
//!
//! Syntax
//!
//! SUBSCRIBE channel [channel ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of channels to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the specified channels.
//!
//! Once the client enters the subscribed state it is not supposed to issue any other commands,
//! except for additional SUBSCRIBE, SSUBSCRIBE, PSUBSCRIBE, UNSUBSCRIBE, SUNSUBSCRIBE, PUNSUBSCRIBE, PING, RESET and QUIT commands.
//! However, if RESP3 is used (see HELLO) it is possible for a client to issue any commands while in subscribed state.
//!
bool QtRedisClient::redisSubscribe(const QStringList &channels)
{
    return this->redisSubscribe_safe("SUBSCRIBE", channels);
}

//!
//! \brief Отписаться от канала сообщений
//! \param channel Название канала
//! \return
//!
//! Syntax
//!
//! UNSUBSCRIBE [channel [channel ...]]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of channels to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given channels, or from all of them if none is given.
//!
//! When no channels are specified, the client is unsubscribed from all the previously subscribed channels.
//! In this case, a message for every unsubscribed channel will be sent to the client.
//!
bool QtRedisClient::redisUnsubscribe(const QString &channel)
{
    return this->redisUnsubscribe_safe("UNSUBSCRIBE", QStringList(channel));
}

//!
//! \brief Отписаться от канала сообщений
//! \param channels Названия каналов
//! \return
//!
//! Syntax
//!
//! UNSUBSCRIBE [channel [channel ...]]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of channels to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given channels, or from all of them if none is given.
//!
//! When no channels are specified, the client is unsubscribed from all the previously subscribed channels.
//! In this case, a message for every unsubscribed channel will be sent to the client.
//!
bool QtRedisClient::redisUnsubscribe(const QStringList &channels)
{
    return this->redisUnsubscribe_safe("UNSUBSCRIBE", channels);
}

//!
//! \brief Подписаться на канал по заданному шаблону
//! \param pattern
//! \return
//!
//! Syntax
//!
//! PSUBSCRIBE pattern [pattern ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of patterns to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the given patterns.
//!
//! Supported glob-style patterns:
//!
//!     h?llo subscribes to hello, hallo and hxllo
//!     h*llo subscribes to hllo and heeeello
//!     h[ae]llo subscribes to hello and hallo, but not hillo
//!
//! Use \ to escape special characters if you want to match them verbatim.
//!
//! Once the client enters the subscribed state it is not supposed to issue any other commands,
//! except for additional SUBSCRIBE, SSUBSCRIBE, PSUBSCRIBE, UNSUBSCRIBE, SUNSUBSCRIBE, PUNSUBSCRIBE, PING, RESET and QUIT commands.
//! However, if RESP3 is used (see HELLO) it is possible for a client to issue any commands while in subscribed state.
//!
bool QtRedisClient::redisPSubscribe(const QString &pattern)
{
    return this->redisSubscribe_safe("PSUBSCRIBE", QStringList(pattern));
}

//!
//! \brief Подписаться на канал по заданному шаблону
//! \param patterns
//! \return
//!
//! Syntax
//!
//! PSUBSCRIBE pattern [pattern ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of patterns to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the given patterns.
//!
//! Supported glob-style patterns:
//!
//!     h?llo subscribes to hello, hallo and hxllo
//!     h*llo subscribes to hllo and heeeello
//!     h[ae]llo subscribes to hello and hallo, but not hillo
//!
//! Use \ to escape special characters if you want to match them verbatim.
//!
//! Once the client enters the subscribed state it is not supposed to issue any other commands,
//! except for additional SUBSCRIBE, SSUBSCRIBE, PSUBSCRIBE, UNSUBSCRIBE, SUNSUBSCRIBE, PUNSUBSCRIBE, PING, RESET and QUIT commands.
//! However, if RESP3 is used (see HELLO) it is possible for a client to issue any commands while in subscribed state.
//!
bool QtRedisClient::redisPSubscribe(const QStringList &patterns)
{
    return this->redisSubscribe_safe("PSUBSCRIBE", patterns);
}

//!
//! \brief Отписаться от канала по заданному шаблону
//! \param pattern
//! \return
//!
//! Syntax
//!
//! PUNSUBSCRIBE [pattern [pattern ...]]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of patterns to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given patterns, or from all of them if none is given.
//!
//! When no patterns are specified, the client is unsubscribed from all the previously subscribed patterns.
//! In this case, a message for every unsubscribed pattern will be sent to the client.
//!
bool QtRedisClient::redisPUnsubscribe(const QString &pattern)
{
    return this->redisUnsubscribe_safe("PUNSUBSCRIBE", QStringList(pattern));
}

//!
//! \brief Отписаться от канала по заданному шаблону
//! \param patterns
//! \return
//!
//! Syntax
//!
//! PUNSUBSCRIBE [pattern [pattern ...]]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) where N is the number of patterns to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given patterns, or from all of them if none is given.
//!
//! When no patterns are specified, the client is unsubscribed from all the previously subscribed patterns.
//! In this case, a message for every unsubscribed pattern will be sent to the client.
//!
bool QtRedisClient::redisPUnsubscribe(const QStringList &patterns)
{
    return this->redisUnsubscribe_safe("PUNSUBSCRIBE", patterns);
}

//!
//! \brief Подписаться на сообщения указанного канала сегмента.
//! \param shardChannel Название канала
//! \return
//!
//! Syntax
//!
//! SSUBSCRIBE shardchannel [shardchannel ...]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of shard channels to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the specified shard channels.
//!
//! In a Redis cluster, shard channels are assigned to slots by the same algorithm used to assign keys to slots.
//! Client(s) can subscribe to a node covering a slot (primary/replica) to receive the messages published.
//! All the specified shard channels needs to belong to a single slot to subscribe in a given SSUBSCRIBE call,
//! A client can subscribe to channels across different slots over separate SSUBSCRIBE call.
//!
//! For more information about sharded Pub/Sub, see Sharded Pub/Sub (https://redis.io/topics/pubsub#sharded-pubsub).
//!
//! Examples
//!
//! > ssubscribe orders
//! Reading messages... (press Ctrl-C to quit)
//! 1) "ssubscribe"
//! 2) "orders"
//! 3) (integer) 1
//! 1) "smessage"
//! 2) "orders"
//! 3) "hello"
//!
bool QtRedisClient::redisSSubscribe(const QString &shardChannel)
{
    return this->redisSubscribe_safe("SSUBSCRIBE", QStringList(shardChannel));
}

//!
//! \brief Подписаться на сообщения указанного канала сегмента.
//! \param shardChannels Название канала
//! \return
//!
//! Syntax
//!
//! SSUBSCRIBE shardchannel [shardchannel ...]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of shard channels to subscribe to.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Subscribes the client to the specified shard channels.
//!
//! In a Redis cluster, shard channels are assigned to slots by the same algorithm used to assign keys to slots.
//! Client(s) can subscribe to a node covering a slot (primary/replica) to receive the messages published.
//! All the specified shard channels needs to belong to a single slot to subscribe in a given SSUBSCRIBE call,
//! A client can subscribe to channels across different slots over separate SSUBSCRIBE call.
//!
//! For more information about sharded Pub/Sub, see Sharded Pub/Sub (https://redis.io/topics/pubsub#sharded-pubsub).
//!
//! Examples
//!
//! > ssubscribe orders
//! Reading messages... (press Ctrl-C to quit)
//! 1) "ssubscribe"
//! 2) "orders"
//! 3) (integer) 1
//! 1) "smessage"
//! 2) "orders"
//! 3) "hello"
//!
bool QtRedisClient::redisSSubscribe(const QStringList &shardChannels)
{
    return this->redisSubscribe_safe("SSUBSCRIBE", shardChannels);
}

//!
//! \brief Отписаться от сообщений указанного канала сегмента.
//! \param shardChannel
//! \return
//!
//! Syntax
//!
//! SUNSUBSCRIBE [shardchannel [shardchannel ...]]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of shard channels to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given shard channels, or from all of them if none is given.
//!
//! When no shard channels are specified, the client is unsubscribed from all the previously subscribed shard channels.
//! In this case a message for every unsubscribed shard channel will be sent to the client.
//!
//! Note: The global channels and shard channels needs to be unsubscribed from separately.
//!
//! For more information about sharded Pub/Sub, see Sharded Pub/Sub (https://redis.io/topics/pubsub#sharded-pubsub).
//!
bool QtRedisClient::redisSUnsubscribe(const QString &shardChannel)
{
    return this->redisUnsubscribe_safe("SUNSUBSCRIBE", QStringList(shardChannel));
}

//!
//! \brief Отписаться от сообщений указанного канала сегмента.
//! \param shardChannels
//! \return
//!
//! Syntax
//!
//! SUNSUBSCRIBE [shardchannel [shardchannel ...]]
//!
//! Available since:
//!     7.0.0
//! Time complexity:
//!     O(N) where N is the number of shard channels to unsubscribe.
//! ACL categories:
//!     @pubsub, @slow
//!
//! Unsubscribes the client from the given shard channels, or from all of them if none is given.
//!
//! When no shard channels are specified, the client is unsubscribed from all the previously subscribed shard channels.
//! In this case a message for every unsubscribed shard channel will be sent to the client.
//!
//! Note: The global channels and shard channels needs to be unsubscribed from separately.
//!
//! For more information about sharded Pub/Sub, see Sharded Pub/Sub (https://redis.io/topics/pubsub#sharded-pubsub).
//!
bool QtRedisClient::redisSUnsubscribe(const QStringList &shardChannels)
{
    return this->redisUnsubscribe_safe("SUNSUBSCRIBE", shardChannels);
}


// ------------------------------------------------------------------------
// -- Pipeline & Transaction COMMANDS -------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Создать объект для работы в режиме RedisPipeline
//! \return
//!
QtRedisPipeline QtRedisClient::createPipeline()
{
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisPipeline(nullptr);
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return QtRedisPipeline(nullptr);
    }
    return QtRedisPipeline(_transporter);
}

//!
//! \brief Создать объект для работы в режиме RedisTransaction
//! \return
//!
QtRedisTransaction QtRedisClient::createTransaction(const bool piped)
{
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisTransaction(nullptr, piped);
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return QtRedisTransaction(nullptr, piped);
    }
    return QtRedisTransaction(_transporter, piped);
}


// ------------------------------------------------------------------------
// -- TOOLS ---------------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Получить информацию о сервере в формате массива ключ-значение из "сырого" формата данных
//! \param data "Сырой" формат данных
//! \return
//!
QMap<QString, QVariant> QtRedisClient::redisInfoFromRawData(const QByteArray &data)
{
    if (data.isEmpty())
        return QMap<QString, QVariant>();

    QMap<QString, QVariant> buffInfo;
    QList<QByteArray> buffReplyList = data.split('\n');
    for (int i = 0; i < buffReplyList.size(); i++) {
        buffReplyList[i] = buffReplyList[i].trimmed();
        if (buffReplyList[i].isEmpty())
            continue;

        const QList<QByteArray> buffSplitValue = buffReplyList[i].split(':');
        if (buffSplitValue.size() < 2)
            continue;
        buffInfo.insert(QString::fromUtf8(buffSplitValue.at(0)), buffSplitValue.at(1));
    }
    return buffInfo;
}

//!
//! \brief Получить информацию о сервере в формате массива ключ-значение из строкового формата данных
//! \param data Строковый формат данных
//! \return
//!
QMap<QString, QVariant> QtRedisClient::redisInfoFromStringData(const QString &data)
{
    if (data.isEmpty())
        return QMap<QString, QVariant>();

    QMap<QString, QVariant> buffInfo;
    QStringList buffReplyList = data.split("\n");
    for (int i = 0; i < buffReplyList.size(); i++) {
        buffReplyList[i] = buffReplyList[i].trimmed();
        if (buffReplyList[i].isEmpty())
            continue;

        const QStringList buffSplitValue = buffReplyList[i].split(":");
        if (buffSplitValue.size() < 2)
            continue;
        buffInfo.insert(buffSplitValue.at(0), buffSplitValue.at(1));
    }
    return buffInfo;
}

// --- protected ---

//!
//! \brief Выполнить команду
//! \param command Команда
//! \return
//!
QtRedisReply QtRedisClient::processCommand(const QtRedisCommand &command)
{
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return QtRedisReply();
    }
    this->clearLastError_safe();
    QString error;
    bool isOk = false;
    const QtRedisReply reply = _transporter->sendCommand(command, error, &isOk);
    if (!error.isEmpty())
        this->setLastError_safe(error);
    return reply;
}

// --- private ---

//!
//! \brief Выполнить подписку на каналы
//! \param command Команда
//! \param channels Список каналов
//! \return
//!
bool QtRedisClient::redisSubscribe_safe(const QString &command, const QStringList &channels)
{
    QMutexLocker lock(&_mutex);
    // make valid list
    QStringList tmpChannels = channels;
    tmpChannels.removeAll(QString());
    // check
    if (tmpChannels.isEmpty()) {
        this->setLastError_safe("Invalid input arguments!");
        return false;
    }
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return false;
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return false;
    }
    QString error;
    if (!_transporter->isSubscribed()
        && !_transporter->subscribeToServer(error)) {
        this->setLastError_safe(!error.isEmpty() ? error : "Subscribe to the server failed!");
        return false;
    }
    // make args
    QStringList argv;
    argv << command.trimmed().toUpper() << tmpChannels;
    bool isOk = false;
    const QtRedisReply replyList = _transporter->sendChannelCommand(QtRedisCommand::fromString(argv.join(" ")), error, &isOk);
    if (!isOk) {
        this->setLastError_safe(error);
        return false;
    }

    // if 1 channel - check reply
    if (tmpChannels.size() == 1
        && replyList.type() == QtRedisReply::ReplyType::Array
        && replyList.arrayValueSize() == 3
        && replyList.arrayValue().at(0).strValue() == command.trimmed().toLower()
        && replyList.arrayValue().at(1).strValue() == tmpChannels.constFirst())
        return true;

    // check size
    if (replyList.arrayValueSize() != tmpChannels.size()) {
        this->setLastError_safe("Invalid reply list size!");
        return false;
    }

    isOk = true;
    for (int i = 0; i < tmpChannels.size(); i++) {
        const QtRedisReply reply = replyList.arrayValueAt(i);
        const QString channel = tmpChannels[i];
        if (reply.isError())
            this->setLastError_safe(reply.strValue());

        isOk = isOk
               && reply.type() == QtRedisReply::ReplyType::Array
               && reply.arrayValueSize() == 3
               && reply.arrayValue().at(0).strValue() == command.trimmed().toLower()
               && reply.arrayValue().at(1).strValue() == channel;
    }
    return isOk;
}

//!
//! \brief Выполнить отписку от каналов
//! \param command Команда
//! \param channels Список каналов
//! \return
//!
bool QtRedisClient::redisUnsubscribe_safe(const QString &command, const QStringList &channels)
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
    if (!_transporter->isSubscribed()) {
        this->setLastError_safe("Client is not subscribed!");
        return false;
    }
    // make valid list
    QStringList tmpChannels = channels;
    tmpChannels.removeAll(QString());
    // make args
    QStringList argv;
    argv << command.trimmed().toUpper();
    if (!tmpChannels.isEmpty())
        argv << tmpChannels;
    QString error;
    bool isOk = false;
    const QtRedisReply replyList = _transporter->sendChannelCommand(QtRedisCommand::fromString(argv.join(" ")), error, &isOk);
    if (!isOk) {
        this->setLastError_safe(error);
        return false;
    }

    // if 1 channel - check reply
    if (tmpChannels.size() == 1
        && replyList.type() == QtRedisReply::ReplyType::Array
        && replyList.arrayValueSize() == 3
        && replyList.arrayValue().at(0).strValue() == command.trimmed().toLower()
        && replyList.arrayValue().at(1).strValue() == tmpChannels.constFirst())
        return true;

    // check size
    if (replyList.arrayValueSize() != tmpChannels.size()) {
        this->setLastError_safe("Invalid reply list size!");
        return false;
    }

    isOk = true;
    for (int i = 0; i < tmpChannels.size(); i++) {
        const QtRedisReply reply = replyList.arrayValueAt(i);
        const QString channel = tmpChannels[i];
        if (reply.isError())
            this->setLastError_safe(reply.strValue());

        isOk = isOk
               && reply.type() == QtRedisReply::ReplyType::Array
               && reply.arrayValueSize() == 3
               && reply.arrayValue().at(0).strValue() == command.trimmed().toLower()
               && reply.arrayValue().at(1).strValue() == channel;
    }
    return isOk;
}

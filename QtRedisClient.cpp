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
//! Redis command: AUTH
//!
//! Syntax
//!
//! AUTH [username] password
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of passwords defined for the user
//! ACL categories:
//!     @fast, @connection
//!
//! The AUTH command authenticates the current connection in two cases:
//!
//!     If the Redis server is password protected via the requirepass option.
//!     A Redis 6.0 instance, or greater, is using the Redis ACL system.
//!
//! Redis versions prior of Redis 6 were only able to understand the one argument version of the command:
//!
//! AUTH <password>
//!
//! This form just authenticates against the password set with requirepass. In this configuration Redis will deny any command executed by the just connected clients, unless the connection gets authenticated via AUTH.
//!
//! If the password provided via AUTH matches the password in the configuration file, the server replies with the OK status code and starts accepting commands. Otherwise, an error is returned and the clients needs to try a new password.
//!
//! When Redis ACLs are used, the command should be given in an extended way:
//!
//! AUTH <username> <password>
//!
//! In order to authenticate the current connection with one of the connections defined in the ACL list (see ACL SETUSER) and the official ACL guide for more information.
//!
//! When ACLs are used, the single argument form of the command, where only the password is specified, assumes that the implicit username is "default".
//! Security notice
//!
//! Because of the high performance nature of Redis, it is possible to try a lot of passwords in parallel in very short time, so make sure to generate a strong and very long password so that this attack is infeasible. A good way to generate strong passwords is via the ACL GENPASS command.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK, or an error if the password, or username/password pair, is invalid.
//!
//! History
//!     Starting with Redis version 6.0.0: Added ACL style (username and password).
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
//! Redis command: PING
//!
//! Syntax
//!
//! PING [message]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast, @connection
//!
//! Returns PONG if no argument is provided, otherwise return a copy of the argument as a bulk. This command is useful for:
//!
//!     Testing whether a connection is still alive.
//!     Verifying the server's ability to serve data - an error is returned when this isn't the case (e.g., during load from persistence or accessing a stale replica).
//!     Measuring latency.
//!
//! If the client is subscribed to a channel or a pattern, it will instead return a multi-bulk with a "pong" in the first position and an empty bulk in the second position, unless an argument is provided in which case it returns a copy of the argument.
//!
//! Examples
//! PING
//! PING "hello world"
//!
//! RESP2/RESP3 Reply
//!
//! Any of the following:
//!
//!     Simple string reply: PONG when no argument is provided.
//!     Bulk string reply: the provided argument.
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
//! Redis command: ECHO
//!
//! Syntax
//!
//! ECHO message
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast, @connection
//!
//! Returns message.
//!
//! Examples
//! ECHO "Hello World!"
//!
//! RESP2/RESP3 Reply
//! Bulk string reply: the given string.
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
//! Redis command: INFO
//!
//! Syntax
//!
//! INFO [section [section ...]]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @slow, @dangerous
//!
//! The INFO command returns information and statistics about the server in a format that is simple to parse by computers and easy to read by humans.
//!
//! The optional parameter can be used to select a specific section of information:
//!
//!     server: General information about the Redis server
//!     clients: Client connections section
//!     memory: Memory consumption related information
//!     persistence: RDB and AOF related information
//!     stats: General statistics
//!     replication: Master/replica replication information
//!     cpu: CPU consumption statistics
//!     commandstats: Redis command statistics
//!     latencystats: Redis command latency percentile distribution statistics
//!     sentinel: Redis Sentinel section (only applicable to Sentinel instances)
//!     cluster: Redis Cluster section
//!     modules: Modules section
//!     keyspace: Database related statistics
//!     errorstats: Redis error statistics
//!
//! It can also take the following values:
//!
//!     all: Return all sections (excluding module generated ones)
//!     default: Return only the default set of sections
//!     everything: Includes all and modules
//!
//! When no parameter is provided, the default option is assumed.
//! INFO
//!
//! Notes
//!
//! Please note depending on the version of Redis some of the fields have been added or removed. A robust client application should therefore parse the result of this command by skipping unknown properties, and gracefully handle missing fields.
//!
//! Here is the description of fields for Redis >= 2.4.
//!
//! Here is the meaning of all fields in the server section:
//!
//!     redis_version: Version of the Redis server
//!     redis_git_sha1: Git SHA1
//!     redis_git_dirty: Git dirty flag
//!     redis_build_id: The build id
//!     redis_mode: The server's mode ("standalone", "sentinel" or "cluster")
//!     os: Operating system hosting the Redis server
//!     arch_bits: Architecture (32 or 64 bits)
//!     multiplexing_api: Event loop mechanism used by Redis
//!     atomicvar_api: Atomicvar API used by Redis
//!     gcc_version: Version of the GCC compiler used to compile the Redis server
//!     process_id: PID of the server process
//!     process_supervised: Supervised system ("upstart", "systemd", "unknown" or "no")
//!     run_id: Random value identifying the Redis server (to be used by Sentinel and Cluster)
//!     tcp_port: TCP/IP listen port
//!     server_time_usec: Epoch-based system time with microsecond precision
//!     uptime_in_seconds: Number of seconds since Redis server start
//!     uptime_in_days: Same value expressed in days
//!     hz: The server's current frequency setting
//!     configured_hz: The server's configured frequency setting
//!     lru_clock: Clock incrementing every minute, for LRU management
//!     executable: The path to the server's executable
//!     config_file: The path to the config file
//!     io_threads_active: Flag indicating if I/O threads are active
//!     shutdown_in_milliseconds: The maximum time remaining for replicas to catch up the replication before completing the shutdown sequence. This field is only present during shutdown.
//!
//! Here is the meaning of all fields in the clients section:
//!
//!     connected_clients: Number of client connections (excluding connections from replicas)
//!     cluster_connections: An approximation of the number of sockets used by the cluster's bus
//!     maxclients: The value of the maxclients configuration directive. This is the upper limit for the sum of connected_clients, connected_slaves and cluster_connections.
//!     client_recent_max_input_buffer: Biggest input buffer among current client connections
//!     client_recent_max_output_buffer: Biggest output buffer among current client connections
//!     blocked_clients: Number of clients pending on a blocking call (BLPOP, BRPOP, BRPOPLPUSH, BLMOVE, BZPOPMIN, BZPOPMAX)
//!     tracking_clients: Number of clients being tracked (CLIENT TRACKING)
//!     pubsub_clients: Number of clients in pubsub mode (SUBSCRIBE, PSUBSCRIBE, SSUBSCRIBE). Added in Redis 7.4
//!     watching_clients: Number of clients in watching mode (WATCH). Added in Redis 7.4
//!     clients_in_timeout_table: Number of clients in the clients timeout table
//!     total_watched_keys: Number of watched keys. Added in Redis 7.4.
//!     total_blocking_keys: Number of blocking keys. Added in Redis 7.2.
//!     total_blocking_keys_on_nokey: Number of blocking keys that one or more clients that would like to be unblocked when the key is deleted. Added in Redis 7.2.
//!
//! Here is the meaning of all fields in the memory section:
//!
//!     used_memory: Total number of bytes allocated by Redis using its allocator (either standard libc, jemalloc, or an alternative allocator such as tcmalloc)
//!     used_memory_human: Human readable representation of previous value
//!     used_memory_rss: Number of bytes that Redis allocated as seen by the operating system (a.k.a resident set size). This is the number reported by tools such as top(1) and ps(1)
//!     used_memory_rss_human: Human readable representation of previous value
//!     used_memory_peak: Peak memory consumed by Redis (in bytes)
//!     used_memory_peak_human: Human readable representation of previous value
//!     used_memory_peak_perc: The percentage of used_memory_peak out of used_memory
//!     used_memory_overhead: The sum in bytes of all overheads that the server allocated for managing its internal data structures
//!     used_memory_startup: Initial amount of memory consumed by Redis at startup in bytes
//!     used_memory_dataset: The size in bytes of the dataset (used_memory_overhead subtracted from used_memory)
//!     used_memory_dataset_perc: The percentage of used_memory_dataset out of the net memory usage (used_memory minus used_memory_startup)
//!     total_system_memory: The total amount of memory that the Redis host has
//!     total_system_memory_human: Human readable representation of previous value
//!     used_memory_lua: Number of bytes used by the Lua engine for EVAL scripts. Deprecated in Redis 7.0, renamed to used_memory_vm_eval
//!     used_memory_vm_eval: Number of bytes used by the script VM engines for EVAL framework (not part of used_memory). Added in Redis 7.0
//!     used_memory_lua_human: Human readable representation of previous value. Deprecated in Redis 7.0
//!     used_memory_scripts_eval: Number of bytes overhead by the EVAL scripts (part of used_memory). Added in Redis 7.0
//!     number_of_cached_scripts: The number of EVAL scripts cached by the server. Added in Redis 7.0
//!     number_of_functions: The number of functions. Added in Redis 7.0
//!     number_of_libraries: The number of libraries. Added in Redis 7.0
//!     used_memory_vm_functions: Number of bytes used by the script VM engines for Functions framework (not part of used_memory). Added in Redis 7.0
//!     used_memory_vm_total: used_memory_vm_eval + used_memory_vm_functions (not part of used_memory). Added in Redis 7.0
//!     used_memory_vm_total_human: Human readable representation of previous value.
//!     used_memory_functions: Number of bytes overhead by Function scripts (part of used_memory). Added in Redis 7.0
//!     used_memory_scripts: used_memory_scripts_eval + used_memory_functions (part of used_memory). Added in Redis 7.0
//!     used_memory_scripts_human: Human readable representation of previous value
//!     maxmemory: The value of the maxmemory configuration directive
//!     maxmemory_human: Human readable representation of previous value
//!     maxmemory_policy: The value of the maxmemory-policy configuration directive
//!     mem_fragmentation_ratio: Ratio between used_memory_rss and used_memory. Note that this doesn't only includes fragmentation, but also other process overheads (see the allocator_* metrics), and also overheads like code, shared libraries, stack, etc.
//!     mem_fragmentation_bytes: Delta between used_memory_rss and used_memory. Note that when the total fragmentation bytes is low (few megabytes), a high ratio (e.g. 1.5 and above) is not an indication of an issue.
//!     allocator_frag_ratio:: Ratio between allocator_active and allocator_allocated. This is the true (external) fragmentation metric (not mem_fragmentation_ratio).
//!     allocator_frag_bytes Delta between allocator_active and allocator_allocated. See note about mem_fragmentation_bytes.
//!     allocator_rss_ratio: Ratio between allocator_resident and allocator_active. This usually indicates pages that the allocator can and probably will soon release back to the OS.
//!     allocator_rss_bytes: Delta between allocator_resident and allocator_active
//!     rss_overhead_ratio: Ratio between used_memory_rss (the process RSS) and allocator_resident. This includes RSS overheads that are not allocator or heap related.
//!     rss_overhead_bytes: Delta between used_memory_rss (the process RSS) and allocator_resident
//!     allocator_allocated: Total bytes allocated form the allocator, including internal-fragmentation. Normally the same as used_memory.
//!     allocator_active: Total bytes in the allocator active pages, this includes external-fragmentation.
//!     allocator_resident: Total bytes resident (RSS) in the allocator, this includes pages that can be released to the OS (by MEMORY PURGE, or just waiting).
//!     allocator_muzzy: Total bytes of 'muzzy' memory (RSS) in the allocator. Muzzy memory is memory that has been freed, but not yet fully returned to the operating system. It can be reused immediately when needed or reclaimed by the OS when system pressure increases.
//!     mem_not_counted_for_evict: Used memory that's not counted for key eviction. This is basically transient replica and AOF buffers.
//!     mem_clients_slaves: Memory used by replica clients - Starting Redis 7.0, replica buffers share memory with the replication backlog, so this field can show 0 when replicas don't trigger an increase of memory usage.
//!     mem_clients_normal: Memory used by normal clients
//!     mem_cluster_links: Memory used by links to peers on the cluster bus when cluster mode is enabled.
//!     mem_aof_buffer: Transient memory used for AOF and AOF rewrite buffers
//!     mem_replication_backlog: Memory used by replication backlog
//!     mem_total_replication_buffers: Total memory consumed for replication buffers - Added in Redis 7.0.
//!     mem_allocator: Memory allocator, chosen at compile time.
//!     mem_overhead_db_hashtable_rehashing: Temporary memory overhead of database dictionaries currently being rehashed - Added in 7.4.
//!     active_defrag_running: When activedefrag is enabled, this indicates whether defragmentation is currently active, and the CPU percentage it intends to utilize.
//!     lazyfree_pending_objects: The number of objects waiting to be freed (as a result of calling UNLINK, or FLUSHDB and FLUSHALL with the ASYNC option)
//!     lazyfreed_objects: The number of objects that have been lazy freed.
//!
//! Ideally, the used_memory_rss value should be only slightly higher than used_memory. When rss >> used, a large difference may mean there is (external) memory fragmentation, which can be evaluated by checking allocator_frag_ratio, allocator_frag_bytes. When used >> rss, it means part of Redis memory has been swapped off by the operating system: expect some significant latencies.
//!
//! Because Redis does not have control over how its allocations are mapped to memory pages, high used_memory_rss is often the result of a spike in memory usage.
//!
//! When Redis frees memory, the memory is given back to the allocator, and the allocator may or may not give the memory back to the system. There may be a discrepancy between the used_memory value and memory consumption as reported by the operating system. It may be due to the fact memory has been used and released by Redis, but not given back to the system. The used_memory_peak value is generally useful to check this point.
//!
//! Additional introspective information about the server's memory can be obtained by referring to the MEMORY STATS command and the MEMORY DOCTOR.
//!
//! Here is the meaning of all fields in the persistence section:
//!
//!     loading: Flag indicating if the load of a dump file is on-going
//!     async_loading: Currently loading replication data-set asynchronously while serving old data. This means repl-diskless-load is enabled and set to swapdb. Added in Redis 7.0.
//!     current_cow_peak: The peak size in bytes of copy-on-write memory while a child fork is running
//!     current_cow_size: The size in bytes of copy-on-write memory while a child fork is running
//!     current_cow_size_age: The age, in seconds, of the current_cow_size value.
//!     current_fork_perc: The percentage of progress of the current fork process. For AOF and RDB forks it is the percentage of current_save_keys_processed out of current_save_keys_total.
//!     current_save_keys_processed: Number of keys processed by the current save operation
//!     current_save_keys_total: Number of keys at the beginning of the current save operation
//!     rdb_changes_since_last_save: Number of changes since the last dump
//!     rdb_bgsave_in_progress: Flag indicating a RDB save is on-going
//!     rdb_last_save_time: Epoch-based timestamp of last successful RDB save
//!     rdb_last_bgsave_status: Status of the last RDB save operation
//!     rdb_last_bgsave_time_sec: Duration of the last RDB save operation in seconds
//!     rdb_current_bgsave_time_sec: Duration of the on-going RDB save operation if any
//!     rdb_last_cow_size: The size in bytes of copy-on-write memory during the last RDB save operation
//!     rdb_last_load_keys_expired: Number of volatile keys deleted during the last RDB loading. Added in Redis 7.0.
//!     rdb_last_load_keys_loaded: Number of keys loaded during the last RDB loading. Added in Redis 7.0.
//!     aof_enabled: Flag indicating AOF logging is activated
//!     aof_rewrite_in_progress: Flag indicating a AOF rewrite operation is on-going
//!     aof_rewrite_scheduled: Flag indicating an AOF rewrite operation will be scheduled once the on-going RDB save is complete.
//!     aof_last_rewrite_time_sec: Duration of the last AOF rewrite operation in seconds
//!     aof_current_rewrite_time_sec: Duration of the on-going AOF rewrite operation if any
//!     aof_last_bgrewrite_status: Status of the last AOF rewrite operation
//!     aof_last_write_status: Status of the last write operation to the AOF
//!     aof_last_cow_size: The size in bytes of copy-on-write memory during the last AOF rewrite operation
//!     module_fork_in_progress: Flag indicating a module fork is on-going
//!     module_fork_last_cow_size: The size in bytes of copy-on-write memory during the last module fork operation
//!     aof_rewrites: Number of AOF rewrites performed since startup
//!     rdb_saves: Number of RDB snapshots performed since startup
//!
//! rdb_changes_since_last_save refers to the number of operations that produced some kind of changes in the dataset since the last time either SAVE or BGSAVE was called.
//!
//! If AOF is activated, these additional fields will be added:
//!
//!     aof_current_size: AOF current file size
//!     aof_base_size: AOF file size on latest startup or rewrite
//!     aof_pending_rewrite: Flag indicating an AOF rewrite operation will be scheduled once the on-going RDB save is complete.
//!     aof_buffer_length: Size of the AOF buffer
//!     aof_rewrite_buffer_length: Size of the AOF rewrite buffer. Note this field was removed in Redis 7.0
//!     aof_pending_bio_fsync: Number of fsync pending jobs in background I/O queue
//!     aof_delayed_fsync: Delayed fsync counter
//!
//! If a load operation is on-going, these additional fields will be added:
//!
//!     loading_start_time: Epoch-based timestamp of the start of the load operation
//!     loading_total_bytes: Total file size
//!     loading_rdb_used_mem: The memory usage of the server that had generated the RDB file at the time of the file's creation
//!     loading_loaded_bytes: Number of bytes already loaded
//!     loading_loaded_perc: Same value expressed as a percentage
//!     loading_eta_seconds: ETA in seconds for the load to be complete
//!
//! Here is the meaning of all fields in the stats section:
//!
//!     total_connections_received: Total number of connections accepted by the server
//!     total_commands_processed: Total number of commands processed by the server
//!     instantaneous_ops_per_sec: Number of commands processed per second
//!     total_net_input_bytes: The total number of bytes read from the network
//!     total_net_output_bytes: The total number of bytes written to the network
//!     total_net_repl_input_bytes: The total number of bytes read from the network for replication purposes
//!     total_net_repl_output_bytes: The total number of bytes written to the network for replication purposes
//!     instantaneous_input_kbps: The network's read rate per second in KB/sec
//!     instantaneous_output_kbps: The network's write rate per second in KB/sec
//!     instantaneous_input_repl_kbps: The network's read rate per second in KB/sec for replication purposes
//!     instantaneous_output_repl_kbps: The network's write rate per second in KB/sec for replication purposes
//!     rejected_connections: Number of connections rejected because of maxclients limit
//!     sync_full: The number of full resyncs with replicas
//!     sync_partial_ok: The number of accepted partial resync requests
//!     sync_partial_err: The number of denied partial resync requests
//!     expired_subkeys: The number of hash field expiration events
//!     expired_keys: Total number of key expiration events
//!     expired_stale_perc: The percentage of keys probably expired
//!     expired_time_cap_reached_count: The count of times that active expiry cycles have stopped early
//!     expire_cycle_cpu_milliseconds: The cumulative amount of time spent on active expiry cycles
//!     evicted_keys: Number of evicted keys due to maxmemory limit
//!     evicted_clients: Number of evicted clients due to maxmemory-clients limit. Added in Redis 7.0.
//!     evicted_scripts: Number of evicted EVAL scripts due to LRU policy, see EVAL for more details. Added in Redis 7.4.
//!     total_eviction_exceeded_time: Total time used_memory was greater than maxmemory since server startup, in milliseconds
//!     current_eviction_exceeded_time: The time passed since used_memory last rose above maxmemory, in milliseconds
//!     keyspace_hits: Number of successful lookup of keys in the main dictionary
//!     keyspace_misses: Number of failed lookup of keys in the main dictionary
//!     pubsub_channels: Global number of pub/sub channels with client subscriptions
//!     pubsub_patterns: Global number of pub/sub pattern with client subscriptions
//!     pubsubshard_channels: Global number of pub/sub shard channels with client subscriptions. Added in Redis 7.0.3
//!     latest_fork_usec: Duration of the latest fork operation in microseconds
//!     total_forks: Total number of fork operations since the server start
//!     migrate_cached_sockets: The number of sockets open for MIGRATE purposes
//!     slave_expires_tracked_keys: The number of keys tracked for expiry purposes (applicable only to writable replicas)
//!     active_defrag_hits: Number of value reallocations performed by active the defragmentation process
//!     active_defrag_misses: Number of aborted value reallocations started by the active defragmentation process
//!     active_defrag_key_hits: Number of keys that were actively defragmented
//!     active_defrag_key_misses: Number of keys that were skipped by the active defragmentation process
//!     total_active_defrag_time: Total time memory fragmentation was over the limit, in milliseconds
//!     current_active_defrag_time: The time passed since memory fragmentation last was over the limit, in milliseconds
//!     tracking_total_keys: Number of keys being tracked by the server
//!     tracking_total_items: Number of items, that is the sum of clients number for each key, that are being tracked
//!     tracking_total_prefixes: Number of tracked prefixes in server's prefix table (only applicable for broadcast mode)
//!     unexpected_error_replies: Number of unexpected error replies, that are types of errors from an AOF load or replication
//!     total_error_replies: Total number of issued error replies, that is the sum of rejected commands (errors prior command execution) and failed commands (errors within the command execution)
//!     dump_payload_sanitizations: Total number of dump payload deep integrity validations (see sanitize-dump-payload config).
//!     total_reads_processed: Total number of read events processed
//!     total_writes_processed: Total number of write events processed
//!     io_threaded_reads_processed: Number of read events processed by the main and I/O threads
//!     io_threaded_writes_processed: Number of write events processed by the main and I/O threads
//!     client_query_buffer_limit_disconnections: Total number of disconnections due to client reaching query buffer limit
//!     client_output_buffer_limit_disconnections: Total number of disconnections due to client reaching output buffer limit
//!     reply_buffer_shrinks: Total number of output buffer shrinks
//!     reply_buffer_expands: Total number of output buffer expands
//!     eventloop_cycles: Total number of eventloop cycles
//!     eventloop_duration_sum: Total time spent in the eventloop in microseconds (including I/O and command processing)
//!     eventloop_duration_cmd_sum: Total time spent on executing commands in microseconds
//!     instantaneous_eventloop_cycles_per_sec: Number of eventloop cycles per second
//!     instantaneous_eventloop_duration_usec: Average time spent in a single eventloop cycle in microseconds
//!     acl_access_denied_auth: Number of authentication failures
//!     acl_access_denied_cmd: Number of commands rejected because of access denied to the command
//!     acl_access_denied_key: Number of commands rejected because of access denied to a key
//!     acl_access_denied_channel: Number of commands rejected because of access denied to a channel
//!
//! Here is the meaning of all fields in the replication section:
//!
//!     role: Value is "master" if the instance is replica of no one, or "slave" if the instance is a replica of some master instance. Note that a replica can be master of another replica (chained replication).
//!     master_failover_state: The state of an ongoing failover, if any.
//!     master_replid: The replication ID of the Redis server.
//!     master_replid2: The secondary replication ID, used for PSYNC after a failover.
//!     master_repl_offset: The server's current replication offset
//!     second_repl_offset: The offset up to which replication IDs are accepted
//!     repl_backlog_active: Flag indicating replication backlog is active
//!     repl_backlog_size: Total size in bytes of the replication backlog buffer
//!     repl_backlog_first_byte_offset: The master offset of the replication backlog buffer
//!     repl_backlog_histlen: Size in bytes of the data in the replication backlog buffer
//!
//! If the instance is a replica, these additional fields are provided:
//!
//!     master_host: Host or IP address of the master
//!     master_port: Master listening TCP port
//!     master_link_status: Status of the link (up/down)
//!     master_last_io_seconds_ago: Number of seconds since the last interaction with master
//!     master_sync_in_progress: Indicate the master is syncing to the replica
//!     slave_read_repl_offset: The read replication offset of the replica instance.
//!     slave_repl_offset: The replication offset of the replica instance
//!     slave_priority: The priority of the instance as a candidate for failover
//!     slave_read_only: Flag indicating if the replica is read-only
//!     replica_announced: Flag indicating if the replica is announced by Sentinel.
//!
//! If a SYNC operation is on-going, these additional fields are provided:
//!
//!     master_sync_total_bytes: Total number of bytes that need to be transferred. this may be 0 when the size is unknown (for example, when the repl-diskless-sync configuration directive is used)
//!     master_sync_read_bytes: Number of bytes already transferred
//!     master_sync_left_bytes: Number of bytes left before syncing is complete (may be negative when master_sync_total_bytes is 0)
//!     master_sync_perc: The percentage master_sync_read_bytes from master_sync_total_bytes, or an approximation that uses loading_rdb_used_mem when master_sync_total_bytes is 0
//!     master_sync_last_io_seconds_ago: Number of seconds since last transfer I/O during a SYNC operation
//!
//! If the link between master and replica is down, an additional field is provided:
//!
//!     master_link_down_since_seconds: Number of seconds since the link is down
//!
//! The following field is always provided:
//!
//!     connected_slaves: Number of connected replicas
//!
//! If the server is configured with the min-slaves-to-write (or starting with Redis 5 with the min-replicas-to-write) directive, an additional field is provided:
//!
//!     min_slaves_good_slaves: Number of replicas currently considered good
//!
//! For each replica, the following line is added:
//!
//!     slaveXXX: id, IP address, port, state, offset, lag
//!
//! Here is the meaning of all fields in the cpu section:
//!
//!     used_cpu_sys: System CPU consumed by the Redis server, which is the sum of system CPU consumed by all threads of the server process (main thread and background threads)
//!     used_cpu_user: User CPU consumed by the Redis server, which is the sum of user CPU consumed by all threads of the server process (main thread and background threads)
//!     used_cpu_sys_children: System CPU consumed by the background processes
//!     used_cpu_user_children: User CPU consumed by the background processes
//!     used_cpu_sys_main_thread: System CPU consumed by the Redis server main thread
//!     used_cpu_user_main_thread: User CPU consumed by the Redis server main thread
//!
//! The commandstats section provides statistics based on the command type, including the number of calls that reached command execution (not rejected), the total CPU time consumed by these commands, the average CPU consumed per command execution, the number of rejected calls (errors prior command execution), and the number of failed calls (errors within the command execution).
//!
//! For each command type, the following line is added:
//!
//!     cmdstat_XXX: calls=XXX,usec=XXX,usec_per_call=XXX,rejected_calls=XXX,failed_calls=XXX
//!
//! The latencystats section provides latency percentile distribution statistics based on the command type.
//!
//! By default, the exported latency percentiles are the p50, p99, and p999. If you need to change the exported percentiles, use CONFIG SET latency-tracking-info-percentiles "50.0 99.0 99.9".
//!
//! This section requires the extended latency monitoring feature to be enabled (by default it's enabled). If you need to enable it, use CONFIG SET latency-tracking yes.
//!
//! For each command type, the following line is added:
//!
//!     latency_percentiles_usec_XXX: p<percentile 1>=<percentile 1 value>,p<percentile 2>=<percentile 2 value>,...
//!
//! The errorstats section enables keeping track of the different errors that occurred within Redis, based upon the reply error prefix ( The first word after the "-", up to the first space. Example: ERR ).
//!
//! For each error type, the following line is added:
//!
//!     errorstat_XXX: count=XXX
//!
//! If the server detects that this section was flooded with an excessive number of errors, it will be disabled, show a single ERRORSTATS_DISABLED error, and print the errors to the server log. This can be reset by CONFIG RESETSTAT.
//!
//! The sentinel section is only available in Redis Sentinel instances. It consists of the following fields:
//!
//!     sentinel_masters: Number of Redis masters monitored by this Sentinel instance
//!     sentinel_tilt: A value of 1 means this sentinel is in TILT mode
//!     sentinel_tilt_since_seconds: Duration in seconds of current TILT, or -1 if not TILTed. Added in Redis 7.0.0
//!     sentinel_running_scripts: The number of scripts this Sentinel is currently executing
//!     sentinel_scripts_queue_length: The length of the queue of user scripts that are pending execution
//!     sentinel_simulate_failure_flags: Flags for the SENTINEL SIMULATE-FAILURE command
//!
//! The cluster section currently only contains a unique field:
//!
//!     cluster_enabled: Indicate Redis cluster is enabled
//!
//! The modules section contains additional information about loaded modules if the modules provide it. The field part of properties lines in this section is always prefixed with the module's name.
//!
//! The keyspace section provides statistics on the main dictionary of each database. The statistics are the number of keys, and the number of keys with an expiration.
//!
//! For each database, the following line is added:
//!
//!     dbXXX: keys=XXX,expires=XXX,avg_ttl=XXX,subexpiry=XXX
//!
//! The debug section contains experimental metrics, which might change or get removed in future versions. It won't be included when INFO or INFO ALL are called, and it is returned only when INFO DEBUG is used.
//!
//!     eventloop_duration_aof_sum: Total time spent on flushing AOF in eventloop in microseconds.
//!     eventloop_duration_cron_sum: Total time consumption of cron in microseconds (including serverCron and beforeSleep, but excluding IO and AOF flushing).
//!     eventloop_duration_max: The maximal time spent in a single eventloop cycle in microseconds.
//!     eventloop_cmd_per_cycle_max: The maximal number of commands processed in a single eventloop cycle.
//!     allocator_allocated_lua: Total bytes allocated from the allocator specifically for Lua, including internal-fragmentation.
//!     allocator_active_lua: Total bytes in the allocator active pages specifically for Lua, including external-fragmentation.
//!     allocator_resident_lua: Total bytes resident (RSS) in the allocator specifically for Lua. This includes pages that can be released to the OS (by MEMORY PURGE, or just waiting).
//!     allocator_frag_bytes_lua: Delta between allocator_active_lua and allocator_allocated_lua.
//!
//! A note about the word slave used in this man page: Starting with Redis 5, if not for backward compatibility, the Redis project no longer uses the word slave. Unfortunately in this command the word slave is part of the protocol, so we'll be able to remove such occurrences only when this API will be naturally deprecated.
//!
//! Modules generated sections: Starting with Redis 6, modules can inject their information into the INFO command. These are excluded by default even when the all argument is provided (it will include a list of loaded modules but not their generated info fields). To get these you must use either the modules argument or everything.
//!
//! RESP2/RESP3 Reply
//!
//! Bulk string reply: a map of info fields, one field per line in the form of <field>:<value> where the value can be a comma separated map like <key>=<val>. Also contains section header lines starting with # and blank lines.
//!
//! Lines can contain a section name (starting with a # character) or a property. All the properties are in the form of field:value terminated by \r\n.
//!
//! History
//!     Starting with Redis version 7.0.0: Added support for taking multiple section arguments.
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
//! Redis command: TIME
//!
//! Syntax
//!
//! TIME
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast
//!
//! The TIME command returns the current server time as a two items lists: a Unix timestamp and the amount of microseconds already elapsed in the current second. Basically the interface is very similar to the one of the gettimeofday system call.
//!
//! Examples
//! TIME TIME
//!
//! RESP2/RESP3 Reply
//! Array reply: specifically, a two-element array consisting of the Unix timestamp in seconds and the microseconds' count.
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
//! Redis command: SELECT
//!
//! Syntax
//!
//! SELECT index
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @fast, @connection
//!
//! Select the Redis logical database having the specified zero-based numeric index. New connections always use the database 0.
//!
//! Selectable Redis databases are a form of namespacing: all databases are still persisted in the same RDB / AOF file. However different databases can have keys with the same name, and commands like FLUSHDB, SWAPDB or RANDOMKEY work on specific databases.
//!
//! In practical terms, Redis databases should be used to separate different keys belonging to the same application (if needed), and not to use a single Redis instance for multiple unrelated applications.
//!
//! When using Redis Cluster, the SELECT command cannot be used, since Redis Cluster only supports database zero. In the case of a Redis Cluster, having multiple databases would be useless and an unnecessary source of complexity. Commands operating atomically on a single database would not be possible with the Redis Cluster design and goals.
//!
//! Since the currently selected database is a property of the connection, clients should track the currently selected database and re-select it on reconnection. While there is no command in order to query the selected database in the current connection, the CLIENT LIST output shows, for each client, the currently selected database.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
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
//! Redis command: DBSIZE
//!
//! Syntax
//!
//! DBSIZE
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @read, @fast
//!
//! Return the number of keys in the currently-selected database.
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of keys in the currently-selected database.
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
//! Redis command: FLUSHALL
//!
//! Syntax
//!
//! FLUSHALL [ASYNC | SYNC]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of keys in all databases
//! ACL categories:
//!     @keyspace, @write, @slow, @dangerous
//!
//! Delete all the keys of all the existing databases, not just the currently selected one. This command never fails.
//!
//! By default, FLUSHALL will synchronously flush all the databases. Starting with Redis 6.2, setting the lazyfree-lazy-user-flush configuration directive to "yes" changes the default flush mode to asynchronous.
//!
//! It is possible to use one of the following modifiers to dictate the flushing mode explicitly:
//!
//!     ASYNC: flushes the databases asynchronously
//!     SYNC: flushes the databases synchronously
//!
//! Note: an asynchronous FLUSHALL command only deletes keys that were present at the time the command was invoked. Keys created during an asynchronous flush will be unaffected.
//! Behavior change history
//!
//!     >= 6.2.0: Default flush behavior now configurable by the lazyfree-lazy-user-flush configuration directive.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
//! History
//!     Starting with Redis version 4.0.0: Added the ASYNC flushing mode modifier.
//!     Starting with Redis version 6.2.0: Added the SYNC flushing mode modifier.
//!
bool QtRedisClient::redisFlushAll(const bool async)
{
    // TODO add flag SYNC!
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
//! Redis command: FLUSHDB
//!
//! Syntax
//!
//! FLUSHDB [ASYNC | SYNC]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of keys in the selected database
//! ACL categories:
//!     @keyspace, @write, @slow, @dangerous
//!
//! Delete all the keys of the currently selected DB. This command never fails.
//!
//! By default, FLUSHDB will synchronously flush all keys from the database. Starting with Redis 6.2, setting the lazyfree-lazy-user-flush configuration directive to "yes" changes the default flush mode to asynchronous.
//!
//! It is possible to use one of the following modifiers to dictate the flushing mode explicitly:
//!
//!     ASYNC: flushes the database asynchronously
//!     SYNC: flushes the database synchronously
//!
//! Note: an asynchronous FLUSHDB command only deletes keys that were present at the time the command was invoked. Keys created during an asynchronous flush will be unaffected.
//! Behavior change history
//!
//!     >= 6.2.0: Default flush behavior now configurable by the lazyfree-lazy-user-flush configuration directive.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
//! History
//!     Starting with Redis version 4.0.0: Added the ASYNC flushing mode modifier.
//!     Starting with Redis version 6.2.0: Added the SYNC flushing mode modifier.
//!
bool QtRedisClient::redisFlushDb(const bool async)
{
    // TODO add flag SYNC!
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
//! Redis command: SAVE
//!
//! Syntax
//!
//! SAVE
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of keys in all databases
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! The SAVE commands performs a synchronous save of the dataset producing a point in time snapshot of all the data inside the Redis instance, in the form of an RDB file.
//!
//! You almost never want to call SAVE in production environments where it will block all the other clients. Instead usually BGSAVE is used. However in case of issues preventing Redis to create the background saving child (for instance errors in the fork(2) system call), the SAVE command can be a good last resort to perform the dump of the latest dataset.
//!
//! See the persistence documentation for detailed information.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
bool QtRedisClient::redisSave()
{
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("SAVE")));
}

//!
//! \brief Сохранение данных в фоновом режиме
//! \return
//!
//! Redis command: BGSAVE
//!
//! Syntax
//!
//! BGSAVE [SCHEDULE]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! Save the DB in background.
//!
//! Normally the OK code is immediately returned. Redis forks, the parent continues to serve the clients, the child saves the DB on disk then exits.
//!
//! An error is returned if there is already a background save running or if there is another non-background-save process running, specifically an in-progress AOF rewrite.
//!
//! If BGSAVE SCHEDULE is used, the command will immediately return OK when an AOF rewrite is in progress and schedule the background save to run at the next opportunity.
//!
//! A client may be able to check if the operation succeeded using the LASTSAVE command.
//!
//! See the persistence documentation for detailed information.
//!
//! RESP2/RESP3 Reply
//! One of the following:
//!
//!     Simple string reply: Background saving started.
//!     Simple string reply: Background saving scheduled.
//!
//! History
//!     Starting with Redis version 3.2.2: Added the SCHEDULE option.
//!
QtRedisReply QtRedisClient::redisBgSave()
{
    // TODO add flag SCHEDULE
    return this->redisExecCommand(QString("BGSAVE"));
}

//!
//! \brief Время последнего сохранения (utc)
//! \return
//!
//! Redis command: LASTSAVE
//!
//! Syntax
//!
//! LASTSAVE
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @admin, @fast, @dangerous
//!
//! Return the UNIX TIME of the last DB save executed with success. A client may check if a BGSAVE command succeeded reading the LASTSAVE value, then issuing a BGSAVE command and checking at regular intervals every N seconds if LASTSAVE changed. Redis considers the database saved successfully at startup.
//!
//! RESP2/RESP3 Reply
//! Integer reply: UNIX TIME of the last DB save executed with success.
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
//! Redis command: CONFIG GET
//!
//! Syntax
//!
//! CONFIG GET parameter [parameter ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) when N is the number of configuration parameters provided
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! The CONFIG GET command is used to read the configuration parameters of a running Redis server. Not all the configuration parameters are supported in Redis 2.4, while Redis 2.6 can read the whole configuration of a server using this command.
//!
//! The symmetric command used to alter the configuration at run time is CONFIG SET.
//!
//! CONFIG GET takes multiple arguments, which are glob-style patterns. Any configuration parameter matching any of the patterns are reported as a list of key-value pairs. Example:
//!
//! redis> config get *max-*-entries* maxmemory
//!  1) "maxmemory"
//!  2) "0"
//!  3) "hash-max-listpack-entries"
//!  4) "512"
//!  5) "hash-max-ziplist-entries"
//!  6) "512"
//!  7) "set-max-intset-entries"
//!  8) "512"
//!  9) "zset-max-listpack-entries"
//! 10) "128"
//! 11) "zset-max-ziplist-entries"
//! 12) "128"
//!
//! You can obtain a list of all the supported configuration parameters by typing CONFIG GET * in an open redis-cli prompt.
//!
//! All the supported parameters have the same meaning of the equivalent configuration parameter used in the redis.conf file:
//!
//! Note that you should look at the redis.conf file relevant to the version you're working with as configuration options might change between versions. The link above is to the latest development version.
//!
//! RESP2 Reply
//! Array reply: a list of configuration parameters matching the provided arguments.
//!
//! RESP3 Reply
//! Map reply: a list of configuration parameters matching the provided arguments.
//!
//! History
//!     Starting with Redis version 7.0.0: Added the ability to pass multiple pattern parameters in one call
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
//! Redis command: CONFIG SET
//!
//! Syntax
//!
//! CONFIG SET parameter value [parameter value ...]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N) when N is the number of configuration parameters provided
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! The CONFIG SET command is used in order to reconfigure the server at run time without the need to restart Redis. You can change both trivial parameters or switch from one to another persistence option using this command.
//!
//! The list of configuration parameters supported by CONFIG SET can be obtained issuing a CONFIG GET * command, that is the symmetrical command used to obtain information about the configuration of a running Redis instance.
//!
//! All the configuration parameters set using CONFIG SET are immediately loaded by Redis and will take effect starting with the next command executed.
//!
//! All the supported parameters have the same meaning of the equivalent configuration parameter used in the redis.conf file.
//!
//! Note that you should look at the redis.conf file relevant to the version you're working with as configuration options might change between versions. The link above is to the latest development version.
//!
//! It is possible to switch persistence from RDB snapshotting to append-only file (and the other way around) using the CONFIG SET command. See the persistence page for more information.
//!
//! In general what you should know is that setting the appendonly parameter to yes will start a background process to save the initial append-only file (obtained from the in memory data set), and will append all the subsequent commands on the append-only file, thus obtaining exactly the same effect of a Redis server that started with AOF turned on since the start.
//!
//! You can have both the AOF enabled with RDB snapshotting if you want, the two options are not mutually exclusive.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK when the configuration was set properly. Otherwise an error is returned.
//!
//! History
//!     Starting with Redis version 7.0.0: Added the ability to set multiple parameters in one call.
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
//! Redis command: CONFIG REWRITE
//!
//! Syntax
//!
//! CONFIG REWRITE
//!
//! Available since:
//!     2.8.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! The CONFIG REWRITE command rewrites the redis.conf file the server was started with, applying the minimal changes needed to make it reflect the configuration currently used by the server, which may be different compared to the original one because of the use of the CONFIG SET command.
//!
//! The rewrite is performed in a very conservative way:
//!
//!     Comments and the overall structure of the original redis.conf are preserved as much as possible.
//!     If an option already exists in the old redis.conf file, it will be rewritten at the same position (line number).
//!     If an option was not already present, but it is set to its default value, it is not added by the rewrite process.
//!     If an option was not already present, but it is set to a non-default value, it is appended at the end of the file.
//!     Non used lines are blanked. For instance if you used to have multiple save directives, but the current configuration has fewer or none as you disabled RDB persistence, all the lines will be blanked.
//!
//! CONFIG REWRITE is also able to rewrite the configuration file from scratch if the original one no longer exists for some reason. However if the server was started without a configuration file at all, the CONFIG REWRITE will just return an error.
//! Atomic rewrite process
//!
//! In order to make sure the redis.conf file is always consistent, that is, on errors or crashes you always end with the old file, or the new one, the rewrite is performed with a single write(2) call that has enough content to be at least as big as the old file. Sometimes additional padding in the form of comments is added in order to make sure the resulting file is big enough, and later the file gets truncated to remove the padding at the end.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK when the configuration was rewritten properly. Otherwise an error is returned.
//!
bool QtRedisClient::redisConfigReWrite()
{
    return QtRedisReply::replySimpleStringToBool(this->redisExecCommand(QString("CONFIG REWRITE")));
}

//!
//! \brief Сбросить статистику сервера
//! \return
//!
//! Redis command: CONFIG RESETSTAT
//!
//! Syntax
//!
//! CONFIG RESETSTAT
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @admin, @slow, @dangerous
//!
//! Resets the statistics reported by Redis using the INFO and LATENCY HISTOGRAM commands.
//!
//! The following is a non-exhaustive list of values that are reset:
//!
//!     Keyspace hits and misses
//!     Number of expired keys
//!     Command and error statistics
//!     Connections received, rejected and evicted
//!     Persistence statistics
//!     Active defragmentation statistics
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
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
//! Redis command: CLIENT LIST
//!
//! Syntax
//!
//! CLIENT LIST [TYPE <NORMAL | MASTER | REPLICA | PUBSUB>]
//!   [ID client-id [client-id ...]]
//!
//! Available since:
//!     2.4.0
//! Time complexity:
//!     O(N) where N is the number of client connections
//! ACL categories:
//!     @admin, @slow, @dangerous, @connection
//!
//! The CLIENT LIST command returns information and statistics about the client connections server in a mostly human readable format.
//!
//! You can use one of the optional subcommands to filter the list. The TYPE type subcommand filters the list by clients' type, where type is one of normal, master, replica, and pubsub. Note that clients blocked by the MONITOR command belong to the normal class.
//!
//! The ID filter only returns entries for clients with IDs matching the client-id arguments.
//!
//! Here is the meaning of the fields:
//!
//!     id: a unique 64-bit client ID
//!     addr: address/port of the client
//!     laddr: address/port of local address client connected to (bind address)
//!     fd: file descriptor corresponding to the socket
//!     name: the name set by the client with CLIENT SETNAME
//!     age: total duration of the connection in seconds
//!     idle: idle time of the connection in seconds
//!     flags: client flags (see below)
//!     db: current database ID
//!     sub: number of channel subscriptions
//!     psub: number of pattern matching subscriptions
//!     ssub: number of shard channel subscriptions. Added in Redis 7.0.3
//!     multi: number of commands in a MULTI/EXEC context
//!     watch: number of keys this client is currently watching. Added in Redis 7.4
//!     qbuf: query buffer length (0 means no query pending)
//!     qbuf-free: free space of the query buffer (0 means the buffer is full)
//!     argv-mem: incomplete arguments for the next command (already extracted from query buffer)
//!     multi-mem: memory is used up by buffered multi commands. Added in Redis 7.0
//!     obl: output buffer length
//!     oll: output list length (replies are queued in this list when the buffer is full)
//!     omem: output buffer memory usage
//!     tot-mem: total memory consumed by this client in its various buffers
//!     events: file descriptor events (see below)
//!     cmd: last command played
//!     user: the authenticated username of the client
//!     redir: client id of current client tracking redirection
//!     resp: client RESP protocol version. Added in Redis 7.0
//!
//! The client flags can be a combination of:
//!
//! A: connection to be closed ASAP
//! b: the client is waiting in a blocking operation
//! c: connection to be closed after writing entire reply
//! d: a watched keys has been modified - EXEC will fail
//! e: the client is excluded from the client eviction mechanism
//! i: the client is waiting for a VM I/O (deprecated)
//! M: the client is a master
//! N: no specific flag set
//! O: the client is a client in MONITOR mode
//! P: the client is a Pub/Sub subscriber
//! r: the client is in readonly mode against a cluster node
//! S: the client is a replica node connection to this instance
//! u: the client is unblocked
//! U: the client is connected via a Unix domain socket
//! x: the client is in a MULTI/EXEC context
//! t: the client enabled keys tracking in order to perform client side caching
//! T: the client will not touch the LRU/LFU of the keys it accesses
//! R: the client tracking target client is invalid
//! B: the client enabled broadcast tracking mode
//!
//! The file descriptor events can be:
//!
//! r: the client socket is readable (event loop)
//! w: the client socket is writable (event loop)
//!
//! Notes
//!
//! New fields are regularly added for debugging purpose. Some could be removed in the future. A version safe Redis client using this command should parse the output accordingly (i.e. handling gracefully missing fields, skipping unknown fields).
//!
//! RESP2/RESP3 Reply
//! Bulk string reply: information and statistics about client connections.
//!
//! History
//!
//!     Starting with Redis version 2.8.12: Added unique client id field.
//!     Starting with Redis version 5.0.0: Added optional TYPE filter.
//!     Starting with Redis version 6.0.0: Added user field.
//!     Starting with Redis version 6.2.0: Added argv-mem, tot-mem, laddr and redir fields and the optional ID filter.
//!     Starting with Redis version 7.0.0: Added resp, multi-mem, rbs and rbp fields.
//!     Starting with Redis version 7.0.3: Added ssub field.
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

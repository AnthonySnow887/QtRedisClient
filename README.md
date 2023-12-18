# QtRedisClient

[![License](https://img.shields.io/github/license/AnthonySnow887/QtRedisClient)](https://github.com/AnthonySnow887/FlyCubeMigration/blob/master/LICENSE)
[![Latest Release](https://img.shields.io/github/v/release/AnthonySnow887/QtRedisClient?label=release)](https://github.com/AnthonySnow887/QtRedisClient/releases)
![Last Commit](https://img.shields.io/github/last-commit/AnthonySnow887/QtRedisClient/develop)

NoSQL Redis client library based on Qt 5.

## Usage

### If you use qmake

To use the library, just copy it to the root of your project and include the "QtRedisClient/QtRedisClient.pri" file:

```cpp
include(QtRedisClient/QtRedisClient.pri)
```

> 
> Example for qmake:
> ```cpp
> include(QtRedisClient/QtRedisClient.pri)
> 
> QT += core
> QT -= gui
> 
> CONFIG += c++14
> QMAKE_CXXFLAGS += -std=c++14
> 
> TARGET = testRedis
> CONFIG += console
> CONFIG += warn_on
> CONFIG -= app_bundle
> 
> TEMPLATE = app
> 
> SOURCES += main.cpp \
>       Tester.cpp
> 
> # The following define makes your compiler emit warnings if you use
> # any feature of Qt which as been marked deprecated (the exact warnings
> # depend on your compiler). Please consult the documentation of the
> # deprecated API in order to know how to port your code away from it.
> DEFINES += QT_DEPRECATED_WARNINGS
> 
> HEADERS += \
>       Tester.h
> 
> ```
> 

### If you use CMake

To use the library, just copy it to the root of your project and include the "QtRedisClient/CMakeLists.txt" file:

```cmake
add_subdirectory(QtRedisClient)

...

target_link_libraries(<target>
    QtRedisClient
    ...
    <item>
    ...)
```

> 
> Example for CMake:
> ```cmake
> cmake_minimum_required(VERSION 3.14)
> 
> project(testRedis LANGUAGES CXX)
> 
> set(CMAKE_INCLUDE_CURRENT_DIR ON)
> set(CMAKE_AUTOUIC ON)
> set(CMAKE_AUTOMOC ON)
> set(CMAKE_AUTORCC ON)
> set(CMAKE_CXX_STANDARD 14)
> set(CMAKE_CXX_STANDARD_REQUIRED ON)
> 
> find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
> find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
> 
> add_subdirectory(QtRedisClient)
> 
> add_executable(testRedis
>   Tester.h
>   Tester.cpp
>   main.cpp)
> 
> target_link_libraries(testRedis
>     QtRedisClient
>     Qt${QT_VERSION_MAJOR}::Core)
> ```
> 

## Supported Redis commands

### Library error functions
```cpp
bool hasLastError() const;
QString lastError() const;
```

### Connection functions

>
> NOTE: 
> 
> Function ```redisConnectUnix(...)``` is only supported on Linux!
>

```cpp
QtRedisTransporter::Type redisContextType();
QtRedisTransporter::ChannelMode redisContextChannelMode();

bool redisIsConnected();

bool redisConnect(const QString &host,
                  const int port = 6379,
                  const int timeOutMsec = -1,
                  const QtRedisTransporter::ChannelMode contextChannelMode = QtRedisTransporter::ChannelMode::CurrentConnection);

// NOTE: not tested!
bool redisConnectEncrypted(const QString &host,
                           const int port,
                           const int timeOutMsec = -1,
                           const QtRedisTransporter::ChannelMode contextChannelMode = QtRedisTransporter::ChannelMode::CurrentConnection);

#if defined(Q_OS_LINUX)
bool redisConnectUnix(const QString &sockPath = QString("/tmp/redis.sock"),
                      const int timeOutMsec = -1,
                      const QtRedisTransporter::ChannelMode contextChannelMode = QtRedisTransporter::ChannelMode::CurrentConnection);
#endif

bool redisReconnect(const int timeOutMsec = -1);

void redisDisconnect();

//
// Qt Signals:
//
void contextConnected(QString contextUid, QString host, int port, int dbIndex);
void contextDisconnected(QString contextUid, QString host, int port, int dbIndex);
```

### Base commands
```cpp
QtRedisReply redisExecCommand(const QString &command);
QtRedisReply redisExecCommand(const QByteArray &command);
QtRedisReply redisExecCommandArgv(const QStringList &commandArgv);
QtRedisReply redisExecCommandArgv(const QList<QByteArray> &commandArgv);

QList<QtRedisReply> redisExecCommand_lst(const QString &command);
QList<QtRedisReply> redisExecCommand_lst(const QByteArray &command);
QList<QtRedisReply> redisExecCommandArgv_lst(const QStringList &commandArgv);
QList<QtRedisReply> redisExecCommandArgv_lst(const QList<QByteArray> &commandArgv);
```

### Server commands
```cpp
bool redisAuth(const QString &password);
bool redisPing(const QString &msg = QString());
QtRedisReply redisEcho(const QString &msg);
QMap<QString, QVariant> redisInfo(const QString &section = QString());
QtRedisReply redisTime();
bool redisSelect(const int dbIndex);
int redisSelectedDb();
qlonglong redisDbSize();
bool redisFlushAll(const bool async = false);
bool redisFlushDb(const bool async = false);

// -- SAVE-methods --
bool redisSave();
QtRedisReply redisBgSave();
uint redisLastSave();

// -- CONFIG-methods --
QtRedisReply redisConfigGet(const QString &param);
bool redisConfigSet(const QString &param, const QString &value);
bool redisConfigReWrite();
bool redisConfigResetStat();

// -- CLIENT-methods --
QVector<QtRedisClientInfo> redisClientList();
bool redisClientSetName(const QString &connectionName);
QString redisClientGetName();
bool redisClientKill(const QString &ip, const uint port);
bool redisClientKill(const QString &id);
```

### Key-Value commands
```cpp
QStringList redisKeys(const QString &arg = QString("*"));
QString redisRandomKey();
qlonglong redisExists(const QStringList &keyList);
QtRedisReply redisGet(const QString &key);
QString redisGetRange(const QString &key, const int startPos, const int endPos);
QtRedisReply redisGetSet(const QString &key, const QString &value);
qlonglong redisAppend(const QString &key, const QString &appendValue);
bool redisSet(const QString &key,
                const QString &value,
                const uint exSec = 0,
                const uint pxMSec = 0,
                const QString existFlag = QString());

qlonglong redisSetRange(const QString &key, const QString &value, const int offset);
qlonglong redisDel(const QStringList &keyList);
qlonglong redisStrlen(const QString &key);
bool redisExpire(const QString &key, const uint sec);
bool redisExpireAt(const QString &key, const uint utcSec);
bool redisPExpire(const QString &key, const uint msec);
bool redisPExpireAt(const QString &key, const qint64 utcMsec);
bool redisPersist(const QString &key);
qlonglong redisTtl(const QString &key);
qlonglong redisPTtl(const QString &key);
qlonglong redisDecr(const QString &key);
qlonglong redisDecrBy(const QString &key, const qint64 decr);
qlonglong redisIncr(const QString &key);
qlonglong redisIncrBy(const QString &key, const qint64 incr);
qlonglong redisIncrByFloat(const QString &key, const float incr);
bool redisRename(const QString &key, const QString &newKey);
bool redisRenameNx(const QString &key, const QString &newKey);
QString redisType(const QString &key);
bool redisMSet(const QMap<QString, QString> &keyValue);
bool redisMSetNx(const QMap<QString, QString> &keyValue);
QtRedisReply redisMGet(const QStringList &keyList);
bool redisMove(const QString &key, const int dbIndex);
QtRedisReply redisDump(const QString &key);
```

### List commands
```cpp
QString redisLIndex(const QString &key, const int index);
qlonglong redisLInsert(const QString &key,
                        const QString &pilot,
                        const QString &value,
                        const QString &insertFlag = QString("AFTER"));

qlonglong redisLLen(const QString &key);
QString redisLPop(const QString &key);
qlonglong redisLPush(const QString &key, const QStringList &valueList);
qlonglong redisLPushX(const QString &key, const QString &value);
QtRedisReply redisLRange(const QString &key, const int start, const int stop);
qlonglong redisLRem(const QString &key, const QString &value, const int count);
bool redisLSet(const QString &key, const QString &value, const int index);
bool redisLTrim(const QString &key, const int start, const int stop);
QString redisRPop(const QString &key);
QString redisRPopLPush(const QString &sourceKey, const QString &destKey);
qlonglong redisRPush(const QString &key, const QStringList &valueList);
qlonglong redisRPushX(const QString &key, const QString &value);
```

### Stored commands
```cpp
qlonglong redisSAdd(const QString &key, const QStringList &memberList);
qlonglong redisSCard(const QString &key);
QtRedisReply redisSDiff(const QStringList &keyList);
qlonglong redisSDiffStore(const QString &dest, const QStringList &keyList);
QtRedisReply redisSInter(const QStringList &keyList);
qlonglong redisSInterStore(const QString &dest, const QStringList &keyList);
bool redisSIsMember(const QString &key, const QString &member);
QStringList redisSMembers(const QString &key);
bool redisSMove(const QString &sourceKey, const QString &destKey, const QString &member);
QtRedisReply redisSPop(const QString &key, const uint count = 1);
QtRedisReply redisSRandMember(const QString &key, const int count = 1);
qlonglong redisSRem(const QString &key, const QStringList &memberList);
QtRedisReply redisSUnion(const QStringList &keyList);
qlonglong redisSUnionStore(const QString &dest, const QStringList &keyList);
```

### Sorted stored commands
```cpp
QtRedisReply redisZAdd(const QString &key,
                        const QMultiMap<QString, QString> scoreMember,
                        const QString &updFlag = QString(),
                        const bool chFlag = false,
                        const bool incrFlag = false);

qlonglong redisZCard(const QString &key);
qlonglong redisZCount(const QString &key,
                        const QVariant &min = QVariant(),
                        const QVariant &max = QVariant());

QtRedisReply redisZIncrBy(const QString &key, const QString &member, const qint64 incr);

qlonglong redisZInterStore(const QString &destKey,
                            const QStringList &keyList,
                            const QList<int> &weightList = QList<int>(),
                            const QString &aggregateFlag = QString());

qlonglong redisZLexCount(const QString &key, const QString &min, const QString &max);

QtRedisReply redisZRange(const QString &key,
                            const int start,
                            const int stop,
                            const bool withScores = false);

QtRedisReply redisZRangeByLex(const QString &key,
                                const QString &min,
                                const QString &max,
                                const int offset = -1,
                                const int count = -1);

QtRedisReply redisZRangeByScore(const QString &key,
                                const QString &min,
                                const QString &max,
                                const bool withScores = false,
                                const int offset = -1,
                                const int count = -1);

qlonglong redisZRank(const QString &key, const QString &member);
qlonglong redisZRem(const QString &key, const QStringList &members);
qlonglong redisZRemRangeByLex(const QString &key, const QString &min, const QString &max);
qlonglong redisZRemRangeByRank(const QString &key, const int start, const int stop);
qlonglong redisZRemRangeByScore(const QString &key, const QString &min, const QString &max);

QtRedisReply redisZRevRange(const QString &key,
                            const int start,
                            const int stop,
                            const bool withScores = false);

QtRedisReply redisZRevRangeByLex(const QString &key,
                                    const QString &max,
                                    const QString &min,
                                    const int offset = -1,
                                    const int count = -1);

QtRedisReply redisZRevRangeByScore(const QString &key,
                                    const QString &max,
                                    const QString &min,
                                    const bool withScores = false,
                                    const int offset = -1,
                                    const int count = -1);

qlonglong redisZRevRank(const QString &key, const QString &member);
QtRedisReply redisZScore(const QString &key, const QString &member);

qlonglong redisZUnionStore(const QString &destKey,
                            const QStringList &keyList,
                            const QList<int> &weightList = QList<int>(),
                            const QString &aggregateFlag = QString());
```

### Pub/Sub commands
```cpp
QStringList redisPubSubChannels(const QString &pattern = QString());
qlonglong redisPubSubNumPat();
QMap<QString, qlonglong> redisPubSubNumSub(const QString &channel = QString());
QMap<QString, qlonglong> redisPubSubNumSub(const QStringList &channels);
QStringList redisPubSubShardChannels(const QString &pattern = QString());
QMap<QString, qlonglong> redisPubSubShardNumSub(const QString &shardChannel = QString());
QMap<QString, qlonglong> redisPubSubShardNumSub(const QStringList &shardChannels);

qlonglong redisPublish(const QString &channel, const QString &message);
qlonglong redisPublish(const QString &channel, const QByteArray &message);
qlonglong redisSPublish(const QString &shardChannel, const QString &message);
qlonglong redisSPublish(const QString &shardChannel, const QByteArray &message);

bool redisSubscribe(const QString &channel);
bool redisSubscribe(const QStringList &channels);
bool redisUnsubscribe(const QString &channel = QString());
bool redisUnsubscribe(const QStringList &channels);

bool redisPSubscribe(const QString &pattern);
bool redisPSubscribe(const QStringList &patterns);
bool redisPUnsubscribe(const QString &pattern = QString());
bool redisPUnsubscribe(const QStringList &patterns);

bool redisSSubscribe(const QString &shardChannel);
bool redisSSubscribe(const QStringList &shardChannels);
bool redisSUnsubscribe(const QString &shardChannel = QString());
bool redisSUnsubscribe(const QStringList &shardChannels);

//
// Qt Signals:
//
void incomingChannelMessage(QString channel, QtRedisReply data);
void incomingChannelShardMessage(QString shardChannel, QtRedisReply data);
void incomingChannelPatternMessage(QString pattern, QString channel, QtRedisReply data);
```

## Code examples

>
> NOTE:
>
> See file "QtRedisClient.h" for a list of available commands.
>

### Base Redis client example
```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient _redisClient;

// Connect Qt signal for check context connected
connect(&_redisClient, &QtRedisClient::contextConnected, this, [](QString contextUid, QString host, int port, int dbIndex) {
    qDebug() << "Redis client CONNECTED:" << contextUid << host << port << dbIndex;
});

// Connect Qt signal for check context disconnected
connect(&_redisClient, &QtRedisClient::contextDisconnected, this, [](QString contextUid, QString host, int port, int dbIndex) {
    qDebug() << "Redis client DISCONNECTED:" << contextUid << host << port << dbIndex;
});

// Connect to Redis...
if (!_redisClient.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed!";
    return;
}
qDebug() << "Connect to Redis success";

// Send Redis ping command
if (!_redisClient.redisPing()) {
    qCritical() << qPrintable(QString("Redis PING failed! Error: %1")
                              .arg(_redisClient.lastError()));
    return;
}

// Select Redis version
qDebug() << "Redis version:" << _redisClient.redisInfo("server").value("redis_version", "?.?.?");

// Select Redis keys
QStringList redisKeysList = _redisClient.redisKeys();
qDebug() << "Redis keys:" << redisKeysList;

// Remove all Redis keys (ver. 1)
if (!_redisClient.redisDel(redisKeysList)) {
    qWarning() << qPrintable(QString("Del redis keys failed! Error: %1")
                             .arg(_redisClient.lastError()));
} else {
    qDebug() << qPrintable(QString("Del redis keys success"));
}

// Remove all Redis keys (ver. 2)
for (const QString &key : qAsConst(redisKeysList)) {
    if (!_redisClient.redisDel(QStringList(key))) {
        qWarning() << qPrintable(QString("Del redis key (%1) failed! Error: %2")
                                 .arg(key)
                                 .arg(_redisClient.lastError()));
    } else {
        qDebug() << qPrintable(QString("Del redis key (%1) success")
                               .arg(key));
    }
}

// Set Redis value
QString rKey("my_redis_key");
QString rValue("my_redis_value");
if (!_redisClient.redisSet(rKey, rValue)) {
    qWarning() << qPrintable(QString("Add redis key-value failed (key: %1)! Error: %2")
                             .arg(rKey)
                             .arg(_redisClient.lastError()));
}

// Disconnect
_redisClient.redisDisconnect();
```

### Base Redis pub/sub client examples

>
> NOTE: 
>
> If you receive an error like: 
> 
> ```ERR Can't execute 'publish': only (P)SUBSCRIBE / (P)UNSUBSCRIBE / PING / QUIT / RESET are allowed in this context-ERR unknown command `023`, with args beginning with: \r\n```
> 
> then this indicates that you are trying to both publish and read messages from the same client!
> Just create a new connection with a new client.
>


>
> NOTE:
>
> To use a separate connection for channels use the following code:
>
> ```cpp
> // Create client with separate connection & connect
> QtRedisClient _redisClient;
> if (!_redisClient.redisConnect("127.0.0.1", 6379, -1, QtRedisTransporter::ChannelMode::SeparateConnection)) {
>     qCritical() << "Connect to redis failed!";
>     return;
> }
> ``` 
>

#### Redis client with base pub/sub channels (uses separate connections)

>
> NOTE:
>
> This approach uses separate connections within the QtRedisClient object to send data and receive incoming messages from channels, so it is enough to create a one class object.
>

```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient _redisClient;
if (!_redisClient.redisConnect("127.0.0.1", 6379, -1, QtRedisTransporter::ChannelMode::SeparateConnection)) {
    qCritical() << "Connect to Redis failed!";
    return;
}
qDebug() << "Connect to Redis success";

// Subscribe to the channel
if (!_redisClient.redisSubscribe("my_channel"))
    qCritical() << qPrintable(QString("Subscribe to the channel failed! Error: %1")
                              .arg(_redisClient.lastError()));
    return;
}

// Connect Qt signal for incoming channel messages
connect(&_redisClient, &QtRedisClient::incomingChannelMessage, this, [](QString channel, QtRedisReply data) {
    qDebug() << "Incoming SUB:" << channel << data;
});

//
// ...
//

// Publishing a message to a channel
qDebug() << "pub:" << _redisClient.redisPublish("my_channel", QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

//
// ...
//

// Disconnect
_redisClient.redisDisconnect();
```

#### Redis client with base pub/sub channels

>
> NOTE:
>
> This approach uses the same connection inside a QtRedisClient object to send data and receive incoming messages from channels, so it requires creating two class objects:
> - to send data
> - to subscribe to the channel and receive incoming data.
>

```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient _redisPub;
if (!_redisPub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-pub)!";
    return;
}
QtRedisClient _redisSub;
if (!_redisSub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-sub)!";
    return;
}
qDebug() << "Connect to Redis success";

// Subscribe to the channel
if (!_redisSub.redisSubscribe("my_channel"))
    qCritical() << qPrintable(QString("Subscribe to the channel failed! Error: %1")
                              .arg(_redisSub.lastError()));
    return;
}

// Connect Qt signal for incoming channel messages
connect(&_redisSub, &QtRedisClient::incomingChannelMessage, this, [](QString channel, QtRedisReply data) {
    qDebug() << "Incoming SUB:" << channel << data;
});

//
// ...
//

// Publishing a message to a channel
qDebug() << "pub:" << _redisPub.redisPublish("my_channel", QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

//
// ...
//

// Disconnect
_redisPub.redisDisconnect();
_redisSub.redisDisconnect();
```

#### Redis client with pattern pub/sub channel

>
> NOTE:
>
> This approach uses the same connection inside a QtRedisClient object to send data and receive incoming messages from channels, so it requires creating two class objects:
> - to send data
> - to subscribe to the channel and receive incoming data.
>

```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient _redisPub;
if (!_redisPub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-pub)!";
    return;
}
QtRedisClient _redisSub;
if (!_redisSub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-sub)!";
    return;
}
qDebug() << "Connect to Redis success";

// Subscribe to the channel
if (!_redisSub.redisPSubscribe("my_channe*"))
    qCritical() << qPrintable(QString("Subscribe to the channel failed! Error: %1")
                              .arg(_redisSub.lastError()));
    return;
}

// Connect Qt signal for incoming pattern channel messages
connect(&_redisSub, &QtRedisClient::incomingChannelPatternMessage, this, [](QString pattern, QString channel, QtRedisReply data) {
    qDebug() << "Incoming PSUB:" << pattern << channel << data;
});

//
// ...
//

// Publishing a message to a channel
qDebug() << "pub:" << _redisPub.redisPublish("my_channel", QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

//
// ...
//

// Disconnect
_redisPub.redisDisconnect();
_redisSub.redisDisconnect();
```

#### Redis client with shard pub/sub channel

>
> NOTE: 
> 
> This functionality has appeared in Redis since version 7.0.0!
>

>
> NOTE:
>
> This approach uses the same connection inside a QtRedisClient object to send data and receive incoming messages from channels, so it requires creating two class objects:
> - to send data
> - to subscribe to the channel and receive incoming data.
>

```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient _redisPub;
if (!_redisPub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-pub)!";
    return;
}
QtRedisClient _redisSub;
if (!_redisSub.redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to Redis failed (redis-sub)!";
    return;
}
qDebug() << "Connect to Redis success";

// Subscribe to the channel
if (!_redisSub.redisSSubscribe("my_shard_channel"))
    qCritical() << qPrintable(QString("Subscribe to the shard channel failed! Error: %1")
                              .arg(_redisSub.lastError()));
    return;
}

// Connect Qt signal for incoming shard channel messages
connect(&_redisSub, &QtRedisClient::incomingChannelShardMessage, this, [](QString shardChannel, QtRedisReply data) {
    qDebug() << "Incoming SSUB:" << shardChannel << data;
});

//
// ...
//

// Publishing a message to a shard channel
qDebug() << "spub:" << _redisPub.redisSPublish("my_shard_channel", QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));

//
// ...
//

// Disconnect
_redisPub.redisDisconnect();
_redisSub.redisDisconnect();
```

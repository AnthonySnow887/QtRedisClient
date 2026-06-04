# QtRedisClient

[![License](https://img.shields.io/github/license/AnthonySnow887/QtRedisClient)](https://github.com/AnthonySnow887/FlyCubeMigration/blob/master/LICENSE)
[![Latest Release](https://img.shields.io/github/v/release/AnthonySnow887/QtRedisClient?label=release)](https://github.com/AnthonySnow887/QtRedisClient/releases)
![Last Commit](https://img.shields.io/github/last-commit/AnthonySnow887/QtRedisClient/develop)

NoSQL Redis client library based on Qt 5 / Qt 6.

>
> NOTE: 
>
> The current version of the library only supports Redis RESP2 (Redis Serialization Protocol version 2)!
>

>
> IMPORTANT: 
>
> Backward compatibility with older versions of the library is not supported, since most of the methods now return an `__RESULT_IMPL` object!
>

## Usage

Minimum required version of the C++ standard: C++14.

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

All available commands are described in the files:
- Core/QtRedisBase.h
- Core/QtRedisPipeline.h
- Core/QtRedisTransaction.h
- QtRedisClient.h


### Library error functions
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

bool hasLastError() const;
const QString &lastError() const;
```

### Connection functions

>
> NOTE: 
> 
> Function ```redisConnectUnix(...)``` is only supported on Linux!
>

```cpp
//
// For details see the file: QtRedisClient.h
//

QtRedisTransporter::Type redisContextType();
QtRedisTransporter::ChannelMode redisContextChannelMode();

bool redisIsConnected();

bool redisConnect(const QString &host,
                  const int port = 6379,
                  const int timeOutMsec = -1,
                  const QtRedisTransporter::ChannelMode contextChannelMode = QtRedisTransporter::ChannelMode::CurrentConnection);

bool redisConnectEncrypted(const QString &host,
                           const int port = 6379,
                           const QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration(),
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

### Server commands
```cpp
//
// For details see the file: QtRedisClient.h
//

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
QList<QtRedisClientInfo> redisClientList();
bool redisClientSetName(const QString &connectionName);
QString redisClientGetName();
bool redisClientKill(const QString &ip, const uint port);
bool redisClientKill(const QString &id);
```

### Base commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

__RESULT_IMPL redisExecCommand(const QtRedisCommand &command);
__RESULT_IMPL redisExecCommand(const QString &command);
__RESULT_IMPL redisExecCommand(const QByteArray &command);
__RESULT_IMPL redisExecCommand(const QStringList &commandArgv);
__RESULT_IMPL redisExecCommand(const QList<QByteArray> &commandArgv);

//
// Where __RESULT_IMPL is:
// - QtRedisReply   - for QtRedisClient class
// - bool           - for QtRedisPipeline and QtRedisTransaction classes
//
```

### Key-Value commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

__RESULT_IMPL redisKeys(const QString &arg = QString("*"));
__RESULT_IMPL redisRandomKey();
__RESULT_IMPL redisExists(const QStringList &keyList);
__RESULT_IMPL redisGet(const QString &key);
__RESULT_IMPL redisGetRange(const QString &key, const int startPos, const int endPos);
__RESULT_IMPL redisGetSet(const QString &key, const QString &value);
__RESULT_IMPL redisAppend(const QString &key, const QString &appendValue);
__RESULT_IMPL redisSet(const QString &key,
                       const QString &value,
                       const uint exSec = 0,
                       const uint pxMSec = 0,
                       const QString existFlag = QString());

__RESULT_IMPL redisSetRange(const QString &key, const QString &value, const int offset);
__RESULT_IMPL redisDel(const QStringList &keyList);
__RESULT_IMPL redisStrlen(const QString &key);
__RESULT_IMPL redisExpire(const QString &key, const uint sec);
__RESULT_IMPL redisExpireAt(const QString &key, const uint utcSec);
__RESULT_IMPL redisPExpire(const QString &key, const uint msec);
__RESULT_IMPL redisPExpireAt(const QString &key, const qint64 utcMsec);
__RESULT_IMPL redisPersist(const QString &key);
__RESULT_IMPL redisTtl(const QString &key);
__RESULT_IMPL redisPTtl(const QString &key);
__RESULT_IMPL redisDecr(const QString &key);
__RESULT_IMPL redisDecrBy(const QString &key, const qint64 decr);
__RESULT_IMPL redisIncr(const QString &key);
__RESULT_IMPL redisIncrBy(const QString &key, const qint64 incr);
__RESULT_IMPL redisIncrByFloat(const QString &key, const float incr);
__RESULT_IMPL redisRename(const QString &key, const QString &newKey);
__RESULT_IMPL redisRenameNx(const QString &key, const QString &newKey);
__RESULT_IMPL redisType(const QString &key);
__RESULT_IMPL redisMSet(const QMap<QString, QString> &keyValue);
__RESULT_IMPL redisMSetNx(const QMap<QString, QString> &keyValue);
__RESULT_IMPL redisMGet(const QStringList &keyList);
__RESULT_IMPL redisMove(const QString &key, const int dbIndex);
__RESULT_IMPL redisDump(const QString &key);

//
// Where __RESULT_IMPL is:
// - QtRedisReply   - for QtRedisClient class
// - bool           - for QtRedisPipeline and QtRedisTransaction classes
//
```

### List commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

__RESULT_IMPL redisLIndex(const QString &key, const int index);
__RESULT_IMPL redisLInsert(const QString &key,
                           const QString &pilot,
                           const QString &value,
                           const QString &insertFlag = QString("AFTER"));

__RESULT_IMPL redisLLen(const QString &key);
__RESULT_IMPL redisLPop(const QString &key);
__RESULT_IMPL redisLPush(const QString &key, const QStringList &valueList);
__RESULT_IMPL redisLPushX(const QString &key, const QString &value);
__RESULT_IMPL redisLRange(const QString &key, const int start, const int stop);
__RESULT_IMPL redisLRem(const QString &key, const QString &value, const int count);
__RESULT_IMPL redisLSet(const QString &key, const QString &value, const int index);
__RESULT_IMPL redisLTrim(const QString &key, const int start, const int stop);
__RESULT_IMPL redisRPop(const QString &key);
__RESULT_IMPL redisRPopLPush(const QString &sourceKey, const QString &destKey);
__RESULT_IMPL redisRPush(const QString &key, const QStringList &valueList);
__RESULT_IMPL redisRPushX(const QString &key, const QString &value);

//
// Where __RESULT_IMPL is:
// - QtRedisReply   - for QtRedisClient class
// - bool           - for QtRedisPipeline and QtRedisTransaction classes
//
```

### Stored commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

__RESULT_IMPL redisSAdd(const QString &key, const QStringList &memberList);
__RESULT_IMPL redisSCard(const QString &key);
__RESULT_IMPL redisSDiff(const QStringList &keyList);
__RESULT_IMPL redisSDiffStore(const QString &dest, const QStringList &keyList);
__RESULT_IMPL redisSInter(const QStringList &keyList);
__RESULT_IMPL redisSInterStore(const QString &dest, const QStringList &keyList);
__RESULT_IMPL redisSIsMember(const QString &key, const QString &member);
__RESULT_IMPL redisSMembers(const QString &key);
__RESULT_IMPL redisSMove(const QString &sourceKey, const QString &destKey, const QString &member);
__RESULT_IMPL redisSPop(const QString &key, const uint count = 1);
__RESULT_IMPL redisSRandMember(const QString &key, const int count = 1);
__RESULT_IMPL redisSRem(const QString &key, const QStringList &memberList);
__RESULT_IMPL redisSUnion(const QStringList &keyList);
__RESULT_IMPL redisSUnionStore(const QString &dest, const QStringList &keyList);

//
// Where __RESULT_IMPL is:
// - QtRedisReply   - for QtRedisClient class
// - bool           - for QtRedisPipeline and QtRedisTransaction classes
//
```

### Sorted stored commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

__RESULT_IMPL redisZAdd(const QString &key,
                        const QMultiMap<QString, QString> scoreMember,
                        const QString &updFlag = QString(),
                        const bool chFlag = false,
                        const bool incrFlag = false);

__RESULT_IMPL redisZCard(const QString &key);
__RESULT_IMPL redisZCount(const QString &key,
                          const QVariant &min = QVariant(),
                          const QVariant &max = QVariant());

__RESULT_IMPL redisZIncrBy(const QString &key, const QString &member, const qint64 incr);

__RESULT_IMPL redisZInterStore(const QString &destKey,
                               const QStringList &keyList,
                               const QList<int> &weightList = QList<int>(),
                               const QString &aggregateFlag = QString());

__RESULT_IMPL redisZLexCount(const QString &key, const QString &min, const QString &max);

__RESULT_IMPL redisZRange(const QString &key,
                          const int start,
                          const int stop,
                          const bool withScores = false);

__RESULT_IMPL redisZRangeByLex(const QString &key,
                               const QString &min,
                               const QString &max,
                               const int offset = -1,
                               const int count = -1);

__RESULT_IMPL redisZRangeByScore(const QString &key,
                                 const QString &min,
                                 const QString &max,
                                 const bool withScores = false,
                                 const int offset = -1,
                                 const int count = -1);

__RESULT_IMPL redisZRank(const QString &key, const QString &member);
__RESULT_IMPL redisZRem(const QString &key, const QStringList &members);
__RESULT_IMPL redisZRemRangeByLex(const QString &key, const QString &min, const QString &max);
__RESULT_IMPL redisZRemRangeByRank(const QString &key, const int start, const int stop);
__RESULT_IMPL redisZRemRangeByScore(const QString &key, const QString &min, const QString &max);

__RESULT_IMPL redisZRevRange(const QString &key,
                             const int start,
                             const int stop,
                             const bool withScores = false);

__RESULT_IMPL redisZRevRangeByLex(const QString &key,
                                  const QString &max,
                                  const QString &min,
                                  const int offset = -1,
                                  const int count = -1);

__RESULT_IMPL redisZRevRangeByScore(const QString &key,
                                    const QString &max,
                                    const QString &min,
                                    const bool withScores = false,
                                    const int offset = -1,
                                    const int count = -1);

__RESULT_IMPL redisZRevRank(const QString &key, const QString &member);
__RESULT_IMPL redisZScore(const QString &key, const QString &member);

__RESULT_IMPL redisZUnionStore(const QString &destKey,
                               const QStringList &keyList,
                               const QList<int> &weightList = QList<int>(),
                               const QString &aggregateFlag = QString());

//
// Where __RESULT_IMPL is:
// - QtRedisReply   - for QtRedisClient class
// - bool           - for QtRedisPipeline and QtRedisTransaction classes
//
```

### Pipeline and Transaction create commands
```cpp
//
// For details see the file: Core/QtRedisBase.h
//

QtRedisPipeline createPipeline();
QtRedisTransaction createTransaction(const bool piped = true);
```

### Pipeline commands

Class `QtRedisPipeline` describes how to work with the Redis server in Redis Pipeline mode.

```cpp
//
// For details see the file: Core/QtRedisPipeline.h
//

//
// Includes all commands from sections:
// - Library error functions
// - Key-Value commands
// - List commands
// - Stored commands
// - Sorted stored commands
//
// For all the above sections __RESULT_IMPL is bool.
//

QtRedisReply exec();
bool execToBool();

void discard();
```

### Transaction commands

Class `QtRedisTransaction` describes how to work with the Redis server in Redis Transaction mode.

```cpp
//
// For details see the file: Core/QtRedisTransaction.h
//

//
// Includes all commands from sections:
// - Library error functions
// - Key-Value commands
// - List commands
// - Stored commands
// - Sorted stored commands
//
// For all the above sections __RESULT_IMPL is bool.
//

bool watch(const QString &key);
bool watch(const QStringList &keys);
bool unwatch();

QtRedisReply exec();
bool execToBool();

void discard();
```

### Pub/Sub commands
```cpp
//
// For details see the file: QtRedisClient.h
//

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

### QtRedisCommand

Class `QtRedisCommand` describes a command for the Redis server.

```cpp
//
// For details see the file: Core/QtRedisCommand.h
//

explicit QtRedisCommand(const QByteArray &command);
explicit QtRedisCommand(const QByteArray &command, const QList<QByteArray> &commandArgv);

const QByteArray &command() const;
const QList<QByteArray> &commandArgv() const;

//! Check command is not empty.
bool isValid() const;

//! Get command data size (COMMAND + COMMAND_ARGS)
int size() const;


// ------------------------------------------------------------------------
// -- TOOLS COMMANDS ------------------------------------------------------
// ------------------------------------------------------------------------

//! Create a command object from a QString.
//! Note: The string is split into an array using a space separator.
//! Note: The first element of the array is taken as the command name, the rest as arguments.
static QtRedisCommand fromString(const QString &command);

//! Create a command object from a QStringList.
//! Note: The first element of the array is taken as the command name, the rest as arguments.
static QtRedisCommand fromStringList(const QStringList &commandArgv);

//! Create a command object from a QByteArray.
//! Note: The byte-array string is split into an array using a space separator.
//! Note: The first element of the array is taken as the command name, the rest as arguments.
static QtRedisCommand fromByteArray(const QByteArray &command);

//! Create a command object from a QByteArray.
//! Note: The first element of the array is taken as the command name, the rest as arguments.
static QtRedisCommand fromByteArrayList(const QList<QByteArray> &commandArgv);
```

### QtRedisReply

Class `QtRedisReply` describes the response from the Redis server.

```cpp
//
// For details see the file: Core/QtRedisReply.h
//

enum class QtRedisReply::ReplyType {
    String = 0,   //!< String object -> use QtRedisReply.strValue()
    Array,        //!< Array object -> use QtRedisReply.arrayValue()
    Integer,      //!< Integer object -> use QtRedisReply.intValue()
    Nil,          //!< Nil object -> nothing
    Status,       //!< Status object -> use QtRedisReply.strValue()
    Error         //!< Error object -> use QtRedisReply.strValue()
};

//! Checks that _type == ReplyType::String || _type == ReplyType::Array || _type == ReplyType::Integer
bool isValue() const;

bool isString() const;
bool isArray() const;
bool isInteger() const;
bool isNil() const;
bool isStatus() const;
bool isError() const;

//! Checks that _type != ReplyType::Nil && _rawValue.isEmpty()
bool isEmpty() const;

ReplyType type() const;

//! Note: Return copy of value!
QByteArray rawValue() const;

//! Note: Return reference value!
const QByteArray &rawValue_ref() const;

QString strValue() const;
qlonglong intValue() const;

int arrayValueSize() const;
bool isArrayValueEmpty() const;

//! Note: Return copy of value!
QVector<QtRedisReply> arrayValue() const;

//! Note: Not check type! Return reference value!
const QVector<QtRedisReply> &arrayValue_ref() const;

//! Note: Return copy of value!
QtRedisReply arrayValueAt(const int index) const;

//! Note: Not check type! Return reference value!
//! Warn: index must be a valid index position in the _arrayValue vector (i.e., 0 <= index < size())!
const QtRedisReply &arrayValueAt_ref(const int index) const;

//! Note: Return copy of value!
QtRedisReply arrayValueFirst() const;

//! Note: Not check type! Return reference value!
//! Warn: This function assumes that the _arrayValue vector isn't empty!
const QtRedisReply &arrayValueFirst_ref() const;

//! Note: Return copy of value!
QtRedisReply arrayValueLast() const;

//! Note: Not check type! Return reference value!
//! Warn: This function assumes that the _arrayValue vector isn't empty!
const QtRedisReply &arrayValueLast_ref() const;

friend QDebug operator<<(QDebug dbg, const QtRedisReply &object);


// ------------------------------------------------------------------------
// -- TOOLS COMMANDS ------------------------------------------------------
// ------------------------------------------------------------------------

static QString typeToStr(const ReplyType &type);

//! Check the result for correctness.
//! This method checks the type of the resulting object, and if it is:
//! - ReplyType::Error - return false
//! - ReplyType::Nil - return false
//! - ReplyType::Status - if the response != "OK" and != "QUEUED", then return false
static bool isReplySuccess(const QtRedisReply &reply);

//! Convert the server response to qlonglong. 
//! Note: If the object is an array, the first element of the array is used for conversion.
//! Warn: This checks the ReplyType::Integer type! If the object type is different, -1 is returned!
static qlonglong replyToLong(const QtRedisReply &reply);

//! Convert the server response to string. 
//! Note: If the object is an array, the first element of the array is used for conversion.
//! Warn: This checks the ReplyType::String or ReplyType::Status type! If the object type is different, empty string is returned!
static QString replyToString(const QtRedisReply &reply);

//! Convert the server response to byte-array. 
//! Note: If the object is an array, the first element of the array is used for conversion.
//! Warn: This checks the ReplyType::String or ReplyType::Status type! If the object type is different, empty byte-array is returned!
static QByteArray replyToByteArray(const QtRedisReply &reply);

//!
//! Convert the server response to a list.
//! Note: If singleValueForNonArray = true, then add a single object to the list if reply is not an array.
//! Warn: The ReplyType::String type is checked for array objects! If the object's type is different, it will not be added to the result list!
static QStringList replyToStringList(const QtRedisReply &reply, const bool singleValueForNonArray = false);

//!
//! Convert the server response to a list.
//! Note: If singleValueForNonArray = true, then add a single object to the list if reply is not an array.
//! Warn: The ReplyType::String type is checked for array objects! If the object's type is different, it will not be added to the result list!
static QList<QByteArray> replyToByteArrayList(const QtRedisReply &reply, const bool singleValueForNonArray = false);

//! Convert the server response to bool.
//! Note: If the object is an array, the first element of the array is used for conversion.
//! Warn: This checks the ReplyType::Integer! If the object type is different, false is returned!
static bool replyIntToBool(const QtRedisReply &reply);

//! Convert the server response simple string to bool.
//! Note: If the object is an array, the first element of the array is used for conversion.
//! Warn: This checks the ReplyType::String or ReplyType::Status! If the object type is different, false is returned!
//! Note: If (reply.strValue() == "OK" || reply.strValue() == "QUEUED"), then true, otherwise - false.
static bool replySimpleStringToBool(const QtRedisReply &reply);
```


## Code examples

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
const QStringList redisKeysList = QtRedisReply::replyToStringList(_redisClient.redisKeys());
qDebug() << "Redis keys:" << redisKeysList;

// Remove all Redis keys (ver. 1)
const qlonglong delNum = QtRedisReply::replyToLong(_redisClient.redisDel(redisKeysList))
if (delNum == 0) {
    qWarning() << qPrintable(QString("Del redis keys failed! Error: %1")
                             .arg(_redisClient.lastError()));
} else {
    qDebug() << qPrintable(QString("Del redis keys success (del num: %1)").arg(delNum));
}

// Remove all Redis keys (ver. 2)
for (const QString &key : redisKeysList) {
    const qlonglong delNum = QtRedisReply::replyToLong(_redisClient.redisDel(QStringList(key)));
    if (delNum == 0) {
        qWarning() << qPrintable(QString("Del redis key (%1) failed! Error: %2")
                                 .arg(key)
                                 .arg(_redisClient.lastError()));
    } else {
        qDebug() << qPrintable(QString("Del redis key (%1) success (del num: %2)")
                               .arg(key)
                               .arg(delNum));
    }
}

// Set Redis value
QString rKey("my_redis_key");
QString rValue("my_redis_value");
if (!QtRedisReply::replySimpleStringToBool(_redisClient.redisSet(rKey, rValue))) {
    qWarning() << qPrintable(QString("Add redis key-value failed (key: %1)! Error: %2")
                             .arg(rKey)
                             .arg(_redisClient.lastError()));
}

// Get Redis value
const QtRedisReply reply = _redisClient.redisGet(rKey);
qDebug() << qPrintable(QString("Get redis key (%1):").arg(rKey)) << reply;

// Disconnect
_redisClient.redisDisconnect();
```

### Base Redis client example with SSL/TLS

>
> NOTE: 
>
> If you see errors in the redis server log: 
> 
> 1. ```# Error accepting a client connection: error:1408F10B:SSL routines:ssl3_get_record:wrong version number```
> 
>       This error indicates that you are using a standard TCP connection. 
>       Use `SSL/TLS connect version 1` or `SSL/TLS connect version 2`.
> 
> 2. ```# Error accepting a client connection: error:1417C0C7:SSL routines:tls_process_client_certificate:peer did not return a certificate```
> 
>       This error indicates that you have not installed certificates for the client. 
>       Use `SSL/TLS connect version 1`.
> 
> 3. ```# Error accepting a client connection: error:14209102:SSL routines:tls_early_post_process_client_hello:unsupported protocol```
> 
>       This error indicates that you have installed a TLS protocol version that is not supported by the server.
>       Use `SSL/TLS connect version 1` or `SSL/TLS connect version 2`.
>

#### SSL/TLS connect version 1

>
> NOTE:
>
> This example is valid if `tls-auth-clients yes` is enabled in the redis.conf file.
>

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

// 0. Create SSL configuration
QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

// 1. enable TLS 1.3 or later
sslConfig.setProtocol(QSsl::TlsV1_3OrLater);

// 2. Loading the CLIENT CERTIFICATE (client.crt)
QFile certFile("[PATH TO FILE]/client.crt");
if (certFile.open(QIODevice::ReadOnly)) {
    QSslCertificate clientCert(&certFile, QSsl::Pem);
    sslConfig.setLocalCertificate(clientCert);
    certFile.close();
} else {
    qDebug() << "Failed to open client certificate file!";
    return;
}

// 3. Loading the CLIENT PRIVATE KEY (client.key)
QFile keyFile("[PATH TO FILE]/client.key");
if (keyFile.open(QIODevice::ReadOnly)) {
    // Specify QSsl::Rsa or QSsl::Ec depending on your key type.
    // If the key is password-protected, pass the password as the second argument.
    QSslKey clientKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    sslConfig.setPrivateKey(clientKey);
    keyFile.close();
} else {
    qDebug() << "Failed to open client key file!";
    return;
}

// 4. (Optional) Add the root certificate (ca.crt) to trust the server
sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
QList<QSslCertificate> caCerts = QSslCertificate::fromPath("[PATH TO FILE]/ca.crt");
sslConfig.setCaCertificates(caCerts);

// 5. If the certificates are self-signed and server verification is not required:
sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

// Connect to Redis...
if (!_redisClient.redisConnectEncrypted("127.0.0.1", 6379, sslConfig)) {
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

// ...

// Set Redis value
QString rKey("my_redis_key");
QString rValue("my_redis_value");
if (!QtRedisReply::replySimpleStringToBool(_redisClient.redisSet(rKey, rValue))) {
    qWarning() << qPrintable(QString("Add redis key-value failed (key: %1)! Error: %2")
                             .arg(rKey)
                             .arg(_redisClient.lastError()));
}

// Get Redis value
const QtRedisReply reply = _redisClient.redisGet(rKey);
qDebug() << qPrintable(QString("Get redis key (%1):").arg(rKey)) << reply;

// Disconnect
_redisClient.redisDisconnect();
```

#### SSL/TLS connect version 2

>
> NOTE:
>
> This example is valid if `tls-auth-clients optional` or `tls-auth-clients no` is enabled in the redis.conf file.
>

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

// 0. Create SSL configuration
QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

// 1. enable TLS 1.3 or later
sslConfig.setProtocol(QSsl::TlsV1_3OrLater);

// 2. (Optional) Add the root certificate (ca.crt) to trust the server
sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
QList<QSslCertificate> caCerts = QSslCertificate::fromPath("[PATH TO FILE]/ca.crt");
sslConfig.setCaCertificates(caCerts);

// 3. If the certificates are self-signed and server verification is not required:
sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

// Connect to Redis...
if (!_redisClient.redisConnectEncrypted("127.0.0.1", 6379, sslConfig)) {
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

// ...

// Set Redis value
QString rKey("my_redis_key");
QString rValue("my_redis_value");
if (!QtRedisReply::replySimpleStringToBool(_redisClient.redisSet(rKey, rValue))) {
    qWarning() << qPrintable(QString("Add redis key-value failed (key: %1)! Error: %2")
                             .arg(rKey)
                             .arg(_redisClient.lastError()));
}

// Get Redis value
const QtRedisReply reply = _redisClient.redisGet(rKey);
qDebug() << qPrintable(QString("Get redis key (%1):").arg(rKey)) << reply;

// Disconnect
_redisClient.redisDisconnect();
```

### Base Redis client with RedisPipeline example
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

// Create pipeline object
QtRedisPipeline pipeline = _redisClient.createPipeline();

qDebug() << "--- QtRedisPipeline ---";
qDebug() << "set:" << pipeline.redisSet("test-1", "test data 123 456 [1]");
qDebug() << "set:" << pipeline.redisSet("test-2", "test data 123 456 [2]");
qDebug() << "set:" << pipeline.redisSet("test-3", "test data 123 456 [3]");
qDebug() << "set:" << pipeline.redisSet("test-4", "test data 123 456 [4]");
qDebug() << "set:" << pipeline.redisSet("test-5", "test data 123 456 [5]");
qDebug() << "append:" << pipeline.redisAppend("test-1", "append data 1");
qDebug() << "append:" << pipeline.redisAppend("test-5", "append data 5");
qDebug() << "keys:" << pipeline.redisKeys();
qDebug() << "get:" << pipeline.redisGet("test-1");
qDebug() << "get:" << pipeline.redisGet("test-2");
qDebug() << "get:" << pipeline.redisGet("test-3");
qDebug() << "get:" << pipeline.redisGet("test-4");
qDebug() << "get:" << pipeline.redisGet("test-5");

// Execute pipeline commands
qDebug() << "exec:" << pipeline.exec();

// If you only need to get the execution status of all commands, you can use the method below:
//qDebug() << "exec-2-bool:" << pipeline.execToBool();

// If you need to cancel the execution of all commands, you can use the method below:
//qDebug() << "discard:" << pipeline.discard();

// Disconnect
_redisClient.redisDisconnect();
```

### Base Redis client with RedisTransaction example
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

// Create transaction object
//
// Note: If piped = true, then all commands of class QtRedisTransaction will be added to the queue.
//       If piped = false, then all commands of class QtRedisTransaction will be executed immediately when the method is called.
//
// Default: piped = true.
QtRedisTransaction transaction = _redisClient.createTransaction();

qDebug() << "--- QtRedisTransaction ---";
qDebug() << "set:" << transaction.redisSet("test-tr-1", "test data 123 456 [1]"); // cmd index: 0
qDebug() << "set:" << transaction.redisSet("test-tr-2", "test data 123 456 [2]"); // cmd index: 1
qDebug() << "set:" << transaction.redisSet("test-tr-3", "test data 123 456 [3]"); // cmd index: 2
qDebug() << "set:" << transaction.redisSet("test-tr-4", "test data 123 456 [4]"); // cmd index: 3
qDebug() << "set:" << transaction.redisSet("test-tr-5", "test data 123 456 [5]"); // cmd index: 4
qDebug() << "append:" << transaction.redisAppend("test-tr-1", "append data 1");   // cmd index: 5
qDebug() << "append:" << transaction.redisAppend("test-tr-5", "append data 5");   // cmd index: 6
qDebug() << "keys:" << transaction.redisKeys();                                   // cmd index: 7
qDebug() << "get:" << transaction.redisGet("test-tr-1");                          // cmd index: 8
qDebug() << "get:" << transaction.redisGet("test-tr-2");                          // cmd index: 9
qDebug() << "get:" << transaction.redisGet("test-tr-3");                          // cmd index: 10
qDebug() << "get:" << transaction.redisGet("test-tr-4");                          // cmd index: 11
qDebug() << "get:" << transaction.redisGet("test-tr-5");                          // cmd index: 12

// Execute transaction commands
const QtRedisReply reply = transaction.exec();

// Show all reply
qDebug() << "exec:" << reply;

// Show selected keys
qDebug() << "keys[7]:" << QtRedisReply::replyToStringList(reply.arrayValueAt(7)); // get value from cmd reply (index: 7)

// If you only need to get the execution status of all commands, you can use the method below:
//qDebug() << "exec-2-bool:" << transaction.execToBool();

// If you need to cancel the execution of all commands, you can use the method below:
//qDebug() << "discard:" << transaction.discard();

// Disconnect
_redisClient.redisDisconnect();
```

### Base Redis client with RedisTransaction example (with Optimistic Concurrency Control)

>
> NOTE:
>
> The Optimistic Concurrency Control approach allows monitoring changes to one or more keys. 
> If another client changes the monitored key while a transaction is pending, the entire transaction (MULTI/EXEC) is automatically canceled.
>

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

// Create transaction object
//
// Note: If piped = true, then all commands of class QtRedisTransaction will be added to the queue.
//       If piped = false, then all commands of class QtRedisTransaction will be executed immediately when the method is called.
//
// Default: piped = true.
QtRedisTransaction transaction = _redisClient.createTransaction();

qDebug() << "--- QtRedisTransaction ---";

//
// Note: Commands `watch` and `unwatch` is sent directly to the server without being added to the queue.
//

// Watch key
qDebug() << "watch:" << transaction.watch("test-tr-1");

// If you need to disable key watching, then use the method below:
//qDebug() << "unwatch:" << transaction.unwatch();

qDebug() << "set:" << transaction.redisSet("test-tr-1", "test data 123 456 [1]"); // cmd index: 0
qDebug() << "set:" << transaction.redisSet("test-tr-2", "test data 123 456 [2]"); // cmd index: 1
qDebug() << "get:" << transaction.redisGet("test-tr-1");                          // cmd index: 2
qDebug() << "get:" << transaction.redisGet("test-tr-2");                          // cmd index: 3

// Execute transaction commands
const QtRedisReply reply = transaction.exec();

// Show all reply
qDebug() << "exec:" << reply;

// Show selected keys
qDebug() << "get[2]:" << QtRedisReply::replyToStringList(reply.arrayValueAt(2)); // get value from cmd reply (index: 2)

// If you only need to get the execution status of all commands, you can use the method below:
//qDebug() << "exec-2-bool:" << transaction.execToBool();

// If you need to cancel the execution of all commands, you can use the method below:
//qDebug() << "discard:" << transaction.discard();

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


## Example of generating certificates

Below is an example of one of the options for generating SSL certificates for the Redis server and client.

### For a server:
```bash
# Generate CA
openssl genrsa -out ca.key 4096
openssl req -new -x509 -days 365 -key ca.key -out ca.crt -subj "/CN=Redis CA"

# Generate Server Key and Certificate
openssl genrsa -out redis.key 2048
openssl req -new -key redis.key -out redis.csr -subj "/CN=redis-server"
openssl x509 -req -days 365 -in redis.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out redis.crt
```

### For a client:
```bash
# Note: Get ca.crt and ca.key from previous step

# Generate Client Key and Certificate
openssl genrsa -out redis-cli.key 2048
openssl req -new -key redis-cli.key -out redis-cli.csr -subj "/CN=redis-cli"
openssl x509 -req -days 365 -in redis-cli.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out redis-cli.crt
```


## Example configuration section for enabling SSL/TLS on the Redis server

Modify your redis.conf configuration file to define the TLS listening port and point to your certificate paths:

```conf
################################# TLS/SSL #####################################

# By default, TLS/SSL is disabled. To enable it, the "tls-port" configuration
# directive can be used to define TLS-listening ports. To enable TLS on the
# default port, use:

# ...

# Disable standard unencrypted TCP port (Optional, for total enforcement)
port 0

# Enable TLS port
tls-port 6379

# Certificate paths
tls-cert-file /etc/ssl/redis/redis.crt
tls-key-file /etc/ssl/redis/redis.key
tls-ca-cert-file /etc/ssl/redis/ca.crt

# Require clients to authenticate with certificates (mTLS)
# Change to "no" if you only want server-side encryption without client certs
tls-auth-clients yes
```

Restart Redis server:

```bash
sudo systemctl restart redis-server
```

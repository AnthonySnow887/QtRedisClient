# QtRedisClient
NoSQL Redis client library based on Qt 5.

## Usage
To use the library, simply include the "QtRedisClient.pri" file in your project:

```
include(QtRedisClient/QtRedisClient.pri)
```

## Code examples

>
> NOTE:
>
> See file "QtRedisClient.h" for a list of available commands.
>

```cpp
#include "QtRedisClient/QtRedisClient.h"

// Create client & connect
QtRedisClient *_redisClient = new QtRedisClient();
if (!_redisClient->redisConnect("127.0.0.1", 6379)) {
    qCritical() << "Connect to redis failed!";
    delete _redisClient;
    return;
}
qDebug() << "Connect to redis success";

// Send Redis ping command
_redisClient->clearLastError();
if (!_redisClient->redisPing()) {
    qCritical() << qPrintable(QString("Redis PING failed! Error: %1")
                              .arg(_redisClient->lastError()));
    delete _redisClient;
    return;
}

// Select Redis keys
QStringList redisKeysList = _redisClient->redisKeys();
qDebug() << "Redis keys:" << redisKeysList;

// Remove all Redis keys (ver. 1)
_redisClient->clearLastError();
if (!_redisClient->redisDel(redisKeysList)) {
    qWarning() << qPrintable(QString("Del redis keys failed! Error: %1")
                             .arg(_redisClient->lastError()));
} else {
    qDebug() << qPrintable(QString("Del redis keys success"));
}

// Remove all Redis keys (ver. 2)
for (const QString &key : qAsConst(redisKeysList)) {
    _redisClient->clearLastError();
    if (!_redisClient->redisDel(QStringList(key))) {
        qWarning() << qPrintable(QString("Del redis key (%1) failed! Error: %2")
                                 .arg(key)
                                 .arg(_redisClient->lastError()));
    } else {
        qDebug() << qPrintable(QString("Del redis key (%1) success")
                               .arg(key));
    }
}

// Set Redis value
QString rKey("my_redis_key");
QString rValue("my_redis_value");
_redisClient->clearLastError();
if (!_redisClient->redisSet(rKey, rValue)) {
    qWarning() << qPrintable(QString("Add redis key-value failed (key: %1)! Error: %2")
                             .arg(rKey)
                             .arg(_redisClient->lastError()));
}

// Disconnect
_redisClient->redisDisconnect();
delete _redisClient;
```



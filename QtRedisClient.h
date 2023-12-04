#ifndef QTREDISCLIENT_H
#define QTREDISCLIENT_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QMultiMap>
#include <QVariant>
#include <QVector>
#include <QMutex>

#include "NetworkLayer/QtRedisTransporter.h"
#include "QtRedisClientInfo.h"
#include "QtRedisClientVersion.h"

//!
//! \file QtRedisClient.h
//! \class QtRedisClient
//! \brief Класс по работе с NoSQL базой данных Redis
//!
//! Документация по командам: https://redis.io/commands
//!
class QtRedisClient : public QObject
{
    Q_OBJECT

public:
    QtRedisClient();
    virtual ~QtRedisClient();

    static QString libraryVersion();

    QString lastError() const;
    bool hasLastError() const;
    void setLastError(const QString &error);
    void clearLastError();

    bool redisIsConnected();

    QtRedisTransporter::TransporterType redisContextType();
    QtRedisTransporter::TransporterChannelMode redisContextChannelMode();

    bool redisConnect(const QString &host,
                      const int port = 6379,
                      const int timeOutMsec = -1,
                      const QtRedisTransporter::TransporterChannelMode contextChannelMode = QtRedisTransporter::TransporterChannelMode::CurrentConnection);

    // NOTE: not tested!
    bool redisConnectEncrypted(const QString &host,
                               const int port,
                               const int timeOutMsec = -1,
                               const QtRedisTransporter::TransporterChannelMode contextChannelMode = QtRedisTransporter::TransporterChannelMode::CurrentConnection);

#if defined(Q_OS_LINUX)
    bool redisConnectUnix(const QString &sockPath = QString("/tmp/redis.sock"),
                          const int timeOutMsec = -1,
                          const QtRedisTransporter::TransporterChannelMode contextChannelMode = QtRedisTransporter::TransporterChannelMode::CurrentConnection);
#endif

    bool redisReconnect(const int timeOutMsec = -1);

    void redisDisconnect();

    // ------------------------------------------------------------------------
    // -- BASE COMMANDS -------------------------------------------------------
    // ------------------------------------------------------------------------
    QtRedisReply redisExecCommand(const QString &command);
    QtRedisReply redisExecCommand(const QByteArray &command);
    QtRedisReply redisExecCommandArgv(const QStringList &commandArgv);
    QtRedisReply redisExecCommandArgv(const QList<QByteArray> &commandArgv);

    bool redisCheckCommand(const QString &command);
    bool redisCheckCommandArgv(const QStringList &commandArgv);

    // ------------------------------------------------------------------------
    // -- SERVER COMMANDS -----------------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- KEY-VALUE COMMANDS --------------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- LIST COMMANDS -------------------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- STORED COMMANDS -----------------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- SORTED STORED COMMANDS ----------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- PUB/SUB COMMANDS ----------------------------------------------------
    // ------------------------------------------------------------------------
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

    // ------------------------------------------------------------------------
    // -- TOOLS COMMANDS ------------------------------------------------------
    // ------------------------------------------------------------------------
    qlonglong replyToLong(const QtRedisReply &reply);
    QString replyToString(const QtRedisReply &reply);
    QStringList replyToArray(const QtRedisReply &reply);
    bool replyIntToBool(const QtRedisReply &reply);
    bool replySimpleStringToBool(const QtRedisReply &reply);

protected:
    mutable QMutex      _mutex;                 //!< мьютекс
    mutable QMutex      _mutexErr;              //!< мьютекс
    QString             _lastError;             //!< ошибка
    QtRedisTransporter *_transporter {nullptr}; //!< слой взаимодействия с redis

private:
    bool redisSubscribe_safe(const QString &command, const QStringList &channels);
    bool redisUnsubscribe_safe(const QString &command, const QStringList &channels);

signals:
    void incomingChannelMessage(QString channel, QtRedisReply data);
    void incomingChannelShardMessage(QString shardChannel, QtRedisReply data);
    void incomingChannelPatternMessage(QString pattern, QString channel, QtRedisReply data);
};

#endif // QTREDISCLIENT_H

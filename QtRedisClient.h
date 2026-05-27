#ifndef QTREDISCLIENT_H
#define QTREDISCLIENT_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QMultiMap>
#include <QVariant>
#include <QVector>
#include <QMutex>

#include "QtRedisBase.h"
#include "QtRedisPipeline.h"
#include "QtRedisClientInfo.h"
#include "QtRedisClientVersion.h"
#include "NetworkLayer/QtRedisTransporter.h"

//!
//! \file QtRedisClient.h
//! \class QtRedisClient
//! \brief Класс по работе с NoSQL базой данных Redis
//!
//! Документация по командам: https://redis.io/commands
//!
class QtRedisClient : public QObject, public QtRedisBase<QtRedisClient, QtRedisReply>
{
    Q_OBJECT
    Q_DISABLE_COPY(QtRedisClient)
    friend class QtRedisBase<QtRedisClient, QtRedisReply>;

public:
    QtRedisClient();
    ~QtRedisClient();

    static QString libraryVersion();

    // ------------------------------------------------------------------------
    // -- CONNECT/DISCONNECT FUNCTIONS ----------------------------------------
    // ------------------------------------------------------------------------
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

    //---------
    QtRedisPipeline createPipeline();

protected:
    std::shared_ptr<QtRedisTransporter> _transporter {nullptr}; //!< слой взаимодействия с redis

    QtRedisReply processCommand(const QtRedisCommand &command);

private:
    bool redisSubscribe_safe(const QString &command, const QStringList &channels);
    bool redisUnsubscribe_safe(const QString &command, const QStringList &channels);

signals:
    void contextConnected(QString contextUid, QString host, int port, int dbIndex);
    void contextDisconnected(QString contextUid, QString host, int port, int dbIndex);

    void incomingChannelMessage(QString channel, QtRedisReply data);
    void incomingChannelShardMessage(QString shardChannel, QtRedisReply data);
    void incomingChannelPatternMessage(QString pattern, QString channel, QtRedisReply data);
};

#endif // QTREDISCLIENT_H

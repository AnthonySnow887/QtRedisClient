#ifndef QTREDISTRANSPORTER_H
#define QTREDISTRANSPORTER_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <QList>

#include "QtRedisContext.h"
#include "../QtRedisCommand.h"
#include "../QtRedisReply.h"

//!
//! \file QtRedisTransporter.h
//! \class QtRedisTransporter
//! \brief Класс транспорта для работы с Redis
//!
class QtRedisTransporter : public QObject
{
    Q_OBJECT

public:
    //!
    //! \brief Типы странспорта
    //!
    enum class Type {
        NoType = 0, //!< не задан
        Tcp,        //!< tcp
        Ssl,        //!< tcp-ssl
        Unix        //!< unix-sockets
    };

    //!
    //! \brief Типы режимов подключения для pub/sub
    //!
    enum class ChannelMode {
        CurrentConnection = 0,  //!< использовать текущее соединение для pub/sub
        SeparateConnection      //!< использовать отдельное соединение для pub/sub
    };

    explicit QtRedisTransporter(const QtRedisTransporter::ChannelMode contextChannelMode);
    ~QtRedisTransporter();

    Type type() const;
    ChannelMode channelMode() const;
    bool isInit() const;
    QString host() const;
    int port() const;
    int currentDbIndex() const;

    void setSslConfig(const QSslConfiguration &sslConfig);
    QSslConfiguration sslConfig() const;

    bool initTransporter(const Type &type,
                         const QString &host,
                         const int port,
                         QString &error);

    void clearTransporter();

    bool connectToServer(QString &error, const int timeoutMSec = 0);
    bool reconnectToServer(QString &error, const int timeoutMSec = 0);
    bool subscribeToServer(QString &error, const int timeoutMSec = 0);
    void unsubscribeFromServer();
    void disconnectFromServer();
    bool isConnected() const;
    bool isSubscribed() const;

    QtRedisReply sendCommand(const QtRedisCommand &command, QString &error, bool *ok = 0);
    QtRedisReply sendCommands(const QList<QtRedisCommand> &commands, QString &error, bool *ok = 0);

    QtRedisReply sendChannelCommand(const QtRedisCommand &command, QString &error, bool *ok = 0);

protected:
    Type            _type {Type::NoType};                            //!< тип
    ChannelMode     _channelMode {ChannelMode::CurrentConnection};   //!< тип соединения для pub/sub
    int             _timeoutMSec {0};                                //!< время ожидания мсек

    QtRedisContext  *_context {nullptr};                             //!< контекс redis-a
    QtRedisContext  *_contextSub {nullptr};                          //!< контекс redis-a для subscribe

    mutable QMutex  _mutex;                                          //!< мьютекс

    QtRedisContext *makeContext_unsafe(const Type &type,
                                       const QString &host,
                                       const int port);

    QtRedisContext *channelContext_unsafe() const;

    QtRedisReply sendContextCommand(QtRedisContext *context, const QtRedisCommand &command, QString &error, bool *ok = 0);
    QtRedisReply sendContextCommands(QtRedisContext *context, const QList<QtRedisCommand> &commands, QString &error, bool *ok = 0);

    bool isCommandSelect(const QtRedisCommand &command) const;
    void checkCommandResult(QtRedisContext *context, const QtRedisCommand &command, const QtRedisReply &reply);

protected slots:
    void onConnected();
    void onDisconnected();
    void onReadyReadSub();

signals:
    void contextConnected(QString contextUid, QString host, int port, int dbIndex);
    void contextDisconnected(QString contextUid, QString host, int port, int dbIndex);

    void incomingChannelMessage(QString channel, QtRedisReply data);
    void incomingChannelShardMessage(QString shardChannel, QtRedisReply data);
    void incomingChannelPatternMessage(QString pattern, QString channel, QtRedisReply data);
};

#endif // QTREDISTRANSPORTER_H

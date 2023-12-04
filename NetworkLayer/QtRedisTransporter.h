#ifndef QTREDISTRANSPORTER_H
#define QTREDISTRANSPORTER_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QVariantList>

#include "QtRedisContext.h"
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
    enum class TransporterType {
        NoType = 0, //!< не задан
        Tcp,        //!< tcp
        Ssl,        //!< tcp-ssl
        Unix        //!< unix-sockets
    };

    //!
    //! \brief Типы режимов подключения для pub/sub
    //!
    enum class TransporterChannelMode {
        CurrentConnection = 0,  //!< использовать текущее соединение для pub/sub
        SeparateConnection      //!< использовать отдельное соединение для pub/sub
    };

    explicit QtRedisTransporter(const QtRedisTransporter::TransporterChannelMode contextChannelMode);
    virtual ~QtRedisTransporter();

    TransporterType type() const;
    TransporterChannelMode channelMode() const;
    bool isInit() const;
    QString host() const;
    int port() const;
    int currentDbIndex() const;

    bool initTransporter(const TransporterType &type,
                         const QString &host,
                         const int port);

    void clearTransporter();

    bool connectToServer(const int timeoutMSec = 0);
    bool reconnectToServer(const int timeoutMSec = 0);
    bool subscribeToServer(const int timeoutMSec = 0);
    void unsubscribeFromServer();
    void disconnectFromServer();
    bool isConnected() const;
    bool isSubscribed() const;

    QtRedisReply sendCommand(const QStringList &command);
    QtRedisReply sendCommand(const QVariantList &command);

    QList<QtRedisReply> sendCommand_lst(const QStringList &command);
    QList<QtRedisReply> sendCommand_lst(const QVariantList &command);

    QtRedisReply sendChannelCommand(const QStringList &command);
    QList<QtRedisReply> sendChannelCommand_lst(const QStringList &command);

protected:
    TransporterType         _type {TransporterType::NoType};                            //!< тип
    TransporterChannelMode  _channelMode {TransporterChannelMode::CurrentConnection};   //!< тип соединения для pub/sub
    int                     _timeoutMSec {0};                                           //!< время ожидания мсек

    QtRedisContext          *_context {nullptr};                                        //!< контекс redis-a
    QtRedisContext          *_contextSub {nullptr};                                     //!< контекс redis-a для subscribe

    mutable QMutex          _mutex;                                                     //!< мьютекс

    QtRedisContext *makeContext_unsafe(const TransporterType &type,
                                       const QString &host,
                                       const int port);

    QtRedisContext *channelContext_unsafe() const;

    QtRedisReply sendContextCommand(QtRedisContext *context, const QStringList &command);
    QtRedisReply sendContextCommand(QtRedisContext *context, const QVariantList &command);

    QList<QtRedisReply> sendContextCommand_lst(QtRedisContext *context, const QStringList &command);
    QList<QtRedisReply> sendContextCommand_lst(QtRedisContext *context, const QVariantList &command);

    void checkCommandResult(QtRedisContext *context, const QStringList &command, const QtRedisReply &reply);
    void checkCommandResult(QtRedisContext *context, const QVariantList &command, const QtRedisReply &reply);

protected slots:
    void onReadyReadSub();

signals:
    void incomingChannelMessage(QString channel, QtRedisReply data);
    void incomingChannelShardMessage(QString shardChannel, QtRedisReply data);
    void incomingChannelPatternMessage(QString pattern, QString channel, QtRedisReply data);
};

#endif // QTREDISTRANSPORTER_H

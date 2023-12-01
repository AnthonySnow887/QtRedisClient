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

    QtRedisTransporter();
    virtual ~QtRedisTransporter();

    TransporterType type() const;
    bool isInit() const;
    bool isSubscribed() const;
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
    bool isConnected();

    QtRedisReply sendCommand(const QStringList &command);
    QtRedisReply sendCommand(const QVariantList &command);

    QtRedisReply sendSubscribeCommand(const QStringList &command);

protected:
    TransporterType     _type {TransporterType::NoType};    //!< тип
    int                 _timeoutMSec {0};                   //!< время ожидания мсек

    QtRedisContext      *_context {nullptr};                //!< контекс redis-a
    QtRedisContext      *_contextSub {nullptr};             //!< контекс redis-a для subscribe

    mutable QMutex       _mutex;                            //!< мьютекс

    QtRedisContext *makeTransporter(const TransporterType &type,
                                    const QString &host,
                                    const int port,
                                    const bool supportSignals);

    QtRedisReply sendConextCommand(QtRedisContext *context, const QStringList &command);
    QtRedisReply sendConextCommand(QtRedisContext *context, const QVariantList &command);

    void checkCommandResult(const QStringList &command, const QtRedisReply &reply);
    void checkCommandResult(const QVariantList &command, const QtRedisReply &reply);

protected slots:
    void onReadyReadSub();

signals:
    void incomingChannelMessage(QString channel, QtRedisReply data);
};

#endif // QTREDISTRANSPORTER_H

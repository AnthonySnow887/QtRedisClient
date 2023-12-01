#ifndef QTREDISCONTEXTTCP_H
#define QTREDISCONTEXTTCP_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QtNetwork/QTcpSocket>

#include "QtRedisContext.h"

//!
//! \file QtRedisContext.h
//! \class QtRedisContext
//! \brief Класс, описывающий TCP контекст Redis-a
//!
class QtRedisContextTcp : public QtRedisContext
{
    Q_OBJECT
    Q_DISABLE_COPY(QtRedisContextTcp)

public:
    explicit QtRedisContextTcp(const QString &host, const uint port, const bool supportSignals = false);
    virtual ~QtRedisContextTcp();

    bool connectToServer(const int msecs = 30000) final;
    bool reconnectToServer(const int msecs = 30000) final;
    void disconnectFromServer() final;
    bool isConnected() final;

    bool canReadRawData() const final;
    qint64 bytesAvailable() const final;
    qint64 writeRawData(const QByteArray &data) final;
    QByteArray readRawData() final;

    bool waitForReadyRead(const int msecs = 30000) final;

protected:
    QTcpSocket     *_socket {nullptr};  //!< tcp-сокет
    mutable QMutex  _mutex;             //!< мьютекс
};

#endif // QTREDISCONTEXTTCP_H

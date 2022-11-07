#ifndef QTREDISCONTEXTUNIX_H
#define QTREDISCONTEXTUNIX_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QtNetwork/QLocalSocket>

#include "QtRedisContext.h"

//!
//! \file QtRedisContextUnix.h
//! \class QtRedisContextUnix
//! \brief Класс, описывающий UNIX контекст Redis-a
//!
class QtRedisContextUnix : public QtRedisContext
{
    Q_OBJECT
    Q_DISABLE_COPY(QtRedisContextUnix)

public:
    explicit QtRedisContextUnix(const QString &sockPath);
    virtual ~QtRedisContextUnix();

    bool connectToServer(const int msecs = 300000) final;
    bool reconnectToServer(const int msecs = 300000) final;
    void disconnectFromServer() final;
    bool isConnected() final;

    bool canReadRawData() const final;
    qint64 bytesAvailable() const final;
    qint64 writeRawData(const QByteArray &data) final;
    QByteArray readRawData() final;

    bool waitForReadyRead(const int msecs = 30000) final;

protected:
    QLocalSocket   *_socket {nullptr};  //!< unix-сокет
    mutable QMutex  _mutex;             //!< мьютекс
};

#endif // QTREDISCONTEXTUNIX_H

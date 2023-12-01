#ifndef QTREDISCONTEXTSSL_H
#define QTREDISCONTEXTSSL_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QtNetwork/QSslSocket>

#include "QtRedisContext.h"

//!
//! \file QtRedisContextSsl.h
//! \class QtRedisContextSsl
//! \brief Класс, описывающий TCP-SSL контекст Redis-a
//!
class QtRedisContextSsl : public QtRedisContext
{
    Q_OBJECT
    Q_DISABLE_COPY(QtRedisContextSsl)

public:
    explicit QtRedisContextSsl(const QString &host, const uint port, const bool supportSignals = false);
    virtual ~QtRedisContextSsl();

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
    QSslSocket     *_socket {nullptr};  //!< SSL-сокет
    mutable QMutex  _mutex;             //!< мьютекс
};

#endif // QTREDISCONTEXTSSL_H

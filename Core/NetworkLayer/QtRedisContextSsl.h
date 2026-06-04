#ifndef QTREDISCONTEXTSSL_H
#define QTREDISCONTEXTSSL_H

#include <QObject>
#include <QMutex>
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
    explicit QtRedisContextSsl(const QString &host, const uint port);
    ~QtRedisContextSsl();


    void setSslConfig(const QSslConfiguration &sslConfig) final;
    QSslConfiguration sslConfig() const final;

    bool connectToServer(const int msecs, QString &error) final;
    bool reconnectToServer(const int msecs, QString &error) final;
    void disconnectFromServer() final;
    bool isConnected() final;

    bool canReadRawData() const final;
    qint64 bytesAvailable() const final;
    qint64 writeRawData(const QByteArray &data) final;
    QByteArray readRawData() final;

    bool waitForReadyRead(const int msecs = 30000) final;

protected:
    QSslSocket     *_socket {nullptr};  //!< SSL-сокет
    QSslConfiguration _sslConfig;       //!< SSL конфигурация
    mutable QMutex  _mutex;             //!< мьютекс
};

#endif // QTREDISCONTEXTSSL_H

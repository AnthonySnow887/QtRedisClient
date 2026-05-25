#ifndef QTREDISPIPELINE_H
#define QTREDISPIPELINE_H

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
class QtRedisPipeline : public QObject
{
    Q_OBJECT

public:
    QtRedisPipeline();
    virtual ~QtRedisPipeline();

protected:
    mutable QMutex      _mutex;                 //!< мьютекс
    mutable QMutex      _mutexErr;              //!< мьютекс
    QString             _lastError;             //!< ошибка
    QtRedisTransporter *_transporter {nullptr}; //!< слой взаимодействия с redis

private:
};

#endif // QTREDISPIPELINE_H

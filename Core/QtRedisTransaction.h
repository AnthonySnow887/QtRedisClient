#ifndef QTREDISTRANSACTION_H
#define QTREDISTRANSACTION_H

#include <memory>

#include "QtRedisBase.h"
#include "NetworkLayer/QtRedisTransporter.h"

//!
//! \file QtRedisTransaction.h
//! \class QtRedisTransaction
//! \brief Класс по работе с NoSQL базой данных Redis в режиме RedisTransaction
//!
//! Документация по командам: https://redis.io/docs/latest/commands/
//!
//! Note: Commands `watch` and `unwatch` is sent directly to the server without being added to the queue.
//!
class QtRedisTransaction : public QtRedisBase<QtRedisTransaction, bool>
{
    friend class QtRedisBase<QtRedisTransaction, bool>;

public:
    QtRedisTransaction(std::shared_ptr<QtRedisTransporter> transporter, const bool piped);
    ~QtRedisTransaction();

    QtRedisTransaction(const QtRedisTransaction &object)
        : _transporter(object._transporter)
        , _commandList(object._commandList)
        , _inTransaction(object._inTransaction)
        , _piped(object._piped)
    {}

    QtRedisTransaction& operator=(const QtRedisTransaction &object) {
        if (this == &object)
            return *this;
        _transporter = object._transporter;
        _commandList = object._commandList;
        _inTransaction = object._inTransaction;
        _piped = object._piped;
        return *this;
    }

    bool watch(const QString &key);
    bool watch(const QStringList &keys);
    bool unwatch();

    QtRedisReply exec();
    bool execToBool();

    void discard();

protected:
    std::shared_ptr<QtRedisTransporter> _transporter {nullptr}; //!< слой взаимодействия с redis
    QList<QtRedisCommand> _commandList; //!< список команд
    QStringList _watchList;             //!< список отслеживаемых ключей

    bool _inTransaction {false};        //!< находится ли объект в режиме транзакции
    bool _piped {false};                //!< находится ли объект в режиме Pipeline (накомпления команд для отправки одним пакетом)

    bool processCommand(const QtRedisCommand &command);

    bool isReplyQueued(const QtRedisReply &reply);
    bool openTransaction_unsafe();
    bool discardTransaction_unsafe();
};

#endif // QTREDISTRANSACTION_H

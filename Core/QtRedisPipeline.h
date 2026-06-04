#ifndef QTREDISPIPELINE_H
#define QTREDISPIPELINE_H

#include <memory>

#include "QtRedisBase.h"
#include "NetworkLayer/QtRedisTransporter.h"

//!
//! \file QtRedisPipeline.h
//! \class QtRedisPipeline
//! \brief Класс по работе с NoSQL базой данных Redis в режиме RedisPipeline
//!
//! Документация по командам: https://redis.io/docs/latest/commands/
//!
class QtRedisPipeline : public QtRedisBase<QtRedisPipeline, bool>
{
    friend class QtRedisBase<QtRedisPipeline, bool>;

public:
    QtRedisPipeline(std::shared_ptr<QtRedisTransporter> transporter);
    ~QtRedisPipeline();

    QtRedisPipeline(const QtRedisPipeline &object)
        : _transporter(object._transporter)
        , _commandList(object._commandList)
    {}

    QtRedisPipeline& operator=(const QtRedisPipeline &object) {
        if (this == &object)
            return *this;
        _transporter = object._transporter;
        _commandList = object._commandList;
        return *this;
    }

    QtRedisReply exec();
    bool execToBool();

    void discard();

protected:
    std::shared_ptr<QtRedisTransporter> _transporter {nullptr}; //!< слой взаимодействия с redis
    QList<QtRedisCommand> _commandList; //!< список команд

    bool processCommand(const QtRedisCommand &command);
};

#endif // QTREDISPIPELINE_H

#ifndef QTREDISPARSER_H
#define QTREDISPARSER_H

#include <QByteArray>
#include <QStringList>
#include <QVariantList>

#include "../QtRedisReply.h"
#include "../QtRedisCommand.h"

//!
//! \file QtRedisParser.h
//! \class QtRedisParser
//! \brief Класс разбора/формирования данных Redis-a
//!
class QtRedisParser
{
public:
    QtRedisParser() = default;
    ~QtRedisParser() = default;

    static QByteArray createRawData(const QtRedisCommand &command);

    static QtRedisReply parseRawData(const QByteArray &data, bool *ok = 0);

protected:
    static QByteArray createRawDataArgument(const QByteArray &arg);

    static QtRedisReply parseRawDataTypes(QByteArray &data, bool *ok = 0);
    static QtRedisReply parseRawDataToState(QByteArray &data, bool *ok = 0);
    static QtRedisReply parseRawDataToError(QByteArray &data, bool *ok = 0);
    static QtRedisReply parseRawDataToInt(QByteArray &data, bool *ok = 0);
    static QtRedisReply parseRawDataToString(QByteArray &data, bool *ok = 0);
    static QtRedisReply parseRawDataToArray(QByteArray &data, bool *ok = 0);
};

#endif // QTREDISPARSER_H

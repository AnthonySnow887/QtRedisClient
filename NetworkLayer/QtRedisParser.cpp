#include "QtRedisParser.h"

//!
//! \brief Создать byte-данные для Redis-а
//! \param command Команда и ее аргументы
//! \return
//!
QByteArray QtRedisParser::createRawData(const QStringList &command)
{
    if (command.isEmpty())
        return QByteArray();

    QByteArray ba;
    ba += QString("*%1\r\n").arg(command.size()).toUtf8();
    for (const QString &arg : command)
        ba += QtRedisParser::createRawDataArgument(arg);

    return ba;
}

//!
//! \brief Создать byte-данные для Redis-а
//! \param command Команда и ее аргументы
//! \return
//!
QByteArray QtRedisParser::createRawData(const QVariantList &command)
{
    if (command.isEmpty())
        return QByteArray();

    QByteArray ba;
    ba += QString("*%1\r\n").arg(command.size()).toUtf8();
    for (const QVariant &arg : command)
        ba += QtRedisParser::createRawDataArgument(arg);

    return ba;
}

//!
//! \brief Разобрать ответ от Redis-а
//! \param data "Сырые" данные
//! \return
//!
QtRedisReply QtRedisParser::parseRawData(const QByteArray &data, bool *ok)
{
    return QtRedisParser::parseRawDataList(data, ok)[0];
}

//!
//! \brief Разобрать ответ от Redis-а (если в данных содержиться несколько сообщений)
//! \param data
//! \param ok
//! \return
//!
QList<QtRedisReply> QtRedisParser::parseRawDataList(const QByteArray &data, bool *ok)
{
    QByteArray buffData = data;
    QList<QtRedisReply> replyList;
    while (true) {
        replyList.append(QtRedisParser::parseRawDataTypes(buffData, ok));
        if (ok && *ok == false)
            break;
        if (buffData.isEmpty())
            break;
    }
    return replyList;
}

//!
//! \brief Создать строку-команду для Redis-а
//! \param arg Аргумент
//! \return
//!
QByteArray QtRedisParser::createRawDataArgument(const QString &arg)
{
    if (arg.isEmpty())
        return QByteArray();

    // Not used! Send only string!
    //    bool isOk = true;
    //    arg.toFloat(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toFloat()).toUtf8();
    //    arg.toDouble(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toDouble()).toUtf8();
    //    arg.toShort(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toShort()).toUtf8();
    //    arg.toInt(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();
    //    arg.toUInt(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();
    //    arg.toLong(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();
    //    arg.toULong(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();
    //    arg.toLongLong(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();
    //    arg.toULongLong(&isOk);
    //    if (isOk)
    //        return QString(":%1\r\n").arg(arg.toLongLong()).toUtf8();

    return QString("$%1\r\n%2\r\n").arg(arg.length()).arg(arg).toUtf8();
}

//!
//! \brief Создать строку-команду для Redis-а
//! \param arg Аргумент
//! \return
//!
QByteArray QtRedisParser::createRawDataArgument(const QVariant &arg)
{
    if (!arg.isValid() || arg.isNull())
        return QByteArray();

    // Not used! Send only string!
    //    if (arg.type() == QVariant::Int
    //        || arg.type() == QVariant::UInt
    //        || arg.type() == QVariant::LongLong
    //        || arg.type() == QVariant::ULongLong
    //        || arg.type() == QVariant::Double)
    //        return QString(":%1")
    //                .arg(arg.toLongLong())
    //                .toUtf8();

    if (arg.type() == QVariant::String)
        return QString("$%1\r\n%2\r\n")
                .arg(arg.toString().length())
                .arg(arg.toString())
                .toUtf8();

    if (arg.type() == QVariant::ByteArray)
        return QString("$%1\r\n%2\r\n")
                .arg(arg.toByteArray().length())
                .arg(arg.toByteArray().constData())
                .toUtf8();

    qWarning() << qPrintable(QString("[QtRedisParser][createRawDataArgument] Invalid QVariant type! (Type: %1)").arg(arg.type()));
    return QByteArray();
}

//!
//! \brief Разобрать ответ от Redis-а
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataTypes(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // state string
    if (data.at(0) == '+')
        return QtRedisParser::parseRawDataToState(data, ok);

    // error string
    else if (data.at(0) == '-')
        return QtRedisParser::parseRawDataToError(data, ok);

    // integer
    else if (data.at(0) == ':')
        return QtRedisParser::parseRawDataToInt(data, ok);

    // string
    else if (data.at(0) == '$')
        return QtRedisParser::parseRawDataToString(data, ok);

    // array
    else if (data.at(0) == '*')
        return QtRedisParser::parseRawDataToArray(data, ok);

    else
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawData] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));

    return QtRedisReply();
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_Status
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
//! Simple Strings are encoded in the following way: a plus character, followed by a string that cannot contain a CR or LF character (no newlines are allowed), terminated by CRLF (that is "\r\n").
//!
//! Simple Strings are used to transmit non binary safe strings with minimal overhead. For example many Redis commands reply with just "OK" on success,
//! that as a RESP Simple String is encoded with the following 5 bytes:
//!
//! "+OK\r\n"
//!
//! In order to send binary-safe strings, RESP Bulk Strings are used instead.
//!
//! When Redis replies with a Simple String, a client library should return to the caller a string composed of the first character after the '+' up to the end of the string,
//! excluding the final CRLF bytes.
//!
QtRedisReply QtRedisParser::parseRawDataToState(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // state string
    if (data.at(0) != '+') {
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawDataToState] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1)
        return QtRedisReply();
    if (buffIndex != -1) {
        buffData = buffData.remove(buffIndex, 2);
        buffData.remove(0, 1);
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Status);
    reply.setRawValue(buffData);
    if (ok)
        *ok = true;

    data = data.remove(0, buffIndex + 2); // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_Error
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataToError(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // error string
    if (data.at(0) != '-') {
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawDataToError] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1)
        return QtRedisReply();
    if (buffIndex != -1) {
        buffData = buffData.remove(buffIndex, 2);
        buffData.remove(0, 1);
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Error);
    reply.setRawValue(buffData);
    if (ok)
        *ok = true;

    data = data.remove(0, buffIndex + 2); // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_Integer
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataToInt(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // integer
    if (data.at(0) != ':') {
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawDataToInt] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1)
        return QtRedisReply();
    if (buffIndex != -1) {
        buffData = buffData.remove(buffIndex, 2);
        buffData.remove(0, 1);
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Integer);
    reply.setRawValue(buffData);
    if (ok)
        *ok = true;

    data = data.remove(0, buffIndex + 2); // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_String
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataToString(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // string
    // $3\r\nfoo\r\n
    // $-1\r\n - Nil
    if (data.at(0) != '$') {
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawDataToString] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    int buffIndexLen = buffData.indexOf("\r\n");
    if (buffIndexLen < 0)
        return QtRedisReply();

    // get strlen...
    QByteArray strLenData = data;
    strLenData = strLenData.remove(buffIndexLen, strLenData.length() - buffIndexLen); // remove data
    strLenData = strLenData.remove(0, 1); // remove type
    qlonglong strLen = strLenData.toLongLong();
    // check is Nil object
    if (strLen < 0)
        return QtRedisReply();

    // get data...
    buffData = buffData.remove(0, buffIndexLen + 2); // remove type + strlen
    int buffIndexData = buffData.indexOf("\r\n", strLen);
    if (buffIndexData == -1)
        return QtRedisReply();
    if (buffIndexData != -1)
        buffData = buffData.remove(buffIndexData, buffData.length() - buffIndexData);

    // check data len
    if (buffData.length() != strLen)
        return QtRedisReply();

    QtRedisReply reply(QtRedisReply::ReplyType::String);
    reply.setRawValue(buffData);
    if (ok)
        *ok = true;

    data = data.remove(0, buffIndexLen + 2 + buffIndexData + 2); // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_Array
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataToArray(QByteArray &data, bool *ok)
{
    // clear ok
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty())
        return QtRedisReply();

    // array
    if (data.at(0) != '*') {
        qWarning() << qPrintable(QString("[QtRedisParser][parseRawDataToArray] Invalid type! Symbol at 0 = \"%1\"").arg(data.at(0)));
        return QtRedisReply();
    }

    QByteArray buffData = data;
    int buffIndexLen = buffData.indexOf("\r\n");
    if (buffIndexLen < 0)
        return QtRedisReply();

    // get strlen...
    QByteArray arrayLenData = data;
    arrayLenData = arrayLenData.remove(buffIndexLen, arrayLenData.length() - buffIndexLen); // remove data
    arrayLenData = arrayLenData.remove(0, 1); // remove type
    qlonglong arrayLen = arrayLenData.toLongLong();
    // check is Nil object
    if (arrayLen < 0)
        return QtRedisReply();

    // get data...
    buffData = buffData.remove(0, buffIndexLen + 2); // remove type + len
    QtRedisReply reply = QtRedisReply(QtRedisReply::ReplyType::Array);
    reply.setRawValue(buffData);
    data = buffData; // remove parsed data (index + 2 chars (\r\n))
    if (arrayLen == 0) {
        if (ok)
            *ok = true;

        return reply;
    }

    // parse array args
    while (true) {
        bool buffOk = false;
        QtRedisReply buffReply = QtRedisParser::parseRawDataTypes(buffData, &buffOk);
        if (!buffOk)
            return QtRedisReply();

        reply.appendArrayValue(buffReply);
        if (buffData.isEmpty())
            break;
        if (reply.arrayValueSize() == arrayLen)
            break;
    }
    if (arrayLen != reply.arrayValue().size())
        return QtRedisReply();
    if (ok)
        *ok = true;

    if (!buffData.isEmpty()) {
        int oldRawSize = reply.rawValue().size();
        int newRawSize = buffData.size();
        reply.setRawValue(data.left(oldRawSize - newRawSize));
    }
    data = buffData; // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

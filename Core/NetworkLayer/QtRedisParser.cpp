#include "QtRedisParser.h"

//!
//! \brief Создать byte-данные для Redis-а
//! \param command Команда и ее аргументы
//! \return
//!
QByteArray QtRedisParser::createRawData(const QtRedisCommand &command)
{
    if (!command.isValid())
        return QByteArray();

    QByteArray ba;
    ba += QString("*%1\r\n").arg(command.size()).toUtf8();
    ba += QtRedisParser::createRawDataArgument(command.command());
    for (const QByteArray &arg : command.commandArgv())
        ba += QtRedisParser::createRawDataArgument(arg);

    return ba;
}

//!
//! \brief Разобрать ответ от Redis-а
//! \param data "Сырые" данные
//! \return
//!
QtRedisReply QtRedisParser::parseRawData(const QByteArray &data, QString &error, bool *ok)
{
    QByteArray buffData = data;
    QtRedisReply replyArray;
    while (true) {
        const QtRedisReply reply = QtRedisParser::parseRawDataTypes(buffData, error, ok);
        if (ok && *ok == false)
            return QtRedisReply();
        replyArray.appendArrayValue(reply);
        if (buffData.isEmpty())
            break;
    }
    replyArray.setType(QtRedisReply::ReplyType::Array);
    replyArray.setRawValue(data);
    return replyArray;
}

//!
//! \brief Создать строку-команду для Redis-а
//! \param arg Аргумент
//! \return
//!
QByteArray QtRedisParser::createRawDataArgument(const QByteArray &arg)
{
    if (arg.isEmpty()) {
        qWarning() << qPrintable(QString("[QtRedisParser][createRawDataArgument] arg is Empty!"));
        return QByteArray();
    }
    return QString("$%1\r\n%2\r\n")
            .arg(arg.length())
            .arg(arg.constData())
            .toUtf8();
}

//!
//! \brief Разобрать ответ от Redis-а
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataTypes(QByteArray &data, QString &error, bool *ok)
{
    // clear err & ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data failed (data is empty)!");
        return QtRedisReply();
    }

    // state string
    if (data.at(0) == '+')
        return QtRedisParser::parseRawDataToState(data, error, ok);

    // error string
    else if (data.at(0) == '-')
        return QtRedisParser::parseRawDataToError(data, error, ok);

    // integer
    else if (data.at(0) == ':')
        return QtRedisParser::parseRawDataToInt(data, error, ok);

    // string
    else if (data.at(0) == '$')
        return QtRedisParser::parseRawDataToString(data, error, ok);

    // array
    else if (data.at(0) == '*')
        return QtRedisParser::parseRawDataToArray(data, error, ok);

    else
        error = QString("Parse raw data failed! Invalid type (symbol at 0 = \"%1\")!").arg(data.at(0));

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
QtRedisReply QtRedisParser::parseRawDataToState(QByteArray &data, QString &error, bool *ok)
{
    // clear err & ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data-to-state failed (data is empty)!");
        return QtRedisReply();
    }

    // state string
    if (data.at(0) != '+') {
        error = QString("Parse raw data-to-state failed! Invalid type (symbol at 0 = \"%1\")").arg(data.at(0));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    const int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1) {
        error = QString("Parse raw data-to-state failed (incorrect data - line break character not found)!");
        return QtRedisReply();
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Status);
    reply.setRawValue(buffData.mid(1, buffIndex - 1));
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
QtRedisReply QtRedisParser::parseRawDataToError(QByteArray &data, QString &error, bool *ok)
{
    // clear err & ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data-to-error failed (data is empty)!");
        return QtRedisReply();
    }

    // error string
    if (data.at(0) != '-') {
        error = QString("Parse raw data-to-error failed! Invalid type (symbol at 0 = \"%1\")").arg(data.at(0));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    const int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1) {
        error = QString("Parse raw data-to-error failed (incorrect data - line break character not found)!");
        return QtRedisReply();
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Error);
    reply.setRawValue(buffData.mid(1, buffIndex - 1));
    error = reply.strValue(); // save error message from reply
    if (ok)
        *ok = true; // set parsing success

    data = data.remove(0, buffIndex + 2); // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

//!
//! \brief Преобразовать ответ от Redis-а в объект QtRedisReply::replyType_Integer
//! \param data "Сырые" данные
//! \param index Индекс конца разобранных данных (включает символы '\r\n')
//! \return
//!
QtRedisReply QtRedisParser::parseRawDataToInt(QByteArray &data, QString &error, bool *ok)
{
    // clear ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data-to-int failed (data is empty)!");
        return QtRedisReply();
    }

    // integer
    if (data.at(0) != ':') {
        error = QString("Parse raw data-to-int failed! Invalid type (symbol at 0 = \"%1\")").arg(data.at(0));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    const int buffIndex = buffData.indexOf("\r\n");
    if (buffIndex == -1) {
        error = QString("Parse raw data-to-int failed (incorrect data - line break character not found)!");
        return QtRedisReply();
    }

    QtRedisReply reply(QtRedisReply::ReplyType::Integer);
    reply.setRawValue(buffData.mid(1, buffIndex - 1));
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
QtRedisReply QtRedisParser::parseRawDataToString(QByteArray &data, QString &error, bool *ok)
{
    // clear ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data-to-string failed (data is empty)!");
        return QtRedisReply();
    }

    // string
    // $3\r\nfoo\r\n
    // $-1\r\n - Nil
    if (data.at(0) != '$') {
        error = QString("Parse raw data-to-string failed! Invalid type (symbol at 0 = \"%1\")").arg(data.at(0));
        return QtRedisReply();
    }
    QByteArray buffData = data;
    const int buffIndexLen = buffData.indexOf("\r\n");
    if (buffIndexLen < 0) {
        error = QString("Parse raw data-to-string failed (incorrect data - line break character for string-length not found)!");
        return QtRedisReply();
    }

    // get strlen...
    const QByteArray strLenData = data.mid(1, buffIndexLen - 1);
    const qlonglong strLen = strLenData.toLongLong();
    // check is Nil object (Nil string)
    if (strLen < 0) {
        if (ok)
            *ok = true;
        data.remove(0, buffIndexLen + 2); // remove type + len
        return QtRedisReply();
    }

    // get data...
    buffData = buffData.remove(0, buffIndexLen + 2); // remove type + strlen
    const int buffIndexData = buffData.indexOf("\r\n", strLen);
    if (buffIndexData == -1) {
        error = QString("Parse raw data-to-string failed (incorrect data - line break character for string data not found)!");
        return QtRedisReply();
    }
    buffData = buffData.remove(buffIndexData, buffData.length() - buffIndexData);

    // check data len
    if (buffData.length() != strLen) {
        error = QString("Parse raw data-to-string failed (incorrect length of parsed string data)!");
        return QtRedisReply();
    }

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
QtRedisReply QtRedisParser::parseRawDataToArray(QByteArray &data, QString &error, bool *ok)
{
    // clear ok
    error.clear();
    if (ok)
        *ok = false;

    // check data
    if (data.isEmpty()) {
        error = QString("Parse raw data-to-array failed (data is empty)!");
        return QtRedisReply();
    }

    // array
    if (data.at(0) != '*') {
        error = QString("Parse raw data-to-array failed! Invalid type (symbol at 0 = \"%1\")").arg(data.at(0));
        return QtRedisReply();
    }

    QByteArray buffData = data;
    const int buffIndexLen = buffData.indexOf("\r\n");
    if (buffIndexLen < 0) {
        error = QString("Parse raw data-to-array failed (incorrect data - line break character for array-length not found)!");
        return QtRedisReply();
    }

    // get strlen...
    const QByteArray arrayLenData = data.mid(1, buffIndexLen - 1);;
    const qlonglong arrayLen = arrayLenData.toLongLong();
    // check is Nil object (Nil array)
    if (arrayLen < 0) {
        if (ok)
            *ok = true;
        data.remove(0, buffIndexLen + 2); // remove type + len
        return QtRedisReply();
    }

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
        QtRedisReply buffReply = QtRedisParser::parseRawDataTypes(buffData, error, &buffOk);
        if (!buffOk)
            return QtRedisReply();

        reply.appendArrayValue(buffReply);
        if (buffData.isEmpty())
            break;
        if (reply.arrayValueSize() == arrayLen)
            break;
    }
    if (arrayLen != reply.arrayValue().size()) {
        error = QString("Parse raw data-to-array failed (incorrect length of parsed array data)!");
        return QtRedisReply();
    }
    if (ok)
        *ok = true;

    if (!buffData.isEmpty()) {
        const int oldRawSize = reply.rawValue().size();
        const int newRawSize = buffData.size();
        reply.setRawValue(data.left(oldRawSize - newRawSize));
    }
    data = buffData; // remove parsed data (index + 2 chars (\r\n))
    return reply;
}

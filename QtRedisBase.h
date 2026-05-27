#ifndef QTREDISBASE_H
#define QTREDISBASE_H

#include <type_traits>

#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QMutex>

#include "QtRedisCommand.h"

//!
//! \file QtRedisBase.h
//! \class QtRedisBase
//! \brief Базовый шаблонный класс по работе с NoSQL базой данных Redis
//!
//! Документация по командам: https://redis.io/commands
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
class QtRedisBase
{
public:
    QtRedisBase() {}
    virtual ~QtRedisBase() {}

    // ------------------------------------------------------------------------
    // -- ERRORS FUNCTIONS ----------------------------------------------------
    // ------------------------------------------------------------------------
    bool hasLastError() const;
    const QString &lastError() const;

    // ------------------------------------------------------------------------
    // -- BASE COMMANDS -------------------------------------------------------
    // ------------------------------------------------------------------------
    __RESULT_IMPL redisExecCommand(const QString &command);
    __RESULT_IMPL redisExecCommand(const QByteArray &command);
    __RESULT_IMPL redisExecCommandArgv(const QStringList &commandArgv);
    __RESULT_IMPL redisExecCommandArgv(const QList<QByteArray> &commandArgv);

    // ------------------------------------------------------------------------
    // -- KEY-VALUE COMMANDS --------------------------------------------------
    // ------------------------------------------------------------------------
    __RESULT_IMPL redisKeys(const QString &arg = QString("*"));
    __RESULT_IMPL redisRandomKey();
    __RESULT_IMPL redisExists(const QStringList &keyList);
    __RESULT_IMPL redisGet(const QString &key);
    __RESULT_IMPL redisGetRange(const QString &key, const int startPos, const int endPos);
    __RESULT_IMPL redisGetSet(const QString &key, const QString &value);
    __RESULT_IMPL redisAppend(const QString &key, const QString &appendValue);
    __RESULT_IMPL redisSet(const QString &key,
                           const QString &value,
                           const uint exSec = 0,
                           const uint pxMSec = 0,
                           const QString existFlag = QString());

    __RESULT_IMPL redisSetRange(const QString &key, const QString &value, const int offset);
    __RESULT_IMPL redisDel(const QStringList &keyList);
    __RESULT_IMPL redisStrlen(const QString &key);
    __RESULT_IMPL redisExpire(const QString &key, const uint sec);
    __RESULT_IMPL redisExpireAt(const QString &key, const uint utcSec);
    __RESULT_IMPL redisPExpire(const QString &key, const uint msec);
    __RESULT_IMPL redisPExpireAt(const QString &key, const qint64 utcMsec);
    __RESULT_IMPL redisPersist(const QString &key);
    __RESULT_IMPL redisTtl(const QString &key);
    __RESULT_IMPL redisPTtl(const QString &key);
    __RESULT_IMPL redisDecr(const QString &key);
    __RESULT_IMPL redisDecrBy(const QString &key, const qint64 decr);
    __RESULT_IMPL redisIncr(const QString &key);
    __RESULT_IMPL redisIncrBy(const QString &key, const qint64 incr);
    __RESULT_IMPL redisIncrByFloat(const QString &key, const float incr);
    __RESULT_IMPL redisRename(const QString &key, const QString &newKey);
    __RESULT_IMPL redisRenameNx(const QString &key, const QString &newKey);
    __RESULT_IMPL redisType(const QString &key);
    __RESULT_IMPL redisMSet(const QMap<QString, QString> &keyValue);
    __RESULT_IMPL redisMSetNx(const QMap<QString, QString> &keyValue);
    __RESULT_IMPL redisMGet(const QStringList &keyList);
    __RESULT_IMPL redisMove(const QString &key, const int dbIndex);
    __RESULT_IMPL redisDump(const QString &key);

    // ------------------------------------------------------------------------
    // -- LIST COMMANDS -------------------------------------------------------
    // ------------------------------------------------------------------------
    __RESULT_IMPL redisLIndex(const QString &key, const int index);
    __RESULT_IMPL redisLInsert(const QString &key,
                               const QString &pilot,
                               const QString &value,
                               const QString &insertFlag = QString("AFTER"));

    __RESULT_IMPL redisLLen(const QString &key);
    __RESULT_IMPL redisLPop(const QString &key);
    __RESULT_IMPL redisLPush(const QString &key, const QStringList &valueList);
    __RESULT_IMPL redisLPushX(const QString &key, const QString &value);
    __RESULT_IMPL redisLRange(const QString &key, const int start, const int stop);
    __RESULT_IMPL redisLRem(const QString &key, const QString &value, const int count);
    __RESULT_IMPL redisLSet(const QString &key, const QString &value, const int index);
    __RESULT_IMPL redisLTrim(const QString &key, const int start, const int stop);
    __RESULT_IMPL redisRPop(const QString &key);
    __RESULT_IMPL redisRPopLPush(const QString &sourceKey, const QString &destKey);
    __RESULT_IMPL redisRPush(const QString &key, const QStringList &valueList);
    __RESULT_IMPL redisRPushX(const QString &key, const QString &value);

    // ------------------------------------------------------------------------
    // -- STORED COMMANDS -----------------------------------------------------
    // ------------------------------------------------------------------------
    __RESULT_IMPL redisSAdd(const QString &key, const QStringList &memberList);
    __RESULT_IMPL redisSCard(const QString &key);
    __RESULT_IMPL redisSDiff(const QStringList &keyList);
    __RESULT_IMPL redisSDiffStore(const QString &dest, const QStringList &keyList);
    __RESULT_IMPL redisSInter(const QStringList &keyList);
    __RESULT_IMPL redisSInterStore(const QString &dest, const QStringList &keyList);
    __RESULT_IMPL redisSIsMember(const QString &key, const QString &member);
    __RESULT_IMPL redisSMembers(const QString &key);
    __RESULT_IMPL redisSMove(const QString &sourceKey, const QString &destKey, const QString &member);
    __RESULT_IMPL redisSPop(const QString &key, const uint count = 1);
    __RESULT_IMPL redisSRandMember(const QString &key, const int count = 1);
    __RESULT_IMPL redisSRem(const QString &key, const QStringList &memberList);
    __RESULT_IMPL redisSUnion(const QStringList &keyList);
    __RESULT_IMPL redisSUnionStore(const QString &dest, const QStringList &keyList);

    // ------------------------------------------------------------------------
    // -- SORTED STORED COMMANDS ----------------------------------------------
    // ------------------------------------------------------------------------
    __RESULT_IMPL redisZAdd(const QString &key,
                            const QMultiMap<QString, QString> scoreMember,
                            const QString &updFlag = QString(),
                            const bool chFlag = false,
                            const bool incrFlag = false);

    __RESULT_IMPL redisZCard(const QString &key);
    __RESULT_IMPL redisZCount(const QString &key,
                              const QVariant &min = QVariant(),
                              const QVariant &max = QVariant());

    __RESULT_IMPL redisZIncrBy(const QString &key, const QString &member, const qint64 incr);

    __RESULT_IMPL redisZInterStore(const QString &destKey,
                                   const QStringList &keyList,
                                   const QList<int> &weightList = QList<int>(),
                                   const QString &aggregateFlag = QString());

    __RESULT_IMPL redisZLexCount(const QString &key, const QString &min, const QString &max);

    __RESULT_IMPL redisZRange(const QString &key,
                              const int start,
                              const int stop,
                              const bool withScores = false);

    __RESULT_IMPL redisZRangeByLex(const QString &key,
                                   const QString &min,
                                   const QString &max,
                                   const int offset = -1,
                                   const int count = -1);

    __RESULT_IMPL redisZRangeByScore(const QString &key,
                                     const QString &min,
                                     const QString &max,
                                     const bool withScores = false,
                                     const int offset = -1,
                                     const int count = -1);

    __RESULT_IMPL redisZRank(const QString &key, const QString &member);
    __RESULT_IMPL redisZRem(const QString &key, const QStringList &members);
    __RESULT_IMPL redisZRemRangeByLex(const QString &key, const QString &min, const QString &max);
    __RESULT_IMPL redisZRemRangeByRank(const QString &key, const int start, const int stop);
    __RESULT_IMPL redisZRemRangeByScore(const QString &key, const QString &min, const QString &max);

    __RESULT_IMPL redisZRevRange(const QString &key,
                                 const int start,
                                 const int stop,
                                 const bool withScores = false);

    __RESULT_IMPL redisZRevRangeByLex(const QString &key,
                                      const QString &max,
                                      const QString &min,
                                      const int offset = -1,
                                      const int count = -1);

    __RESULT_IMPL redisZRevRangeByScore(const QString &key,
                                        const QString &max,
                                        const QString &min,
                                        const bool withScores = false,
                                        const int offset = -1,
                                        const int count = -1);

    __RESULT_IMPL redisZRevRank(const QString &key, const QString &member);
    __RESULT_IMPL redisZScore(const QString &key, const QString &member);

    __RESULT_IMPL redisZUnionStore(const QString &destKey,
                                   const QStringList &keyList,
                                   const QList<int> &weightList = QList<int>(),
                                   const QString &aggregateFlag = QString());

protected:
    void setLastError_safe(const QString &error);
    void clearLastError_safe();

    mutable QMutex  _mutex;  //!< мьютекс

private:
    template <typename T = __RESULT_IMPL>
    typename std::enable_if<std::is_same<T, bool>::value, bool>::type
    make_error(const QString &error) {
        this->setLastError_safe(error);
        return false;
    }

    template <typename T = __RESULT_IMPL>
    typename std::enable_if<!std::is_same<T, bool>::value, __RESULT_IMPL>::type
    make_error(const QString &error) {
        this->setLastError_safe(error);
        return __RESULT_IMPL();
    }

    __CLIENT_IMPL *as_CLIENT_IMPL_ptr();

    mutable QMutex  _mutexErr;  //!< мьютекс
    QString         _lastError; //!< ошибка
};


// ------------------------------------------------------------------------
// -- ERRORS FUNCTIONS ----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Задана ли ошибка
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
bool QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::hasLastError() const
{
    QMutexLocker lock(&_mutexErr);
    return !_lastError.isEmpty();
}

//!
//! \brief Последняя ошибка
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
const QString &QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::lastError() const
{
    QMutexLocker lock(&_mutexErr);
    return _lastError;
}


// ------------------------------------------------------------------------
// -- BASE COMMANDS -------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Выпонить команду
//! \param command Команда
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QString &command)
{
    QMutexLocker lock(&_mutex);
    if (command.isEmpty())
        return make_error("Command is Empty!");

    return as_CLIENT_IMPL_ptr()->processCommand(QtRedisCommand::fromString(command));
}

//!
//! \brief Выпонить команду
//! \param command Команда
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QByteArray &command)
{
    QMutexLocker lock(&_mutex);
    if (command.isEmpty())
        return make_error("Command is Empty!");

    return as_CLIENT_IMPL_ptr()->processCommand(QtRedisCommand::fromByteArray(command));
}

//!
//! \brief Выполнить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommandArgv(const QStringList &commandArgv)
{
    QMutexLocker lock(&_mutex);
    if (commandArgv.isEmpty())
        return make_error("CommandList is Empty!");

    const QByteArray cmd = commandArgv.first().toUtf8();
    QList<QByteArray> cmdArgv;
    for (int i = 1; i < commandArgv.size(); i++)
        cmdArgv.append(commandArgv.at(i).toUtf8());
    return as_CLIENT_IMPL_ptr()->processCommand(QtRedisCommand(cmd, cmdArgv));
}

//!
//! \brief Выполнить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommandArgv(const QList<QByteArray> &commandArgv)
{
    QMutexLocker lock(&_mutex);
    if (commandArgv.isEmpty())
        return make_error("CommandList is Empty!");

    const QByteArray cmd = commandArgv.first();
    QList<QByteArray> cmdArgv;
    for (int i = 1; i < commandArgv.size(); i++)
        cmdArgv.append(commandArgv.at(i));
    return as_CLIENT_IMPL_ptr()->processCommand(QtRedisCommand(cmd, cmdArgv));
}


// ------------------------------------------------------------------------
// -- KEY-VALUE COMMANDS --------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Список ключей в БД
//! \param arg Условие поиска
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisKeys(const QString &arg)
{
    return this->redisExecCommand(QString("KEYS %1").arg(arg));
}

//!
//! \brief Случайный ключ из текущей БД
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRandomKey()
{
    return this->redisExecCommand(QString("RANDOMKEY"));
}

//!
//! \brief Проверка наличия ключей в БД
//! \param keyList Список ключей
//! \return
//!
//! Since Redis 3.0.3 it is possible to specify multiple keys instead of a single one. In such a case, it returns the total number of keys existing.
//! Note that returning 1 or 0 for a single key is just a special case of the variadic usage, so the command is completely backward compatible.
//!
//! The user should be aware that if the same existing key is mentioned in the arguments multiple times, it will be counted multiple times.
//! So if somekey exists, EXISTS somekey somekey will return 2.
//!
//! Integer reply, specifically:
//! - 1 if the key exists.
//! - 0 if the key does not exist.
//!
//! Since Redis 3.0.3 the command accepts a variable number of keys and the return value is generalized:
//! - The number of keys existing among the ones specified as arguments. Keys mentioned multiple times and existing are counted multiple times.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExists(const QStringList &keyList)
{
    if (keyList.isEmpty())
        return make_error("Invalid key list (Empty)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("EXISTS %1").arg(keyList.join(" ")).trimmed());
}

//!
//! \brief Получить значение по ключу
//! \param key Ключ
//! \return
//!
//! Get the value of key. If the key does not exist the special value nil is returned.
//! An error is returned if the value stored at key is not a string, because GET only handles string values.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisGet(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("GET %1").arg(key));
}

//!
//! \brief Получить подстроку значения в указанных диапазонах
//! \param key Ключ
//! \param startPos Начало
//! \param endPos Конец
//! \return
//!
//! Warning: this command was renamed to GETRANGE, it is called SUBSTR in Redis versions <= 2.0.
//! Returns the substring of the string value stored at key, determined by the offsets start and end (both are inclusive).
//! Negative offsets can be used in order to provide an offset starting from the end of the string. So -1 means the last character, -2 the penultimate and so forth.
//! The function handles out of range requests by limiting the resulting range to the actual length of the string.
//! Examples:
//! redis>  SET mykey "This is a string"
//! "OK"
//! redis>  GETRANGE mykey 0 3
//! "This"
//! redis>  GETRANGE mykey -3 -1
//! "ing"
//! redis>  GETRANGE mykey 0 -1
//! "This is a string"
//! redis>  GETRANGE mykey 10 100
//! "string"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisGetRange(const QString &key, const int startPos, const int endPos)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("GETRANGE %1 %2 %3").arg(key).arg(startPos).arg(endPos));
}

//!
//! \brief Получить значение ключа и задать новое
//! \param key Ключ
//! \param value Новое значение
//! \return
//!
//! Atomically sets key to value and returns the old value stored at key. Returns an error when key exists but does not hold a string value.
//!
//! GETSET can be used together with INCR for counting with atomic reset. For example: a process may call INCR against the key mycounter every time some event occurs,
//! but from time to time we need to get the value of the counter and reset it to zero atomically.
//! This can be done using GETSET mycounter "0":
//! redis>  INCR mycounter
//! (integer) 1
//! redis>  GETSET mycounter "0"
//! "1"
//! redis>  GET mycounter
//! "0"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisGetSet(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "GETSET" << key << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Добавить/задать значение ключу
//! \param key Ключ
//! \param appendValue Значение
//! \return
//!
//! If key already exists and is a string, this command appends the value at the end of the string.
//! If key does not exist it is created and set as an empty string, so APPEND will be similar to SET in this special case.
//!
//! Examples
//! redis>  EXISTS mykey
//! (integer) 0
//! redis>  APPEND mykey "Hello"
//! (integer) 5
//! redis>  APPEND mykey " World"
//! (integer) 11
//! redis>  GET mykey
//! "Hello World"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisAppend(const QString &key, const QString &appendValue)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "APPEND" << key << appendValue;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Задать значение ключу
//! \param key Ключ
//! \param value Значение
//! \param exSec Время "жизни" объекта в сек.
//! \param pxMSec Время "жизни" объекта в мсек.
//! \param existFlag Флаг проверки существования объекта (NX|XX)
//! \return
//!
//! Если заданы exSec и(или) pxMSec, то по истечении времени объект будет автоматически удален.
//!
//! Set key to hold the string value. If key already holds a value, it is overwritten, regardless of its type. Any previous time to live associated with the key is discarded on successful SET operation.
//! Options
//!
//! Starting with Redis 2.6.12 SET supports a set of options that modify its behavior:
//! - EX seconds -- Set the specified expire time, in seconds.
//! - PX milliseconds -- Set the specified expire time, in milliseconds.
//! - NX -- Only set the key if it does not already exist.
//! - XX -- Only set the key if it already exist.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSet(const QString &key, const QString &value, const uint exSec, const uint pxMSec, const QString existFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (!existFlag.isEmpty()
        && existFlag.toUpper() != QString("NX")
        && existFlag.toUpper() != QString("XX"))
        return make_error("Invalid existFlag!");

    QStringList argv;
    argv << "SET" << key << value;
    if (exSec > 0)
        argv << "EX" << QString::number(exSec);
    if (pxMSec > 0)
        argv << "PX" << QString::number(pxMSec);
    if (!existFlag.isEmpty())
        argv << existFlag.toUpper();

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Перезаписать часть строки значения на новое, начиная с позиции
//! \param key Ключ
//! \param value Новое значение
//! \param offset Позиция
//! \return
//!
//! Overwrites part of the string stored at key, starting at the specified offset, for the entire length of value.
//! If the offset is larger than the current length of the string at key, the string is padded with zero-bytes to make offset fit.
//! Non-existing keys are considered as empty strings, so this command will make sure it holds a string large enough to be able to set value at offset.
//!
//! Note that the maximum offset that you can set is 229 -1 (536870911), as Redis Strings are limited to 512 megabytes.
//! If you need to grow beyond this size, you can use multiple keys.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSetRange(const QString &key, const QString &value, const int offset)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "SETRANGE" << key << QString::number(offset) << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Удалить ключи и их значения из БД
//! \param keyList Список ключей
//! \return
//!
//! Removes the specified keys. A key is ignored if it does not exist.
//! Return value: The number of keys that were removed.
//! Examples
//! redis>  SET key1 "Hello"
//! "OK"
//! redis>  SET key2 "World"
//! "OK"
//! redis>  DEL key1 key2 key3
//! (integer) 2
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisDel(const QStringList &keyList)
{
    if (keyList.isEmpty())
        return make_error("Invalid key list (Empty)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("DEL %1").arg(keyList.join(" ")).trimmed());
}

//!
//! \brief Получить длину строки значения
//! \param key Ключ
//! \return
//!
//! Returns the length of the string value stored at key. An error is returned when key holds a non-string value.
//! Return value: the length of the string at key, or 0 when key does not exist.
//! Examples
//! redis>  SET mykey "Hello world"
//! "OK"
//! redis>  STRLEN mykey
//! (integer) 11
//! redis>  STRLEN nonexisting
//! (integer) 0
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisStrlen(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("STRLEN %1").arg(key));
}

//!
//! \brief Задать время "жизни" объекта в сек.
//! \param key Ключ
//! \param sec Время в сек.
//! \return
//!
//! Set a timeout on key. After the timeout has expired, the key will automatically be deleted. A key with an associated timeout is often said to be volatile in Redis terminology.
//!
//! The timeout will only be cleared by commands that delete or overwrite the contents of the key, including DEL, SET, GETSET and all the *STORE commands.
//! This means that all the operations that conceptually alter the value stored at the key without replacing it with a new one will leave the timeout untouched.
//! For instance, incrementing the value of a key with INCR, pushing a new value into a list with LPUSH, or altering the field value of a hash with HSET are all operations
//! that will leave the timeout untouched.
//!
//! The timeout can also be cleared, turning the key back into a persistent key, using the PERSIST command.
//! If a key is renamed with RENAME, the associated time to live is transferred to the new key name.
//! If a key is overwritten by RENAME, like in the case of an existing key Key_A that is overwritten by a call like RENAME Key_B Key_A,
//! it does not matter if the original Key_A had a timeout associated or not, the new key Key_A will inherit all the characteristics of Key_B.
//!
//! Refreshing expires
//! It is possible to call EXPIRE using as argument a key that already has an existing expire set. In this case the time to live of a key is updated to the new value.
//! There are many useful applications for this, an example is documented in the Navigation session pattern section below.
//!
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXPIRE mykey 10
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 10
//! redis>  SET mykey "Hello World"
//! "OK"
//! redis>  TTL mykey
//! (integer) -1
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExpire(const QString &key, const uint sec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (sec == 0)
        return make_error("Invalid sec!");

    return this->redisExecCommand(QString("EXPIRE %1 %2").arg(key).arg(sec));
}

//!
//! \brief Задать время "жизни" объекта в формате utc (сек)
//! \param key Ключ
//! \param utcSec Время в формате utc (сек)
//! \return
//!
//! EXPIREAT has the same effect and semantic as EXPIRE, but instead of specifying the number of seconds representing the TTL (time to live),
//! it takes an absolute Unix timestamp (seconds since January 1, 1970). A timestamp in the past will delete the key immediately.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXISTS mykey
//! (integer) 1
//! redis>  EXPIREAT mykey 1293840000
//! (integer) 1
//! redis>  EXISTS mykey
//! (integer) 0
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExpireAt(const QString &key, const uint utcSec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (utcSec == 0)
        return make_error("Invalid utcSec!");

    return this->redisExecCommand(QString("EXPIREAT %1 %2").arg(key).arg(utcSec));
}

//!
//! \brief Задать время "жизни" объекта в мсек.
//! \param key Ключ
//! \param msec Время в мсек
//! \return
//!
//! This command works exactly like EXPIRE but the time to live of the key is specified in milliseconds instead of seconds.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  PEXPIRE mykey 1500
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 1
//! redis>  PTTL mykey
//! (integer) 1494
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisPExpire(const QString &key, const uint msec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("PEXPIRE %1 %2").arg(key).arg(msec));
}

//!
//! \brief Задать время "жизни" объекта в формате utc (мсек)
//! \param key Ключ
//! \param utcMsec Время в формате utc (мсек)
//! \return
//!
//! PEXPIREAT has the same effect and semantic as EXPIREAT, but the Unix time at which the key will expire is specified in milliseconds instead of seconds.
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  PEXPIREAT mykey 1555555555005
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 47755408
//! redis>  PTTL mykey
//! (integer) 47755408215
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisPExpireAt(const QString &key, const qint64 utcMsec)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("PEXPIREAT %1 %2").arg(key).arg(utcMsec));
}

//!
//! \brief Удалить заданное время "жизни" объекта
//! \param key Ключ
//! \return
//!
//! Remove the existing timeout on key, turning the key from volatile (a key with an expire set) to persistent (a key that will never expire as no timeout is associated).
//! Examples
//! redis>  SET mykey "Hello"
//! "OK"
//! redis>  EXPIRE mykey 10
//! (integer) 1
//! redis>  TTL mykey
//! (integer) 10
//! redis>  PERSIST mykey
//! (integer) 1
//! redis>  TTL mykey
//! (integer) -1
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisPersist(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("PERSIST %1").arg(key));
}

//!
//! \brief Получить время "жизни" объекта в сек.
//! \param key Ключ
//! \return
//!
//! Returns the remaining time to live of a key that has a timeout. This introspection capability allows a Redis client to check how many seconds a given key will continue
//! to be part of the dataset.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//! - The command returns -2 if the key does not exist.
//! - The command returns -1 if the key exists but has no associated expire.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisTtl(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("TTL %1").arg(key));
}

//!
//! \brief Получить время "жизни" объекта в мсек.
//! \param key Ключ
//! \return
//!
//! Like TTL this command returns the remaining time to live of a key that has an expire set, with the sole difference that TTL returns the amount of remaining time
//! in seconds while PTTL returns it in milliseconds.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//! - The command returns -2 if the key does not exist.
//! - The command returns -1 if the key exists but has no associated expire.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisPTtl(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("PTTL %1").arg(key));
}

//!
//! \brief Уменьшить число, сохраненное в ключе на один
//! \param key Ключ
//! \return
//!
//! Decrements the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the decrement
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisDecr(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("DECR %1").arg(key));
}

//!
//! \brief Уменьшить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param decr Значение
//! \return
//!
//! Decrements the number stored at key by decrement. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the decrement
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisDecrBy(const QString &key, const qint64 decr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("DECRBY %1 %2").arg(key).arg(decr));
}

//!
//! \brief Увеличить число, сохраненное в ключе на один
//! \param key Ключ
//! \return
//!
//! Increments the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Note: this is a string operation because Redis does not have a dedicated integer type.
//!       The string stored at the key is interpreted as a base-10 64 bit signed integer to execute the operation.
//!
//! Redis stores integers in their integer representation, so for string values that actually hold an integer,
//! there is no overhead for storing the string representation of the integer.
//!
//! Return value: the value of key after the increment
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisIncr(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("INCR %1").arg(key));
}

//!
//! \brief Увеличить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param incr Значение
//! \return
//!
//! Increments the number stored at key by increment. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Return value: the value of key after the increment
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisIncrBy(const QString &key, const qint64 incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("INCRBY %1 %2").arg(key).arg(incr));
}

//!
//! \brief Увеличить число, сохраненное в ключе на значение
//! \param key Ключ
//! \param incr Значение
//! \return
//!
//! Increment the string representing a floating point number stored at key by the specified increment.
//! By using a negative increment value, the result is that the value stored at the key is decremented (by the obvious properties of addition).
//! If the key does not exist, it is set to 0 before performing the operation. An error is returned if one of the following conditions occur:
//!
//! The key contains a value of the wrong type (not a string).
//! The current key content or the specified increment are not parsable as a double precision floating point number.
//!
//! If the command is successful the new incremented value is stored as the new value of the key (replacing the old one), and returned to the caller as a string.
//!
//! Both the value already contained in the string key and the increment argument can be optionally provided in exponential notation,
//! however the value computed after the increment is stored consistently in the same format, that is, an integer number followed (if needed) by a dot,
//! and a variable number of digits representing the decimal part of the number. Trailing zeroes are always removed.
//!
//! The precision of the output is fixed at 17 digits after the decimal point regardless of the actual internal precision of the computation.
//!
//! Return value: the value of key after the increment
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisIncrByFloat(const QString &key, const float incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("INCRBYFLOAT %1 %2").arg(key).arg(incr));
}

//!
//! \brief Переименовать ключ
//! \param key Ключ
//! \param newKey Новое имя ключа
//! \return
//!
//! Renames key to newkey. It returns an error when key does not exist. If newkey already exists it is overwritten, when this happens RENAME executes an implicit DEL operation,
//! so if the deleted key contains a very big value it may cause high latency even if RENAME itself is usually a constant-time operation.
//!
//! Note: Before Redis 3.2.0, an error is returned if source and destination names are the same.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRename(const QString &key, const QString &newKey)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (newKey.isEmpty() || newKey.indexOf(" ") != -1)
        return make_error("Invalid newKey!");

    return this->redisExecCommand(QString("RENAME %1 %2").arg(key, newKey));
}

//!
//! \brief Переименовать ключ, если новое имя не существует
//! \param key Ключ
//! \param newKey Новое имя ключа
//! \return
//!
//! Renames key to newkey if newkey does not yet exist. It returns an error when key does not exist.
//!
//! Note: Before Redis 3.2.0, an error is returned if source and destination names are the same.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRenameNx(const QString &key, const QString &newKey)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (newKey.isEmpty() || newKey.indexOf(" ") != -1)
        return make_error("Invalid newKey!");

    return this->redisExecCommand(QString("RENAMENX %1 %2").arg(key, newKey));
}

//!
//! \brief Получить тип значения ключа
//! \param key Ключ
//! \return
//!
//! Returns the string representation of the type of the value stored at key.
//! The different types that can be returned are: string, list, set, zset and hash.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisType(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("TYPE %1").arg(key));
}

//!
//! \brief Установить ключи в соответствии со значениями
//! \param keyValue Список ключ-значение
//! \return
//!
//! Sets the given keys to their respective values. MSET replaces existing values with new values, just as regular SET.
//! See MSETNX if you don't want to overwrite existing values.
//! MSET is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisMSet(const QMap<QString, QString> &keyValue)
{
    if (keyValue.isEmpty())
        return make_error("Invalid key-value (Empty)!");

    const QList<QString> keyList = keyValue.keys();
    for (const QString &key : keyList) {
        if (key.isEmpty() || key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    QStringList argv;
    argv << "MSET";
    QMapIterator<QString, QString> i (keyValue);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Установить ключи в соответствии со значениями, если ключи не существуют
//! \param keyValue Список ключ-значение
//! \return
//!
//! Sets the given keys to their respective values. MSETNX will not perform any operation at all even if just a single key already exists.
//! Because of this semantic MSETNX can be used in order to set different keys representing different fields of an unique logic object
//! in a way that ensures that either all the fields or none at all are set.
//!
//! MSETNX is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisMSetNx(const QMap<QString, QString> &keyValue)
{
    if (keyValue.isEmpty())
        return make_error("Invalid key-value (Empty)!");

    const QList<QString> keyList = keyValue.keys();
    for (const QString &key : keyList) {
        if (key.isEmpty() || key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    QStringList argv;
    argv << "MSETNX";
    QMapIterator<QString, QString> i (keyValue);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить значения ключей
//! \param keyList Список ключей
//! \return
//!
//! Returns the values of all specified keys. For every key that does not hold a string value or does not exist, the special value nil is returned.
//! Because of this, the operation never fails.
//! Examples
//! redis>  SET key1 "Hello"
//! "OK"
//! redis>  SET key2 "World"
//! "OK"
//! redis>  MGET key1 key2 nonexisting
//! 1) "Hello"
//! 2) "World"
//! 3) (nil)
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisMGet(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("MGET %1").arg(keyList.join(" ")));
}

//!
//! \brief Переместить ключ с его значениями в другую БД
//! \param key Ключ
//! \param dbIndex Индекс БД
//! \return
//!
//! Move key from the currently selected database (see SELECT) to the specified destination database.
//! When key already exists in the destination database, or it does not exist in the source database, it does nothing.
//! It is possible to use MOVE as a locking primitive because of this.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisMove(const QString &key, const int dbIndex)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (dbIndex < 0)
        return make_error("Invalid db index!");

    return this->redisExecCommand(QString("MOVE %1 %2").arg(key).arg(dbIndex));
}

//!
//! \brief Преобразовать значение ключа в формат Redis-а
//! \param key Ключ
//! \return
//!
//! Для получения корректного значения пользуйтесь QtRedisReply::rawValue().
//!
//! Serialize the value stored at key in a Redis-specific format and return it to the user. The returned value can be synthesized back into a Redis key using the RESTORE command.
//!
//! The serialization format is opaque and non-standard, however it has a few semantic characteristics:
//!
//! - It contains a 64-bit checksum that is used to make sure errors will be detected. The RESTORE command makes sure to check the checksum before synthesizing a key using the serialized value.
//! - Values are encoded in the same format used by RDB.
//! - An RDB version is encoded inside the serialized value, so that different Redis versions with incompatible RDB formats will refuse to process the serialized value.
//!
//! The serialized value does NOT contain expire information. In order to capture the time to live of the current value the PTTL command should be used.
//!
//! If key does not exist a nil bulk reply is returned.
//! Return value: the serialized value.
//!
//! Examples
//! redis>  SET mykey 10
//! "OK"
//! redis>  DUMP mykey
//! "\u0000\xC0\n\b\u0000ײ\xBB\xFA\xA7\xB7\xE9\x83"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisDump(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("DUMP %1").arg(key));
}


// ------------------------------------------------------------------------
// -- LIST COMMANDS -------------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Получить элменет списка по индексу
//! \param key Ключ
//! \param index Индекс
//! \return
//!
//! Returns the element at index index in the list stored at key. The index is zero-based, so 0 means the first element,
//! 1 the second element and so on. Negative indices can be used to designate elements starting at the tail of the list.
//! Here, -1 means the last element, -2 means the penultimate and so forth.
//!
//! When the value at key is not a list, an error is returned.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLIndex(const QString &key, const int index)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("LINDEX %1 %2").arg(key).arg(index));
}

//!
//! \brief Вставить значение (value) в список до или после значения (pilot)
//! \param key Ключ
//! \param pilot Значение поиска
//! \param value Новое значение
//! \param insertFlag Тип вставки (BEFORE|AFTER)
//! \return
//!
//! Inserts value in the list stored at key either before or after the reference value pivot.
//! When key does not exist, it is considered an empty list and no operation is performed.
//! An error is returned when key exists but does not hold a list value.
//! Return value: the length of the list after the insert operation, or -1 when the value pivot was not found.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLInsert(const QString &key, const QString &pilot, const QString &value, const QString &insertFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (insertFlag.toUpper() != QString("BEFORE")
        && insertFlag.toUpper() != QString("AFTER"))
        return make_error("Invalid insertFlag!");

    QStringList argv;
    argv << "LINSERT" << key << insertFlag.toUpper() << pilot << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить размер списка
//! \param key Ключ
//! \return
//!
//! Returns the length of the list stored at key. If key does not exist, it is interpreted as an empty list and 0 is returned.
//! An error is returned when the value stored at key is not a list.
//! Return value: the length of the list at key.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLLen(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("LLEN %1").arg(key));
}

//!
//! \brief Удалить и вернуть первый элменет списка
//! \param key Ключ
//! \return
//!
//! Removes and returns the first element of the list stored at key.
//! Return value: the value of the first element, or nil when key does not exist.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLPop(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("LPOP %1").arg(key));
}

//!
//! \brief Вставить значения в начало списка
//! \param key Ключ
//! \param valueList Значения
//! \return
//!
//! Insert all the specified values at the head of the list stored at key. If key does not exist, it is created as empty list before performing the push operations.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the head of the list, from the leftmost element to the rightmost element.
//! So for instance the command LPUSH mylist a b c will result into a list containing c as first element, b as second element and a as third element.
//! Return value: the length of the list after the push operations.
//!
//! Redis >= 2.4: Accepts multiple value arguments. In Redis versions older than 2.4 it was possible to push a single value per command.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLPush(const QString &key, const QStringList &valueList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (valueList.isEmpty())
        return make_error("Invalid valueList (Empty)!");

    QStringList argv;
    argv << "LPUSH" << key;
    for (const QString &str : valueList)
        argv << str;

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вставить значение в начало списка, если ключ существует и он содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Inserts value at the head of the list stored at key, only if key already exists and holds a list. In contrary to LPUSH, no operation will be performed when key does not yet exist.
//! Return value: the length of the list after the push operation.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LPUSHX" << key << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить список значений ключа
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \return
//!
//! Returns the specified elements of the list stored at key. The offsets start and stop are zero-based indexes, with 0 being the first element of the list (the head of the list),
//! 1 being the next element and so on.
//!
//! These offsets can also be negative numbers indicating offsets starting at the end of the list. For example, -1 is the last element of the list, -2 the penultimate, and so on.
//!
//! Consistency with range functions in various programming languages:
//! Note that if you have a list of numbers from 0 to 100, LRANGE list 0 10 will return 11 elements, that is, the rightmost item is included.
//! This may or may not be consistent with behavior of range-related functions in your programming language of choice (think Ruby's Range.new, Array#slice or Python's range() function).
//!
//! Out-of-range indexes:
//! Out of range indexes will not produce an error. If start is larger than the end of the list, an empty list is returned.
//! If stop is larger than the actual end of the list, Redis will treat it like the last element of the list.
//!
//! Return value: list of elements in the specified range.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLRange(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("LRANGE %1 %2 %3").arg(key).arg(start).arg(stop));
}

//!
//! \brief Удалить первое найденное значение из списка начиная с заданного индекса
//! \param key Ключ
//! \param value Значение
//! \param count Индекс
//! \return
//!
//! Removes the first count occurrences of elements equal to value from the list stored at key. The count argument influences the operation in the following ways:
//! - count > 0: Remove elements equal to value moving from head to tail.
//! - count < 0: Remove elements equal to value moving from tail to head.
//! - count = 0: Remove all elements equal to value.
//!
//! For example, LREM list -2 "hello" will remove the last two occurrences of "hello" in the list stored at list.
//!
//! Note that non-existing keys are treated like empty lists, so when key does not exist, the command will always return 0.
//! Return value: the number of removed elements.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLRem(const QString &key, const QString &value, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LREM" << key << QString::number(count) << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Установить новое знаяение в списке по индексу
//! \param key Ключ
//! \param value Новое значение
//! \param index Индекс
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLSet(const QString &key, const QString &value, const int index)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LSET" << key << QString::number(index) << value;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Обрезать список, чтобы он содержал тоько указанный диапазон элементов
//! \param key Ключ
//! \param start Начало диапазона
//! \param stop Конец диапазона
//! \return
//!
//! Trim an existing list so that it will contain only the specified range of elements specified. Both start and stop are zero-based indexes,
//! where 0 is the first element of the list (the head), 1 the next element and so on.
//!
//! For example: LTRIM foobar 0 2 will modify the list stored at foobar so that only the first three elements of the list will remain.
//!
//! start and end can also be negative numbers indicating offsets from the end of the list, where -1 is the last element of the list, -2 the penultimate element and so on.
//!
//! Out of range indexes will not produce an error: if start is larger than the end of the list, or start > end, the result will be an empty list (which causes key to be removed).
//! If end is larger than the end of the list, Redis will treat it like the last element of the list.
//!
//! A common use of LTRIM is together with LPUSH / RPUSH. For example:
//! LPUSH mylist someelement
//! LTRIM mylist 0 99
//!
//! This pair of commands will push a new element on the list, while making sure that the list will not grow larger than 100 elements.
//! This is very useful when using Redis to store logs for example. It is important to note that when used in this way LTRIM is an O(1) operation because in the average case
//! just one element is removed from the tail of the list.
//!
//! Examples
//! redis>  RPUSH mylist "one"
//! (integer) 1
//! redis>  RPUSH mylist "two"
//! (integer) 2
//! redis>  RPUSH mylist "three"
//! (integer) 3
//! redis>  LTRIM mylist 1 -1
//! "OK"
//! redis>  LRANGE mylist 0 -1
//! 1) "two"
//! 2) "three"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLTrim(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("LTRIM %1 %2 %3").arg(key).arg(start).arg(stop));
}

//!
//! \brief Удалить и получить последний элемент списка, хранящегося в ключе
//! \param key Ключ
//! \return
//!
//! Removes and returns the last element of the list stored at key.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRPop(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("RPOP %1").arg(key));
}

//!
//! \brief Удалить и получить последний элемент списка (sourceKey), и поместить его в начало списка (destKey)
//! \param sourceKey Ключ-ичтоник
//! \param destKey Ключ-получатель
//! \return
//!
//! Atomically returns and removes the last element (tail) of the list stored at source, and pushes the element at the first element (head) of the list stored at destination.
//!
//! For example: consider source holding the list a,b,c, and destination holding the list x,y,z. Executing RPOPLPUSH results in source holding a,b and destination holding c,x,y,z.
//!
//! If source does not exist, the value nil is returned and no operation is performed. If source and destination are the same,
//! the operation is equivalent to removing the last element from the list and pushing it as first element of the list, so it can be considered as a list rotation command.
//!
//! Return value: the element being popped and pushed.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRPopLPush(const QString &sourceKey, const QString &destKey)
{
    if (sourceKey.isEmpty() || sourceKey.indexOf(" ") != -1)
        return make_error("Invalid sourceKey!");

    if (destKey.isEmpty() || destKey.indexOf(" ") != -1)
        return make_error("Invalid destKey!");

    return this->redisExecCommand(QString("RPOPLPUSH %1 %2").arg(sourceKey, destKey));
}

//!
//! \brief Вставить значения в конец списка
//! \param key Ключ
//! \param valueList Значения
//! \return
//!
//! Insert all the specified values at the tail of the list stored at key. If key does not exist, it is created as empty list before performing the push operation.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the tail of the list, from the leftmost element to the rightmost element.
//! So for instance the command RPUSH mylist a b c will result into a list containing a as first element, b as second element and c as third element.
//!
//! Return value: the length of the list after the push operation.
//! Redis >= 2.4: Accepts multiple value arguments. In Redis versions older than 2.4 it was possible to push a single value per command.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRPush(const QString &key, const QStringList &valueList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (valueList.isEmpty())
        return make_error("Invalid valueList (Empty)!");

    QStringList argv;
    argv << "RPUSH" << key;
    for (const QString &str : valueList)
        argv << str;

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вставить значение в конец списка, если ключ существует и содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Inserts value at the tail of the list stored at key, only if key already exists and holds a list.
//! In contrary to RPUSH, no operation will be performed when key does not yet exist.
//! Return value: the length of the list after the push operation.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "RPUSHX" << key << value;
    return this->redisExecCommandArgv(argv);
}


// ------------------------------------------------------------------------
// -- STORED COMMANDS -----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Добавить значения в набор
//! \param key Ключ
//! \param memberList Значения
//! \return
//!
//! Если одно или несколько значений уже есть в наборе, то они не добавляются.
//!
//! Add the specified members to the set stored at key. Specified members that are already a member of this set are ignored.
//! If key does not exist, a new set is created before adding the specified members.
//!
//! An error is returned when the value stored at key is not a set.
//! Return value: the number of elements that were added to the set, not including all the elements already present into the set.
//! Redis >= 2.4: Accepts multiple member arguments. Redis versions before 2.4 are only able to add a single member per call.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSAdd(const QString &key, const QStringList &memberList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (memberList.isEmpty())
        return make_error("Invalid memberList (Empty)!");

    QStringList argv;
    argv << "SADD" << key;
    for (const QString &str : memberList)
        argv << str;

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить количество элементов в наборе
//! \param key Ключ
//! \return
//!
//! Returns the set cardinality (number of elements) of the set stored at key.
//! Return value: the cardinality (number of elements) of the set, or 0 if key does not exist.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSCard(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("SCARD %1").arg(key));
}

//!
//! \brief Получить список элементов, полученный путем разницы между первым набором(keyList[0]) и остальными(keyList[1..N])
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the difference between the first set and all the successive sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SDIFF key1 key2 key3 = {b,d}
//!
//! Keys that do not exist are considered to be empty sets.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSDiff(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SDIFF %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученный путем разницы между первым набором(keyList[0]) и остальными(keyList[1..N]), и записать результат в новый набор
//! \param dest Название нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SDIFF, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSDiffStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1)
        return make_error("Invalid dest!");

    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SDIFFSTORE %1 %2").arg(dest, keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученных путем пересечения наборов
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the intersection of all the given sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SINTER key1 key2 key3 = {c}
//!
//! Keys that do not exist are considered to be empty sets. With one of the keys being an empty set, the resulting set is also empty
//! (since set intersection with an empty set always results in an empty set).
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSInter(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SINTER %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученных путем пересечения наборов, и записать результат в новый набор
//! \param dest Название нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SINTER, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSInterStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1)
        return make_error("Invalid dest!");

    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SINTERSTORE %1 %2").arg(dest, keyList.join(" ")));
}

//!
//! \brief Входит ли элемент в набор
//! \param key Ключ набора
//! \param member Значение
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSIsMember(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "SISMEMBER" << key << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить список всех элементов набора
//! \param key Ключ набора
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSMembers(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("SMEMBERS %1").arg(key));
}

//!
//! \brief Переместить элемент из одного набора(sourceKey) в другой(destKey)
//! \param sourceKey Ключ набора источника
//! \param destKey Ключ набора получателя
//! \param member Значение
//! \return
//!
//! Move member from the set at source to the set at destination. This operation is atomic.
//! In every given moment the element will appear to be a member of source or destination for other clients.
//!
//! If the source set does not exist or does not contain the specified element, no operation is performed and 0 is returned.
//! Otherwise, the element is removed from the source set and added to the destination set.
//! When the specified element already exists in the destination set, it is only removed from the source set.
//!
//! An error is returned if source or destination does not hold a set value.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSMove(const QString &sourceKey, const QString &destKey, const QString &member)
{
    if (sourceKey.isEmpty() || sourceKey.indexOf(" ") != -1)
        return make_error("Invalid sourceKey!");

    if (destKey.isEmpty() || destKey.indexOf(" ") != -1)
        return make_error("Invalid destKey!");

    QStringList argv;
    argv << "SMOVE" << sourceKey << destKey << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Удалить и вернуть один или несколько случайных элментов в наборе
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! Removes and returns one or more random elements from the set value store at key.
//!
//! This operation is similar to SRANDMEMBER, that returns one or more random elements from a set but does not remove it.
//!
//! The count argument is available since version 3.2.
//! Return value: the removed element, or nil when key does not exist.
//! Examples
//! redis>  SADD myset "one"
//! (integer) 1
//! redis>  SADD myset "two"
//! (integer) 1
//! redis>  SADD myset "three"
//! (integer) 1
//! redis>  SPOP myset
//! "three"
//! redis>  SMEMBERS myset
//! 1) "two"
//! 2) "one"
//! redis>  SADD myset "four"
//! (integer) 1
//! redis>  SADD myset "five"
//! (integer) 1
//! redis>  SPOP myset 3
//! 1) "four"
//! 2) "two"
//! 3) "one"
//! redis>  SMEMBERS myset
//! 1) "five"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSPop(const QString &key, const uint count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "SPOP" << key;
    if (count > 1)
        argv << QString::number(count);

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить один или несколько случайных элементов из набора
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! When called with just the key argument, return a random element from the set value stored at key.
//!
//! Starting from Redis version 2.6, when called with the additional count argument, return an array of count distinct elements if count is positive.
//! If called with a negative count the behavior changes and the command is allowed to return the same element multiple times.
//! In this case the number of returned elements is the absolute value of the specified count.
//!
//! When called with just the key argument, the operation is similar to SPOP, however while SPOP also removes the randomly selected element from the set,
//! SRANDMEMBER will just return a random element without altering the original set in any way.
//! Return value:
//! - String reply: without the additional count argument the command returns a Bulk Reply with the randomly selected element, or nil when key does not exist.
//! - Array reply: when the additional count argument is passed the command returns an array of elements, or an empty array when key does not exist.
//!
//! Examples
//! redis>  SADD myset one two three
//! (integer) 3
//! redis>  SRANDMEMBER myset
//! "two"
//! redis>  SRANDMEMBER myset 2
//! 1) "two"
//! 2) "one"
//! redis>  SRANDMEMBER myset -5
//! 1) "two"
//! 2) "three"
//! 3) "two"
//! 4) "three"
//! 5) "three"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSRandMember(const QString &key, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (count == 0 || count == 1)
        return this->redisExecCommand(QString("SRANDMEMBER %1").arg(key));

    return this->redisExecCommand(QString("SRANDMEMBER %1 %2").arg(key).arg(count));
}

//!
//! \brief Удалить элемент(ы) из набора
//! \param key Ключ набора
//! \param memberList Список элементов
//! \return
//!
//! Remove the specified members from the set stored at key. Specified members that are not a member of this set are ignored.
//! If key does not exist, it is treated as an empty set and this command returns 0.
//!
//! An error is returned when the value stored at key is not a set.
//! Return value: the number of members that were removed from the set, not including non existing members.
//! Redis >= 2.4: Accepts multiple member arguments. Redis versions older than 2.4 can only remove a set member per call.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSRem(const QString &key, const QStringList &memberList)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (memberList.isEmpty())
        return make_error("Invalid memberList (Empty)!");

    QStringList argv;
    argv << "SREM" << key;
    for (const QString &str : memberList)
        argv << str;

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить список элементов, полученных путем объединения всех заданных наборов
//! \param keyList Список ключей наборов
//! \return
//!
//! Returns the members of the set resulting from the union of all the given sets.
//!
//! For example:
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SUNION key1 key2 key3 = {a,b,c,d,e}
//!
//! Keys that do not exist are considered to be empty sets.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSUnion(const QStringList &keyList)
{
    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SUNION %1").arg(keyList.join(" ")));
}

//!
//! \brief Получить список элементов, полученных путем объединения всех заданных наборов, и записать в новый набор (dest)
//! \param dest Название ключа нового набора
//! \param keyList Список ключей наборов
//! \return
//!
//! This command is equal to SUNION, but instead of returning the resulting set, it is stored in destination.
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting set.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSUnionStore(const QString &dest, const QStringList &keyList)
{
    if (dest.isEmpty() || dest.indexOf(" ") != -1)
        return make_error("Invalid dest!");

    if (keyList.isEmpty() || keyList.size() < 2)
        return make_error("Invalid keyList (Empty or size < 2)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    return this->redisExecCommand(QString("SUNIONSTORE %1 %2").arg(dest, keyList.join(" ")));
}


// ------------------------------------------------------------------------
// -- SORTED STORED COMMANDS ----------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Добавить элементы со значениями в отсортированный набор
//! \param key Ключ
//! \param scoreMember Список элементов со значениями
//! \param updFlag Флаг обновления (NX|XX)
//! \param chFlag Флаг модификации
//! \param incrFlag Флаг инкремента
//! \return
//!
//! Adds all the specified members with the specified scores to the sorted set stored at key. It is possible to specify multiple score / member pairs.
//! If a specified member is already a member of the sorted set, the score is updated and the element reinserted at the right position to ensure the correct ordering.
//!
//! If key does not exist, a new sorted set with the specified members as sole members is created, like if the sorted set was empty. If the key exists but does not hold a sorted set,
//! an error is returned.
//!
//! The score values should be the string representation of a double precision floating point number. +inf and -inf values are valid values as well.
//!
//! ZADD options (Redis 3.0.2 or greater):
//! ZADD supports a list of options, specified after the name of the key and before the first score argument. Options are:
//! - XX: Only update elements that already exist. Never add elements.
//! - NX: Don't update already existing elements. Always add new elements.
//! - CH: Modify the return value from the number of new elements added, to the total number of elements changed (CH is an abbreviation of changed). Changed elements are new elements added and elements already existing for which the score was updated. So elements specified in the command line having the same score as they had in the past are not counted. Note: normally the return value of ZADD only counts the number of new elements added.
//! - INCR: When this option is specified ZADD acts like ZINCRBY. Only one score-element pair can be specified in this mode.
//!
//! Range of integer scores that can be expressed precisely:
//! Redis sorted sets use a double 64-bit floating point number to represent the score. In all the architectures we support, this is represented as an IEEE 754 floating point number,
//! that is able to represent precisely integer numbers between -(2^53) and +(2^53) included. In more practical terms,
//! all the integers between -9007199254740992 and 9007199254740992 are perfectly representable. Larger integers, or fractions, are internally represented in exponential form,
//! so it is possible that you get only an approximation of the decimal number, or of the very big integer, that you set as score.
//!
//! Sorted sets 101:
//! Sorted sets are sorted by their score in an ascending way. The same element only exists a single time, no repeated elements are permitted.
//! The score can be modified both by ZADD that will update the element score, and as a side effect, its position on the sorted set,
//! and by ZINCRBY that can be used in order to update the score relatively to its previous value.
//!
//! The current score of an element can be retrieved using the ZSCORE command, that can also be used to verify if an element already exists or not.
//! For an introduction to sorted sets, see the data types page on sorted sets.
//!
//! Elements with the same score:
//! While the same element can't be repeated in a sorted set since every element is unique, it is possible to add multiple different elements having the same score.
//! When multiple elements have the same score, they are ordered lexicographically (they are still ordered by score as a first key, however, locally,
//! all the elements with the same score are relatively ordered lexicographically).
//!
//! The lexicographic ordering used is binary, it compares strings as array of bytes.
//! If the user inserts all the elements in a sorted set with the same score (for example 0), all the elements of the sorted set are sorted lexicographically,
//! and range queries on elements are possible using the command ZRANGEBYLEX (Note: it is also possible to query sorted sets by range of scores using ZRANGEBYSCORE).
//!
//! Return value:
//! - The number of elements added to the sorted sets, not including elements already existing for which the score was updated.
//! - If the INCR option is specified, the return value will be Bulk string reply: the new score of member (a double precision floating point number), represented as string.
//!
//! Redis >= 2.4: Accepts multiple elements. In Redis versions older than 2.4 it was possible to add or update a single member per call.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZAdd(const QString &key, const QMultiMap<QString, QString> scoreMember, const QString &updFlag, const bool chFlag, const bool incrFlag)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (scoreMember.isEmpty())
        return make_error("Invalid scoreMember (Empty)!");

    if (!updFlag.isEmpty()
        && updFlag.toUpper() != QString("NX")
        && updFlag.toUpper() != QString("XX"))
        return make_error("Invalid updFlag!");

    QStringList argv;
    argv << "ZADD" << key;
    if (!updFlag.isEmpty())
        argv << updFlag.toUpper();
    if (chFlag)
        argv << "CH";
    if (incrFlag)
        argv << "INCR";

    QMapIterator<QString, QString> i (scoreMember);
    while (i.hasNext()) {
        i.next();
        argv << i.key() << i.value();
    }
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить размер отсортированного набора
//! \param key Ключ
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZCard(const QString &key)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("ZCARD %1").arg(key));
}

//!
//! \brief Получить кличество элементов отсортированного набора, находящихся между min и max
//! \param key Ключ
//! \param min Мин.
//! \param max Макс.
//! \return
//!
//! Returns the number of elements in the sorted set at key with a score between min and max.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//!
//! For example:
//!
//! ZRANGEBYSCORE zset (1 5
//! Will return all elements with 1 < score <= 5 while:
//!
//! ZRANGEBYSCORE zset (5 (10
//! Will return all the elements with 5 < score < 10 (5 and 10 excluded).
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range. Because of this there is no need to do a work proportional to the size of the range.
//! Return value: the number of elements in the specified score range.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZCount(const QString &key, const QVariant &min, const QVariant &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.type() != QVariant::Invalid
        && min.type() != QVariant::Int
        && min.type() != QVariant::UInt
        && min.type() != QVariant::LongLong
        && min.type() != QVariant::ULongLong
        && min.type() != QVariant::Double)
        return make_error("Invalid min!");

    if (max.type() != QVariant::Invalid
        && max.type() != QVariant::Int
        && max.type() != QVariant::UInt
        && max.type() != QVariant::LongLong
        && max.type() != QVariant::ULongLong
        && max.type() != QVariant::Double)
        return make_error("Invalid max!");

    QString buffMin("-inf");
    if (min.type() != QVariant::Invalid)
        buffMin = min.toString();

    QString buffMax("+inf");
    if (max.type() != QVariant::Invalid)
        buffMax = max.toString();

    return this->redisExecCommand(QString("ZCOUNT %1 %2 %3").arg(key, buffMin, buffMax));
}

//!
//! \brief Увеличить значение отсортированного набора на величину incr
//! \param key Ключ
//! \param member Значение набора
//! \param incr Инкремент
//! \return
//!
//! Increments the score of member in the sorted set stored at key by increment. If member does not exist in the sorted set, it is added with increment as its score (as if its previous score was 0.0). If key does not exist, a new sorted set with the specified member as its sole member is created.
//!
//! An error is returned when key exists but does not hold a sorted set.
//!
//! The score value should be the string representation of a numeric value, and accepts double precision floating point numbers. It is possible to provide a negative value to decrement the score.
//! Return value: the new score of member (a double precision floating point number), represented as string.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZIncrBy(const QString &key, const QString &member, const qint64 incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZINCRBY" << key << QString::number(incr) << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вычислить пересечения отсортированных наборов и результат записать в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей
//! \param weightList Список значений
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Computes the intersection of numkeys sorted sets given by the specified keys, and stores the result in destination.
//! It is mandatory to provide the number of input keys (numkeys) before passing the input keys and the other (optional) arguments.
//!
//! By default, the resulting score of an element is the sum of its scores in the sorted sets where it exists.
//! Because intersection requires an element to be a member of every given sorted set, this results in the score of every element in the resulting sorted set to be equal
//! to the number of input sorted sets.
//!
//! For a description of the WEIGHTS and AGGREGATE options, see ZUNIONSTORE.
//!
//! If destination already exists, it is overwritten.
//! Return value: the number of elements in the resulting sorted set at destination.
//! Examples
//! redis>  ZADD zset1 1 "one"
//! (integer) 1
//! redis>  ZADD zset1 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 1 "one"
//! (integer) 1
//! redis>  ZADD zset2 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 3 "three"
//! (integer) 1
//! redis>  ZINTERSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 2
//! redis>  ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "two"
//! 4) "10"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZInterStore(const QString &destKey, const QStringList &keyList, const QList<int> &weightList, const QString &aggregateFlag)
{
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1)
        return make_error("Invalid destKey!");

    if (keyList.isEmpty())
        return make_error("Invalid keyList (Empty)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    QString weightListStr;
    for (const int weight : weightList) {
        if (weight <= 0)
            return __RESULT_IMPL();
        weightListStr += QString("%1 ").arg(weight);
    }
    if (!aggregateFlag.isEmpty()
        && aggregateFlag.toUpper() != QString("SUM")
        && aggregateFlag.toUpper() != QString("MIN")
        && aggregateFlag.toUpper() != QString("MAX")) {
        this->setLastError_safe("Invalid aggregateFlag!");
        return __RESULT_IMPL();
    }

    QString command = QString("ZINTERSTORE %1 %2 %3")
                      .arg(destKey)
                      .arg(keyList.size())
                      .arg(keyList.join(" "));

    if (!weightList.isEmpty())
        command += QString(" WEIGHTS %1").arg(weightListStr);
    if (!aggregateFlag.isEmpty())
        command += QString(" AGGREGATE %1").arg(aggregateFlag.toUpper());

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает количество элементов в отсортированном наборе между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering, this command returns the number of elements
//! in the sorted set at key with a value between min and max.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range.
//! Because of this there is no need to do a work proportional to the size of the range.
//! Return value: the number of elements in the specified score range.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZLexCount(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    return this->redisExecCommand(QString("ZLEXCOUNT %1 %2 %3").arg(key, min, max));
}

//!
//! \brief Получить диапазон элементов отсортированного набора
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \param withScores Если true - вернуть количество элементов вместе с элементами
//! \return
//!
//! Returns the specified range of elements in the sorted set stored at key. The elements are considered to be ordered from the lowest to the highest score.
//! Lexicographical order is used for elements with equal score.
//!
//! See ZREVRANGE when you need the elements ordered from highest to lowest score (and descending lexicographical order for elements with equal score).
//!
//! Both start and stop are zero-based indexes, where 0 is the first element, 1 is the next element and so on.
//! They can also be negative numbers indicating offsets from the end of the sorted set, with -1 being the last element of the sorted set, -2 the penultimate element and so on.
//!
//! start and stop are inclusive ranges, so for example ZRANGE myzset 0 1 will return both the first and the second element of the sorted set.
//!
//! Out of range indexes will not produce an error. If start is larger than the largest index in the sorted set, or start > stop, an empty list is returned.
//! If stop is larger than the end of the sorted set Redis will treat it like it is the last element of the sorted set.
//!
//! It is possible to pass the WITHSCORES option in order to return the scores of the elements together with the elements.
//! The returned list will contain value1,score1,...,valueN,scoreN instead of value1,...,valueN.
//! Client libraries are free to return a more appropriate data type (suggestion: an array with (value, score) arrays/tuples).
//! Return value: list of elements in the specified range (optionally with their scores, in case the WITHSCORES option is given).
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRange(const QString &key, const int start, const int stop, const bool withScores)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QString command = QString("ZREMRANGEBYRANK %1 %2 %3").arg(key).arg(start).arg(stop);
    if (withScores)
        command += QString(" WITHSCORES");

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает все элементы между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering, this command returns all the elements
//! in the sorted set at key with a value between min and max.
//!
//! If the elements in the sorted set have different scores, the returned elements are unspecified.
//!
//! The elements are considered to be ordered from lower to higher strings as compared byte-by-byte using the memcmp() C function.
//! Longer strings are considered greater than shorter strings if the common part is identical.
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! Keep in mind that if offset is large, the sorted set needs to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! How to specify intervals:
//! Valid start and stop must start with ( or [, in order to specify if the range item is respectively exclusive or inclusive.
//! The special values of + or - for start and stop have the special meaning or positively infinite and negatively infinite strings,
//! so for instance the command ZRANGEBYLEX myzset - + is guaranteed to return all the elements in the sorted set, if all the elements have the same score.
//!
//! Details on strings comparison:
//! Strings are compared as binary array of bytes. Because of how the ASCII character set is specified, this means that usually this also have the effect
//! of comparing normal ASCII characters in an obvious dictionary way. However this is not true if non plain ASCII strings are used (for example utf8 strings).
//!
//! However the user can apply a transformation to the encoded string so that the first part of the element inserted in the sorted set will compare as the user requires
//! for the specific application. For example if I want to add strings that will be compared in a case-insensitive way, but I still want to retrieve the real case when querying,
//! I can add strings in the following way:
//!
//! ZADD autocomplete 0 foo:Foo 0 bar:BAR 0 zap:zap
//!
//! Because of the first normalized part in every element (before the colon character), we are forcing a given comparison, however after the range is queries using ZRANGEBYLEX
//! the application can display to the user the second part of the string, after the colon.
//!
//! The binary nature of the comparison allows to use sorted sets as a general purpose index, for example the first part of the element can be a 64 bit big endian number:
//! since big endian numbers have the most significant bytes in the initial positions, the binary comparison will match the numerical comparison of the numbers.
//! This can be used in order to implement range queries on 64 bit values. As in the example below, after the first 8 bytes we can store the value of the element we are actually indexing.
//!
//! Return value: list of elements in the specified score range.
//!
//! Examples
//! redis>  ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis>  ZRANGEBYLEX myzset - [c
//! 1) "a"
//! 2) "b"
//! 3) "c"
//! redis>  ZRANGEBYLEX myzset - (c
//! 1) "a"
//! 2) "b"
//! redis>  ZRANGEBYLEX myzset [aaa (g
//! 1) "b"
//! 2) "c"
//! 3) "d"
//! 4) "e"
//! 5) "f"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRangeByLex(const QString &key, const QString &min, const QString &max, const int offset, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    QString command = QString("ZRANGEBYLEX %1 %2 %3").arg(key, min, max);
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Возвращает все элементы между min и max, включая min и max
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \param withScores Если true - вернуть количество элементов вместе с элементами
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! Returns all the elements in the sorted set at key with a score between min and max (including elements with score equal to min or max).
//! The elements are considered to be ordered from low to high scores.
//!
//! The elements having the same score are returned in lexicographical order (this follows from a property of the sorted set implementation in Redis and does not involve further computation).
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! Keep in mind that if offset is large, the sorted set needs to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! The optional WITHSCORES argument makes the command return both the element and its score, instead of the element alone. This option is available since Redis 2.0.
//!
//! Exclusive intervals and infinity:
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//!
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (.
//! For example:
//!
//! ZRANGEBYSCORE zset (1 5
//! Will return all elements with 1 < score <= 5 while:
//!
//! ZRANGEBYSCORE zset (5 (10
//! Will return all the elements with 5 < score < 10 (5 and 10 excluded).
//!
//! Return value: list of elements in the specified score range (optionally with their scores).
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZRANGEBYSCORE myzset -inf +inf
//! 1) "one"
//! 2) "two"
//! 3) "three"
//! redis>  ZRANGEBYSCORE myzset 1 2
//! 1) "one"
//! 2) "two"
//! redis>  ZRANGEBYSCORE myzset (1 2
//! 1) "two"
//! redis>  ZRANGEBYSCORE myzset (1 (2
//! (empty list or set)
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRangeByScore(const QString &key, const QString &min, const QString &max, const bool withScores, const int offset, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    QString command = QString("ZRANGEBYSCORE %1 %2 %3").arg(key, min, max);
    if (withScores)
        command += QString(" WITHSCORES");
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить ранг элемента отсортированного набора от мин к макс
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from low to high.
//! The rank (or index) is 0-based, which means that the member with the lowest score has rank 0.
//!
//! Use ZREVRANK to get the rank of an element with the scores ordered from high to low.
//! Return value:
//! - If member exists in the sorted set, Integer reply: the rank of member.
//! - If member does not exist in the sorted set or key does not exist, Bulk string reply: nil.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZRANK myzset "three"
//! (integer) 2
//! redis>  ZRANK myzset "four"
//! (nil)
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZRANK" << key << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Удалить элемент(ы) отсортированного набора
//! \param key Ключ
//! \param members Значения
//! \return
//!
//! Removes the specified members from the sorted set stored at key. Non existing members are ignored.
//!
//! An error is returned when key exists and does not hold a sorted set.
//! Return value: the number of members removed from the sorted set, not including non existing members.
//! Redis >= 2.4: Accepts multiple elements. In Redis versions older than 2.4 it was possible to remove a single member per call.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRem(const QString &key, const QStringList &members)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (members.isEmpty())
        return make_error("Invalid members (Empty)!");

    QStringList argv;
    argv << "ZREM" << key;
    for (const QString &member : members)
        argv << member;

    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, удаляет все элементы между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command removes all elements in the sorted set stored at key between the lexicographical range specified by min and max.
//!
//! The meaning of min and max are the same of the ZRANGEBYLEX command. Similarly, this command actually returns the same elements that ZRANGEBYLEX would return
//! if called with the same min and max arguments.
//!
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 0 aaaa 0 b 0 c 0 d 0 e
//! (integer) 5
//! redis>  ZADD myzset 0 foo 0 zap 0 zip 0 ALPHA 0 alpha
//! (integer) 5
//! redis>  ZRANGE myzset 0 -1
//! 1) "ALPHA"
//! 2) "aaaa"
//! 3) "alpha"
//! 4) "b"
//! 5) "c"
//! 6) "d"
//! 7) "e"
//! 8) "foo"
//! 9) "zap"
//! 10) "zip"
//! redis>  ZREMRANGEBYLEX myzset [alpha [omega
//! (integer) 6
//! redis>  ZRANGE myzset 0 -1
//! 1) "ALPHA"
//! 2) "aaaa"
//! 3) "zap"
//! 4) "zip"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRemRangeByLex(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    return this->redisExecCommand(QString("ZREMRANGEBYLEX %1 %2 %3").arg(key, min, max));
}

//!
//! \brief Удалить все элементе в отсортированном наборе между индексами start и stop
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \return
//!
//! Removes all elements in the sorted set stored at key with rank between start and stop.
//! Both start and stop are 0 -based indexes with 0 being the element with the lowest score.
//! These indexes can be negative numbers, where they indicate offsets starting at the element with the highest score.
//! For example: -1 is the element with the highest score, -2 the element with the second highest score and so forth.
//!
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREMRANGEBYRANK myzset 0 1
//! (integer) 2
//! redis>  ZRANGE myzset 0 -1 WITHSCORES
//! 1) "three"
//! 2) "3"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRemRangeByRank(const QString &key, const int start, const int stop)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    return this->redisExecCommand(QString("ZREMRANGEBYRANK %1 %2 %3").arg(key).arg(start).arg(stop));
}

//!
//! \brief Удалить все элементе в отсортированном наборе между min и max, включая их.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! Removes all elements in the sorted set stored at key with a score between min and max (inclusive).
//!
//! Since version 2.1.6, min and max can be exclusive, following the syntax of ZRANGEBYSCORE.
//! Return value: the number of elements removed.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREMRANGEBYSCORE myzset -inf (2
//! (integer) 1
//! redis>  ZRANGE myzset 0 -1 WITHSCORES
//! 1) "two"
//! 2) "2"
//! 3) "three"
//! 4) "3"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRemRangeByScore(const QString &key, const QString &min, const QString &max)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    return this->redisExecCommand(QString("ZREMRANGEBYSCORE %1 %2 %3").arg(key, min, max));
}

//!
//! \brief Получить набор элементов в указанном диапазоне индексов start - stop
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \param withScores
//! \return
//!
//! Returns the specified range of elements in the sorted set stored at key. The elements are considered to be ordered from the highest to the lowest score.
//! Descending lexicographical order is used for elements with equal score.
//!
//! Apart from the reversed ordering, ZREVRANGE is similar to ZRANGE.
//! Return value: list of elements in the specified range (optionally with their scores).
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANGE myzset 0 -1
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis>  ZREVRANGE myzset 2 3
//! 1) "one"
//! redis>  ZREVRANGE myzset -2 -1
//! 1) "two"
//! 2) "one"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRevRange(const QString &key, const int start, const int stop, const bool withScores)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QString command = QString("ZREVRANGE %1 %2 %3").arg(key).arg(start).arg(stop);
    if (withScores)
        command += QString(" WITHSCORES");

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, возвращает все элементы между min и max в обратном порядке.
//! \param key Ключ
//! \param max Макс
//! \param min Мин
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command returns all the elements in the sorted set at key with a value between max and min.
//!
//! Apart from the reversed ordering, ZREVRANGEBYLEX is similar to ZRANGEBYLEX.
//! Return value: list of elements in the specified score range.
//!
//! Examples
//! redis>  ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis>  ZREVRANGEBYLEX myzset [c -
//! 1) "c"
//! 2) "b"
//! 3) "a"
//! redis>  ZREVRANGEBYLEX myzset (c -
//! 1) "b"
//! 2) "a"
//! redis>  ZREVRANGEBYLEX myzset (g [aaa
//! 1) "f"
//! 2) "e"
//! 3) "d"
//! 4) "c"
//! 5) "b"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRevRangeByLex(const QString &key, const QString &max, const QString &min, const int offset, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    QString command = QString("ZREVRANGEBYLEX %1 %2 %3").arg(key, max, min);
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить все элементе в отсортированном наборе между min и max, включая их, в обратном порядке.
//! \param key Ключ
//! \param max Макс
//! \param min Мин
//! \param withScores
//! \param offset Смещение
//! \param count Количество
//! \return
//!
//! Returns all the elements in the sorted set at key with a score between max and min (including elements with score equal to max or min). In contrary to the default ordering of sorted sets, for this command the elements are considered to be ordered from high to low scores.
//!
//! The elements having the same score are returned in reverse lexicographical order.
//!
//! Apart from the reversed ordering, ZREVRANGEBYSCORE is similar to ZRANGEBYSCORE.
//! Return value
//!
//! Array reply: list of elements in the specified score range (optionally with their scores).
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANGEBYSCORE myzset +inf -inf
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis>  ZREVRANGEBYSCORE myzset 2 1
//! 1) "two"
//! 2) "one"
//! redis>  ZREVRANGEBYSCORE myzset 2 (1
//! 1) "two"
//! redis>  ZREVRANGEBYSCORE myzset (2 (1
//! (empty list or set)
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRevRangeByScore(const QString &key, const QString &max, const QString &min, const bool withScores, const int offset, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    if (min.isEmpty() || min.indexOf(" ") != -1)
        return make_error("Invalid min!");

    if (max.isEmpty() || max.indexOf(" ") != -1)
        return make_error("Invalid max!");

    if (min.at(0) != '(' && min.at(0) != '['
        && min != "+" && min != "-")
        return make_error("Invalid min!");

    if (max.at(0) != '(' && max.at(0) != '['
        && max != "+" && max != "-")
        return make_error("Invalid max!");

    QString command = QString("ZREVRANGEBYSCORE %1 %2 %3").arg(key, max, min);
    if (withScores)
        command += QString(" WITHSCORES");
    if (offset > 0 && count > 0)
        command += QString(" LIMIT %1 %2").arg(offset).arg(count);

    return this->redisExecCommand(command.trimmed());
}

//!
//! \brief Получить ранг элемента отсортированного набора от макс к мин
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from high to low.
//! The rank (or index) is 0-based, which means that the member with the highest score has rank 0.
//!
//! Use ZRANK to get the rank of an element with the scores ordered from low to high.
//! Return value:
//! - If member exists in the sorted set, Integer reply: the rank of member.
//! - If member does not exist in the sorted set or key does not exist, Bulk string reply: nil.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZADD myzset 2 "two"
//! (integer) 1
//! redis>  ZADD myzset 3 "three"
//! (integer) 1
//! redis>  ZREVRANK myzset "one"
//! (integer) 2
//! redis>  ZREVRANK myzset "four"
//! (nil)
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRevRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZREVRANK" << key << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Получить счет элемента отсортированного набора
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Returns the score of member in the sorted set at key.
//!
//! If member does not exist in the sorted set, or key does not exist, nil is returned.
//! Return value: the score of member (a double precision floating point number), represented as string.
//!
//! Examples
//! redis>  ZADD myzset 1 "one"
//! (integer) 1
//! redis>  ZSCORE myzset "one"
//! "1"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZScore(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZSCORE" << key << member;
    return this->redisExecCommandArgv(argv);
}

//!
//! \brief Вычислить объединение отсортированных наборов и сохранить результат в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей наборов
//! \param weightList Список "весов"
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Computes the union of numkeys sorted sets given by the specified keys, and stores the result in destination.
//! It is mandatory to provide the number of input keys (numkeys) before passing the input keys and the other (optional) arguments.
//!
//! By default, the resulting score of an element is the sum of its scores in the sorted sets where it exists.
//!
//! Using the WEIGHTS option, it is possible to specify a multiplication factor for each input sorted set.
//! This means that the score of every element in every input sorted set is multiplied by this factor before being passed to the aggregation function.
//! When WEIGHTS is not given, the multiplication factors default to 1.
//!
//! With the AGGREGATE option, it is possible to specify how the results of the union are aggregated.
//! This option defaults to SUM, where the score of an element is summed across the inputs where it exists.
//! When this option is set to either MIN or MAX, the resulting set will contain the minimum or maximum score of an element across the inputs where it exists.
//!
//! If destination already exists, it is overwritten.
//!
//! Return value: the number of elements in the resulting sorted set at destination.
//!
//! Examples
//! redis>  ZADD zset1 1 "one"
//! (integer) 1
//! redis>  ZADD zset1 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 1 "one"
//! (integer) 1
//! redis>  ZADD zset2 2 "two"
//! (integer) 1
//! redis>  ZADD zset2 3 "three"
//! (integer) 1
//! redis>  ZUNIONSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 3
//! redis>  ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "three"
//! 4) "9"
//! 5) "two"
//! 6) "10"
//! redis>
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZUnionStore(const QString &destKey, const QStringList &keyList, const QList<int> &weightList, const QString &aggregateFlag)
{
    if (destKey.isEmpty() || destKey.indexOf(" ") != -1)
        return make_error("Invalid destKey!");

    if (keyList.isEmpty())
        return make_error("Invalid keyList (Empty)!");

    for (const QString &key : keyList) {
        if (key.indexOf(" ") != -1)
            return make_error(QString("Invalid key (%1)!").arg(key));
    }
    QString weightListStr;
    for (const int weight : weightList) {
        if (weight <= 0)
            return __RESULT_IMPL();
        weightListStr += QString("%1 ").arg(weight);
    }
    if (!aggregateFlag.isEmpty()
        && aggregateFlag.toUpper() != QString("SUM")
        && aggregateFlag.toUpper() != QString("MIN")
        && aggregateFlag.toUpper() != QString("MAX"))
        return make_error("Invalid aggregateFlag!");

    QString command = QString("ZUNIONSTORE %1 %2 %3")
                      .arg(destKey)
                      .arg(keyList.size())
                      .arg(keyList.join(" "));

    if (!weightList.isEmpty())
        command += QString(" WEIGHTS %1").arg(weightListStr);
    if (!aggregateFlag.isEmpty())
        command += QString(" AGGREGATE %1").arg(aggregateFlag.toUpper());

    return this->redisExecCommand(command.trimmed());
}

// --- protected ---

//!
//! \brief Установить последнюю ошибку
//! \param error Ошибка
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
void QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::setLastError_safe(const QString &error)
{
    QMutexLocker lock(&_mutexErr);
    _lastError = error;
}

//!
//! \brief Очистить посленюю ошибку
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
void QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::clearLastError_safe()
{
    QMutexLocker lock(&_mutexErr);
    _lastError.clear();
}

// --- private ---

//!
//! \brief Получить указатель на объект класса слиента
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__CLIENT_IMPL *QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::as_CLIENT_IMPL_ptr()
{
    return static_cast<__CLIENT_IMPL*>(this);
}

#endif // QTREDISBASE_H

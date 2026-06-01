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
//! Документация по командам: https://redis.io/docs/latest/commands/
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
    __RESULT_IMPL redisExecCommand(const QtRedisCommand &command);
    __RESULT_IMPL redisExecCommand(const QString &command);
    __RESULT_IMPL redisExecCommand(const QByteArray &command);
    __RESULT_IMPL redisExecCommand(const QStringList &commandArgv);
    __RESULT_IMPL redisExecCommand(const QList<QByteArray> &commandArgv);

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
    //!
    //! \brief Задать сообщение об ошибке и вернуть пустой результат
    //! \param error Сообщение об ошибке
    //! \return
    //!
    template <typename T = __RESULT_IMPL>
    typename std::enable_if<std::is_same<T, bool>::value, bool>::type
    make_error(const QString &error) {
        this->setLastError_safe(error);
        return false;
    }

    //!
    //! \brief Задать сообщение об ошибке и вернуть пустой результат
    //! \param error Сообщение об ошибке
    //! \return
    //!
    template <typename T = __RESULT_IMPL>
    typename std::enable_if<!std::is_same<T, bool>::value, __RESULT_IMPL>::type
    make_error(const QString &error) {
        this->setLastError_safe(error);
        return __RESULT_IMPL();
    }

    __CLIENT_IMPL *as_CLIENT_IMPL_ptr();

    mutable QMutex  _mutexErr;  //!< мьютекс для обработки ошибок
    QString         _lastError; //!< сообщение об ошибке
};


// ------------------------------------------------------------------------
// -- ERRORS FUNCTIONS ----------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Задано ли сообщение об ошибке
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
bool QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::hasLastError() const
{
    QMutexLocker lock(&_mutexErr);
    return !_lastError.isEmpty();
}

//!
//! \brief Сообщение об ошибке
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
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QtRedisCommand &command)
{
    QMutexLocker lock(&_mutex);
    if (!command.isValid())
        return make_error("Command is Invalid!");

    return as_CLIENT_IMPL_ptr()->processCommand(command);
}

//!
//! \brief Выпонить команду
//! \param command Команда
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QString &command)
{
    return this->redisExecCommand(QtRedisCommand::fromString(command));
}

//!
//! \brief Выпонить команду
//! \param command Команда
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QByteArray &command)
{
    return this->redisExecCommand(QtRedisCommand::fromByteArray(command));
}

//!
//! \brief Выполнить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QStringList &commandArgv)
{
    return this->redisExecCommand(QtRedisCommand::fromStringList(commandArgv));
}

//!
//! \brief Выполнить команду
//! \param commandArgv Команда в формате списка аргументов
//! \return
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisExecCommand(const QList<QByteArray> &commandArgv)
{
    return this->redisExecCommand(QtRedisCommand::fromByteArrayList(commandArgv));
}


// ------------------------------------------------------------------------
// -- KEY-VALUE COMMANDS --------------------------------------------------
// ------------------------------------------------------------------------

//!
//! \brief Список ключей в БД
//! \param arg Условие поиска
//! \return
//!
//! Redis command: KEYS
//!
//! Syntax
//!
//! KEYS pattern
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) with N being the number of keys in the database, under the assumption that the key names in the database and the given pattern have limited length.
//! ACL categories:
//!     @keyspace, @read, @slow, @dangerous
//!
//! Returns all keys matching pattern.
//!
//! While the time complexity for this operation is O(N), the constant times are fairly low. For example,
//! Redis running on an entry level laptop can scan a 1 million key database in 40 milliseconds.
//!
//! Warning: consider KEYS as a command that should only be used in production environments with extreme care.
//! It may ruin performance when it is executed against large databases. This command is intended for debugging and special operations,
//! such as changing your keyspace layout. Don't use KEYS in your regular application code.
//! If you're looking for a way to find keys in a subset of your keyspace, consider using SCAN or sets.
//!
//! Supported glob-style patterns:
//!
//!     h?llo matches hello, hallo and hxllo
//!     h*llo matches hllo and heeeello
//!     h[ae]llo matches hello and hallo, but not hillo
//!     h[^e]llo matches hallo, hbllo, ... but not hello
//!     h[a-b]llo matches hallo and hbllo
//!
//! Use \ to escape special characters if you want to match them verbatim.
//!
//! When using Redis Cluster, the search is optimized for patterns that imply a single slot.
//! If a pattern can only match keys of one slot, Redis only iterates over keys in that slot,
//! rather than the whole database, when searching for keys matching the pattern.
//! For example, with the pattern {a}h*llo, Redis would only try to match it with the keys in slot 15495, which hash tag {a} implies.
//! To use pattern with hash tag, see Hash tags in the Cluster specification for more information.
//!
//! Examples
//! redis> MSET firstname Jack lastname Stuntman age 35
//! "OK"
//! redis> KEYS *name*
//! 1) "lastname"
//! 2) "firstname"
//! redis> KEYS a??
//! 1) "age"
//! redis> KEYS *
//! 1) "age"
//! 2) "lastname"
//! 3) "firstname"
//!
//! RESP2/RESP3 Reply
//! Array reply: a list of keys matching pattern.
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
//! Redis command: RANDOMKEY
//!
//! Syntax
//!
//! RANDOMKEY
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @read, @slow
//!
//! Return a random key from the currently selected database.
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: when the database is empty.
//!     Bulk string reply: a random key in database.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: when the database is empty.
//!     Bulk string reply: a random key in the database.
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
//! Redis command: EXISTS
//!
//! Syntax
//!
//! EXISTS key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of keys to check.
//! ACL categories:
//!     @keyspace, @read, @fast
//!
//! Returns if key exists.
//!
//! The user should be aware that if the same existing key is mentioned in the arguments multiple times,
//! it will be counted multiple times. So if somekey exists, EXISTS somekey somekey will return 2.
//!
//! Examples
//! redis> SET key1 "Hello"
//! "OK"
//! redis> EXISTS key1
//! (integer) 1
//! redis> EXISTS nosuchkey
//! (integer) 0
//! redis> SET key2 "World"
//! "OK"
//! redis> EXISTS key1 key2 nosuchkey
//! (integer) 2
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of keys that exist from those specified as arguments.
//!
//! History
//!     Starting with Redis version 3.0.3: Accepts multiple key arguments.
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
//! Redis command: GET
//!
//! Syntax
//!
//! GET key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @string, @fast
//!
//! Get the value of key. If the key does not exist the special value nil is returned. An error is returned if the value stored at key is not a string, because GET only handles string values.
//!
//! Examples
//! redis> GET nonexisting
//! (nil)
//! redis> SET mykey "Hello"
//! "OK"
//! redis> GET mykey
//! "Hello"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: the value of the key.
//!     Nil reply: if the key does not exist.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Bulk string reply: the value of the key.
//!     Null reply: key does not exist.
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
//! Redis commadn: GETRANGE
//!
//! Syntax
//!
//! GETRANGE key start end
//!
//! Available since:
//!     2.4.0
//! Time complexity:
//!     O(N) where N is the length of the returned string. The complexity is ultimately determined by the returned length,
//!          but because creating a substring from an existing string is very cheap, it can be considered O(1) for small strings.
//! ACL categories:
//!     @read, @string, @slow
//!
//! Returns the substring of the string value stored at key, determined by the offsets start and end (both are inclusive).
//! Negative offsets can be used in order to provide an offset starting from the end of the string.
//! So -1 means the last character, -2 the penultimate and so forth.
//!
//! The function handles out of range requests by limiting the resulting range to the actual length of the string.
//!
//! Examples
//! redis> SET mykey "This is a string"
//! "OK"
//! redis> GETRANGE mykey 0 3
//! "This"
//! redis> GETRANGE mykey -3 -1
//! "ing"
//! redis> GETRANGE mykey 0 -1
//! "This is a string"
//! redis> GETRANGE mykey 10 100
//! "string"
//!
//! RESP2/RESP3 Reply
//! Bulk string reply: The substring of the string value stored at key, determined by the offsets start and end (both are inclusive).
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
//! Redis command: GETSET (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by SET with the GET argument when migrating or writing new code.
//!
//! Syntax
//!
//! GETSET key value
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! Atomically sets key to value and returns the old value stored at key. Returns an error when key exists but does not hold a string value.
//! Any previous time to live associated with the key is discarded on successful SET operation.
//!
//! Design pattern
//!
//! GETSET can be used together with INCR for counting with atomic reset. For example: a process may call INCR against the key mycounter every time some event occurs,
//! but from time to time we need to get the value of the counter and reset it to zero atomically. This can be done using GETSET mycounter "0":
//!
//! redis> INCR mycounter
//! (integer) 1
//! redis> GETSET mycounter "0"
//! "1"
//! redis> GET mycounter
//! "0"
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> GETSET mykey "World"
//! "Hello"
//! redis> GET mykey
//! "World"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: the old value stored at the key.
//!     Nil reply: if the key does not exist.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Bulk string reply: the old value stored at the key.
//!     Null reply: if the key does not exist.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisGetSet(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "GETSET" << key << value;
    return this->redisExecCommand(argv);
}

//!
//! \brief Добавить/задать значение ключу
//! \param key Ключ
//! \param appendValue Значение
//! \return
//!
//! Redis command: APPEND
//!
//! Syntax
//!
//! APPEND key value
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(1). The amortized time complexity is O(1) assuming the appended value is small and the already present value is of any size,
//!           since the dynamic string library used by Redis will double the free space available on every reallocation.
//! ACL categories:
//!     @write, @string, @fast
//!
//! If key already exists and is a string, this command appends the value at the end of the string.
//! If key does not exist it is created and set as an empty string, so APPEND will be similar to SET in this special case.
//!
//! Examples
//! redis> EXISTS mykey
//! (integer) 0
//! redis> APPEND mykey "Hello"
//! (integer) 5
//! redis> APPEND mykey " World"
//! (integer) 11
//! redis> GET mykey
//! "Hello World"
//!
//! Pattern: Time series
//!
//! The APPEND command can be used to create a very compact representation of a list of fixed-size samples,
//! usually referred as time series. Every time a new sample arrives we can store it using the command
//!
//! APPEND timeseries "fixed-size sample"
//!
//! Accessing individual elements in the time series is not hard:
//!
//!     STRLEN can be used in order to obtain the number of samples.
//!     GETRANGE allows for random access of elements. If our time series have associated time information we can easily implement a binary search
//!              to get range combining GETRANGE with the Lua scripting engine available in Redis 2.6.
//!     SETRANGE can be used to overwrite an existing time series.
//!
//! The limitation of this pattern is that we are forced into an append-only mode of operation, there is no way to cut the time series to a given size easily
//! because Redis currently lacks a command able to trim string objects. However the space efficiency of time series stored in this way is remarkable.
//!
//! Hint: it is possible to switch to a different key based on the current Unix time, in this way it is possible to have just a relatively small amount of samples per key,
//! to avoid dealing with very big keys, and to make this pattern more friendly to be distributed across many Redis instances.
//!
//! An example sampling the temperature of a sensor using fixed-size strings (using a binary format is better in real implementations).
//! redis> APPEND ts "0043"
//! (integer) 4
//! redis> APPEND ts "0035"
//! (integer) 8
//! redis> GETRANGE ts 0 3
//! "0043"
//! redis> GETRANGE ts 4 7
//! "0035"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the string after the append operation.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisAppend(const QString &key, const QString &appendValue)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "APPEND" << key << appendValue;
    return this->redisExecCommand(argv);
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
//! Redis command: SET
//!
//! Syntax
//!
//! SET key value [NX | XX] [GET] [EX seconds | PX milliseconds |
//!   EXAT unix-time-seconds | PXAT unix-time-milliseconds | KEEPTTL]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @slow
//!
//! Set key to hold the string value. If key already holds a value, it is overwritten, regardless of its type.
//! Any previous time to live associated with the key is discarded on successful SET operation.
//!
//! Options
//!
//! The SET command supports a set of options that modify its behavior:
//!
//!     EX seconds -- Set the specified expire time, in seconds (a positive integer).
//!     PX milliseconds -- Set the specified expire time, in milliseconds (a positive integer).
//!     EXAT timestamp-seconds -- Set the specified Unix time at which the key will expire, in seconds (a positive integer).
//!     PXAT timestamp-milliseconds -- Set the specified Unix time at which the key will expire, in milliseconds (a positive integer).
//!     NX -- Only set the key if it does not already exist.
//!     XX -- Only set the key if it already exists.
//!     KEEPTTL -- Retain the time to live associated with the key.
//!     GET -- Return the old string stored at key, or nil if key did not exist. An error is returned and SET aborted if the value stored at key is not a string.
//!
//! Note: Since the SET command options can replace SETNX, SETEX, PSETEX, GETSET, it is possible that in future versions of Redis these commands will be deprecated and finally removed.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> GET mykey
//! "Hello"
//! redis> SET anotherkey "will expire in a minute" EX 60
//! "OK"
//!
//! Patterns
//!
//! Note: The following pattern is discouraged in favor of the Redlock algorithm which is only a bit more complex to implement, but offers better guarantees and is fault tolerant.
//!
//! The command SET resource-name anystring NX EX max-lock-time is a simple way to implement a locking system with Redis.
//!
//! A client can acquire the lock if the above command returns OK (or retry after some time if the command returns Nil), and remove the lock just using DEL.
//!
//! The lock will be auto-released after the expire time is reached.
//!
//! It is possible to make this system more robust modifying the unlock schema as follows:
//!
//!     Instead of setting a fixed string, set a non-guessable large random string, called token.
//!     Instead of releasing the lock with DEL, send a script that only removes the key if the value matches.
//!
//! This avoids that a client will try to release the lock after the expire time deleting the key created by another client that acquired the lock later.
//!
//! An example of unlock script would be similar to the following:
//!
//! if redis.call("get",KEYS[1]) == ARGV[1]
//! then
//!     return redis.call("del",KEYS[1])
//! else
//!     return 0
//! end
//!
//! The script should be called with EVAL ...script... 1 resource-name token-value
//!
//! RESP2 Reply
//!
//! Any of the following:
//!     Nil reply: GET not given: Operation was aborted (conflict with one of the XX/NX options).
//!     Simple string reply: OK. GET not given: The key was set.
//!     Nil reply: GET given: The key didn't exist before the SET.
//!     Bulk string reply: GET given: The previous value of the key.
//!
//! RESP3 Reply
//!
//! Any of the following:
//!     Null reply: GET not given: Operation was aborted (conflict with one of the XX/NX options).
//!     Simple string reply: OK. GET not given: The key was set.
//!     Null reply: GET given: The key didn't exist before the SET.
//!     Bulk string reply: GET given: The previous value of the key.
//!
//! History
//!     Starting with Redis version 2.6.12: Added the EX, PX, NX and XX options.
//!     Starting with Redis version 6.0.0: Added the KEEPTTL option.
//!     Starting with Redis version 6.2.0: Added the GET, EXAT and PXAT option.
//!     Starting with Redis version 7.0.0: Allowed the NX and GET options to be used together.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Перезаписать часть строки значения на новое, начиная с позиции
//! \param key Ключ
//! \param value Новое значение
//! \param offset Позиция
//! \return
//!
//! Redis command: SETRANGE
//!
//! Syntax
//!
//! SETRANGE key offset value
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1), not counting the time taken to copy the new string in place. Usually, this string is very small so the amortized complexity is O(1).
//!           Otherwise, complexity is O(M) with M being the length of the value argument.
//! ACL categories:
//!     @write, @string, @slow
//!
//! Overwrites part of the string stored at key, starting at the specified offset, for the entire length of value. If the offset is larger than the current length
//! of the string at key, the string is padded with zero-bytes to make offset fit. Non-existing keys are considered as empty strings,
//! so this command will make sure it holds a string large enough to be able to set value at offset.
//!
//! Note that the maximum offset that you can set is 2^29 -1 (536870911), as Redis Strings are limited to 512 megabytes.
//! If you need to grow beyond this size, you can use multiple keys.
//!
//! Warning: When setting the last possible byte and the string value stored at key does not yet hold a string value, or holds a small string value,
//! Redis needs to allocate all intermediate memory which can block the server for some time. On a 2010 MacBook Pro,
//! setting byte number 536870911 (512MB allocation) takes ~300ms, setting byte number 134217728 (128MB allocation) takes ~80ms,
//! setting bit number 33554432 (32MB allocation) takes ~30ms and setting bit number 8388608 (8MB allocation) takes ~8ms.
//! Note that once this first allocation is done, subsequent calls to SETRANGE for the same key will not have the allocation overhead.
//!
//! Patterns
//!
//! Thanks to SETRANGE and the analogous GETRANGE commands, you can use Redis strings as a linear array with O(1) random access.
//! This is a very fast and efficient storage in many real world use cases.
//!
//! Examples
//! redis> SET key1 "Hello World"
//! "OK"
//! redis> SETRANGE key1 6 "Redis"
//! (integer) 11
//! redis> GET key1
//! "Hello Redis"
//!
//! Example of zero padding:
//! redis> SETRANGE key2 6 "Redis"
//! (integer) 11
//! redis> GET key2
//! "Redis"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the string after it was modified by the command.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSetRange(const QString &key, const QString &value, const int offset)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "SETRANGE" << key << QString::number(offset) << value;
    return this->redisExecCommand(argv);
}

//!
//! \brief Удалить ключи и их значения из БД
//! \param keyList Список ключей
//! \return
//!
//! Redis command: DEL
//!
//! Syntax
//!
//! DEL key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of keys that will be removed. When a key to remove holds a value other than a string,
//!          the individual complexity for this key is O(M) where M is the number of elements in the list, set,
//!          sorted set or hash. Removing a single key that holds a string value is O(1).
//! ACL categories:
//!     @keyspace, @write, @slow
//!
//! Removes the specified keys. A key is ignored if it does not exist.
//!
//! Examples
//! redis> SET key1 "Hello"
//! "OK"
//! redis> SET key2 "World"
//! "OK"
//! redis> DEL key1 key2 key3
//! (integer) 2
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of keys that were removed.
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
//! Redis command: STRLEN
//!
//! Syntax
//!
//! STRLEN key
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @string, @fast
//!
//! Returns the length of the string value stored at key. An error is returned when key holds a non-string value.
//!
//! Examples
//! redis> SET mykey "Hello world"
//! "OK"
//! redis> STRLEN mykey
//! (integer) 11
//! redis> STRLEN nonexisting
//! (integer) 0
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the string stored at key, or 0 when the key does not exist.
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
//! Redis command: EXPIRE
//!
//! Syntax
//!
//! EXPIRE key seconds [NX | XX | GT | LT]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! Set a timeout on key. After the timeout has expired, the key will automatically be deleted.
//! A key with an associated timeout is often said to be volatile in Redis terminology.
//!
//! The timeout will only be cleared by commands that delete or overwrite the contents of the key, including DEL, SET, GETSET and all the *STORE commands.
//! This means that all the operations that conceptually alter the value stored at the key without replacing it with a new one will leave the timeout untouched.
//! For instance, incrementing the value of a key with INCR, pushing a new value into a list with LPUSH,
//! or altering the field value of a hash with HSET are all operations that will leave the timeout untouched.
//!
//! The timeout can also be cleared, turning the key back into a persistent key, using the PERSIST command.
//!
//! If a key is renamed with RENAME, the associated time to live is transferred to the new key name.
//!
//! If a key is overwritten by RENAME, like in the case of an existing key Key_A that is overwritten by a call like RENAME Key_B Key_A,
//! it does not matter if the original Key_A had a timeout associated or not, the new key Key_A will inherit all the characteristics of Key_B.
//!
//! Note that calling EXPIRE/PEXPIRE with a non-positive timeout or EXPIREAT/PEXPIREAT with a time in the past will result
//! in the key being deleted rather than expired (accordingly, the emitted key event will be del, not expired).
//!
//! Options
//!
//! The EXPIRE command supports a set of options:
//!
//!     NX -- Set expiry only when the key has no expiry
//!     XX -- Set expiry only when the key has an existing expiry
//!     GT -- Set expiry only when the new expiry is greater than current one
//!     LT -- Set expiry only when the new expiry is less than current one
//!
//! A non-volatile key is treated as an infinite TTL for the purpose of GT and LT. The GT, LT and NX options are mutually exclusive.
//!
//! Refreshing expires
//!
//! It is possible to call EXPIRE using as argument a key that already has an existing expire set. In this case the time to live of a key is updated to the new value.
//! There are many useful applications for this, an example is documented in the Navigation session pattern section below.
//!
//! Differences in Redis prior 2.1.3
//!
//! In Redis versions prior 2.1.3 altering a key with an expire set using a command altering its value had the effect of removing the key entirely.
//! This semantics was needed because of limitations in the replication layer that are now fixed.
//!
//! EXPIRE would return 0 and not alter the timeout for a key with a timeout set.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> EXPIRE mykey 10
//! (integer) 1
//! redis> TTL mykey
//! (integer) 10
//! redis> SET mykey "Hello World"
//! "OK"
//! redis> TTL mykey
//! (integer) -1
//! redis> EXPIRE mykey 10 XX
//! (integer) 0
//! redis> TTL mykey
//!
//! Pattern: Navigation session
//!
//! Imagine you have a web service and you are interested in the latest N pages recently visited by your users,
//! such that each adjacent page view was not performed more than 60 seconds after the previous. Conceptually you may consider this set of page views as a Navigation session of your user,
//! that may contain interesting information about what kind of products he or she is looking for currently, so that you can recommend related products.
//!
//! You can easily model this pattern in Redis using the following strategy: every time the user does a page view you call the following commands:
//!
//! MULTI
//! RPUSH pagewviews.user:<userid> http://.....
//! EXPIRE pagewviews.user:<userid> 60
//! EXEC
//!
//! If the user will be idle more than 60 seconds, the key will be deleted and only subsequent page views that have less than 60 seconds of difference will be recorded.
//!
//! This pattern is easily modified to use counters using INCR instead of lists using RPUSH.
//!
//! Appendix: Redis expires
//! Keys with an expire
//!
//! Normally Redis keys are created without an associated time to live. The key will simply live forever, unless it is removed by the user in an explicit way, for instance using the DEL command.
//!
//! The EXPIRE family of commands is able to associate an expire to a given key, at the cost of some additional memory used by the key. When a key has an expire set,
//! Redis will make sure to remove the key when the specified amount of time elapsed.
//!
//! The key time to live can be updated or entirely removed using the EXPIRE and PERSIST command (or other strictly related commands).
//!
//! Expire accuracy
//!
//! In Redis 2.4 the expire might not be pin-point accurate, and it could be between zero to one seconds out.
//!
//! Since Redis 2.6 the expire error is from 0 to 1 milliseconds.
//!
//! Expires and persistence
//!
//! Keys expiring information is stored as absolute Unix timestamps (in milliseconds in case of Redis version 2.6 or greater).
//! This means that the time is flowing even when the Redis instance is not active.
//!
//! For expires to work well, the computer time must be taken stable. If you move an RDB file from two computers with a big desync in their clocks,
//! funny things may happen (like all the keys loaded to be expired at loading time).
//!
//! Even running instances will always check the computer clock, so for instance if you set a key with a time to live of 1000 seconds,
//! and then set your computer time 2000 seconds in the future, the key will be expired immediately, instead of lasting for 1000 seconds.
//!
//! How Redis expires keys
//!
//! Redis keys are expired in two ways: a passive way and an active way.
//!
//! A key is passively expired when a client tries to access it and the key is timed out.
//!
//! However, this is not enough as there are expired keys that will never be accessed again. These keys should be expired anyway,
//! so periodically, Redis tests a few keys at random amongst the set of keys with an expiration. All the keys that are already expired are deleted from the keyspace.
//! How expires are handled in the replication link and AOF file
//!
//! In order to obtain a correct behavior without sacrificing consistency, when a key expires, a DEL operation is synthesized in both the AOF file and gains all the attached replicas nodes.
//! This way the expiration process is centralized in the master instance, and there is no chance of consistency errors.
//!
//! However while the replicas connected to a master will not expire keys independently (but will wait for the DEL coming from the master),
//! they'll still take the full state of the expires existing in the dataset, so when a replica is elected to master it will be able to expire the keys independently, fully acting as a master.
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0 if the timeout was not set; for example, the key doesn't exist, or the operation was skipped because of the provided arguments.
//!     Integer reply: 1 if the timeout was set.
//!
//! History
//!     Starting with Redis version 7.0.0: Added options: NX, XX, GT and LT.
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
//! Redis command: EXPIREAT
//!
//! Syntax
//!
//! EXPIREAT key unix-time-seconds [NX | XX | GT | LT]
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! EXPIREAT has the same effect and semantic as EXPIRE, but instead of specifying the number of seconds representing the TTL (time to live),
//! it takes an absolute Unix timestamp (seconds since January 1, 1970). A timestamp in the past will delete the key immediately.
//!
//! Please for the specific semantics of the command refer to the documentation of EXPIRE.
//! Background
//!
//! EXPIREAT was introduced in order to convert relative timeouts to absolute timeouts for the AOF persistence mode.
//! Of course, it can be used directly to specify that a given key should expire at a given time in the future.
//!
//! Options
//!
//! The EXPIREAT command supports a set of options:
//!
//!     NX -- Set expiry only when the key has no expiry
//!     XX -- Set expiry only when the key has an existing expiry
//!     GT -- Set expiry only when the new expiry is greater than current one
//!     LT -- Set expiry only when the new expiry is less than current one
//!
//! A non-volatile key is treated as an infinite TTL for the purpose of GT and LT. The GT, LT and NX options are mutually exclusive.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> EXISTS mykey
//! (integer) 1
//! redis> EXPIREAT mykey 1293840000
//! (integer) 1
//! redis> EXISTS mykey
//! (integer) 0
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0 if the timeout was not set; for example, the key doesn't exist, or the operation was skipped because of the provided arguments.
//!     Integer reply: 1 if the timeout was set.
//!
//! History
//!     Starting with Redis version 7.0.0: Added options: NX, XX, GT and LT.
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
//! Redis command: PEXPIRE
//!
//! Syntax
//!
//! PEXPIRE key milliseconds [NX | XX | GT | LT]
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! This command works exactly like EXPIRE but the time to live of the key is specified in milliseconds instead of seconds.
//!
//! Options
//!
//! The PEXPIRE command supports a set of options since Redis 7.0:
//!
//!     NX -- Set expiry only when the key has no expiry
//!     XX -- Set expiry only when the key has an existing expiry
//!     GT -- Set expiry only when the new expiry is greater than current one
//!     LT -- Set expiry only when the new expiry is less than current one
//!
//! A non-volatile key is treated as an infinite TTL for the purpose of GT and LT. The GT, LT and NX options are mutually exclusive.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> PEXPIRE mykey 1500
//! (integer) 1
//! redis> TTL mykey
//! (integer) 2
//! redis> PTTL mykey
//! (integer) 1499
//! redis> PEXPIRE mykey 1000 XX
//! (integer) 1
//! redis> TTL mykey
//! (integer) 1
//! redis> PEXPIRE mykey 1000 NX
//! (integer) 0
//! redis> TTL mykey
//! (integer) 1
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0if the timeout was not set. For example, if the key doesn't exist, or the operation skipped because of the provided arguments.
//!     Integer reply: 1 if the timeout was set.
//!
//! History
//!     Starting with Redis version 7.0.0: Added options: NX, XX, GT and LT.
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
//! Redis command: PEXPIREAT
//!
//! Syntax
//!
//! PEXPIREAT key unix-time-milliseconds [NX | XX | GT | LT]
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! PEXPIREAT has the same effect and semantic as EXPIREAT, but the Unix time at which the key will expire is specified in milliseconds instead of seconds.
//!
//! Options
//!
//! The PEXPIREAT command supports a set of options since Redis 7.0:
//!
//!     NX -- Set expiry only when the key has no expiry
//!     XX -- Set expiry only when the key has an existing expiry
//!     GT -- Set expiry only when the new expiry is greater than current one
//!     LT -- Set expiry only when the new expiry is less than current one
//!
//! A non-volatile key is treated as an infinite TTL for the purpose of GT and LT. The GT, LT and NX options are mutually exclusive.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> PEXPIREAT mykey 1555555555005
//! (integer) 1
//! redis> TTL mykey
//! (integer) -2
//! redis> PTTL mykey
//! (integer) -2
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 1 if the timeout was set.
//!     Integer reply: 0 if the timeout was not set. For example, if the key doesn't exist, or the operation was skipped due to the provided arguments.
//!
//! History
//!     Starting with Redis version 7.0.0: Added options: NX, XX, GT and LT.
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
//! Redis command: PERSIST
//!
//! Syntax
//!
//! PERSIST key
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! Remove the existing timeout on key, turning the key from volatile (a key with an expire set) to persistent (a key that will never expire as no timeout is associated).
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> EXPIRE mykey 10
//! (integer) 1
//! redis> TTL mykey
//! (integer) 10
//! redis> PERSIST mykey
//! (integer) 1
//! redis> TTL mykey
//! (integer) -1
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0 if key does not exist or does not have an associated timeout.
//!     Integer reply: 1 if the timeout has been removed.
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
//! Redis command: TTL
//!
//! Syntax
//!
//! TTL key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @read, @fast
//!
//! Returns the remaining time to live of a key that has a timeout. This introspection capability allows a Redis client
//! to check how many seconds a given key will continue to be part of the dataset.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//!
//!     The command returns -2 if the key does not exist.
//!     The command returns -1 if the key exists but has no associated expire.
//!
//! See also the PTTL command that returns the same information with milliseconds resolution (Only available in Redis 2.6 or greater).
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> EXPIRE mykey 10
//! (integer) 1
//! redis> TTL mykey
//! (integer) 10
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: TTL in seconds.
//!     Integer reply: -1 if the key exists but has no associated expiration.
//!     Integer reply: -2 if the key does not exist.
//!
//! History
//!     Starting with Redis version 2.8.0: Added the -2 reply.
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
//! Redis command: PTTL
//!
//! Syntax
//!
//! PTTL key
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @read, @fast
//!
//! Like TTL this command returns the remaining time to live of a key that has an expire set, with the sole difference that TTL returns
//! the amount of remaining time in seconds while PTTL returns it in milliseconds.
//!
//! In Redis 2.6 or older the command returns -1 if the key does not exist or if the key exist but has no associated expire.
//!
//! Starting with Redis 2.8 the return value in case of error changed:
//!
//!     The command returns -2 if the key does not exist.
//!     The command returns -1 if the key exists but has no associated expire.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> EXPIRE mykey 1
//! (integer) 1
//! redis> PTTL mykey
//! (integer) 1000
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: TTL in milliseconds.
//!     Integer reply: -1 if the key exists but has no associated expiration.
//!     Integer reply: -2 if the key does not exist.
//!
//! History
//!     Starting with Redis version 2.8.0: Added the -2 reply.
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
//! Redis command: DECR
//!
//! Syntax
//!
//! DECR key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! Decrements the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! See INCR for extra information on increment/decrement operations.
//!
//! Examples
//! redis> SET mykey "10"
//! "OK"
//! redis> DECR mykey
//! (integer) 9
//! redis> SET mykey "234293482390480948029348230948"
//! "OK"
//! redis> DECR mykey
//! (error) value is not an integer or out of range
//!
//! RESP2/RESP3 Reply
//! Integer reply: the value of the key after decrementing it.
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
//! Redis command: DECRBY
//!
//! Syntax
//!
//! DECRBY key decrement
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! The DECRBY command reduces the value stored at the specified key by the specified decrement.
//! If the key does not exist, it is initialized with a value of 0 before performing the operation.
//! If the key's value is not of the correct type or cannot be represented as an integer, an error is returned.
//! This operation is limited to 64-bit signed integers.
//!
//! See INCR for extra information on increment/decrement operations.
//!
//! Examples
//! redis> SET mykey "10"
//! "OK"
//! redis> DECRBY mykey 3
//! (integer) 7
//!
//! RESP2/RESP3 Reply
//! Integer reply: the value of the key after decrementing it.
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
//! Redis command: INCR
//!
//! Syntax
//!
//! INCR key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! Increments the number stored at key by one. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! Note: this is a string operation because Redis does not have a dedicated integer type.
//! The string stored at the key is interpreted as a base-10 64 bit signed integer to execute the operation.
//!
//! Redis stores integers in their integer representation, so for string values that actually hold an integer,
//! there is no overhead for storing the string representation of the integer.
//!
//! Examples
//! redis> SET mykey "10"
//! "OK"
//! redis> INCR mykey
//! (integer) 11
//! redis> GET mykey
//! "11"
//!
//! Pattern: Counter
//!
//! The counter pattern is the most obvious thing you can do with Redis atomic increment operations.
//! The idea is simply send an INCR command to Redis every time an operation occurs.
//! For instance in a web application we may want to know how many page views this user did every day of the year.
//!
//! To do so the web application may simply increment a key every time the user performs a page view,
//! creating the key name concatenating the User ID and a string representing the current date.
//!
//! This simple pattern can be extended in many ways:
//!
//!     It is possible to use INCR and EXPIRE together at every page view to have a counter counting only the latest N page views separated by less than the specified amount of seconds.
//!     A client may use GETSET in order to atomically get the current counter value and reset it to zero.
//!     Using other atomic increment/decrement commands like DECR or INCRBY it is possible to handle values that may get bigger or smaller depending on the operations performed by the user.
//!     Imagine for instance the score of different users in an online game.
//!
//! Pattern: Rate limiter
//!
//! The rate limiter pattern is a special counter that is used to limit the rate at which an operation can be performed.
//! The classical materialization of this pattern involves limiting the number of requests that can be performed against a public API.
//!
//! We provide two implementations of this pattern using INCR, where we assume that the problem to solve is limiting the number of API calls to a maximum of ten requests per second per IP address.
//!
//! Pattern: Rate limiter 1
//!
//! The more simple and direct implementation of this pattern is the following:
//!
//! FUNCTION LIMIT_API_CALL(ip)
//! ts = CURRENT_UNIX_TIME()
//! keyname = ip+":"+ts
//! MULTI
//!     INCR(keyname)
//!     EXPIRE(keyname,10)
//! EXEC
//! current = RESPONSE_OF_INCR_WITHIN_MULTI
//! IF current > 10 THEN
//!     ERROR "too many requests per second"
//! ELSE
//!     PERFORM_API_CALL()
//! END
//!
//! Basically we have a counter for every IP, for every different second. But these counters are always incremented setting an expire of 10 seconds
//! so that they'll be removed by Redis automatically when the current second is a different one.
//!
//! Note the used of MULTI and EXEC in order to make sure that we'll both increment and set the expire at every API call.
//!
//! Pattern: Rate limiter 2
//!
//! An alternative implementation uses a single counter, but is a bit more complex to get it right without race conditions. We'll examine different variants.
//!
//! FUNCTION LIMIT_API_CALL(ip):
//! current = GET(ip)
//! IF current != NULL AND current > 10 THEN
//!     ERROR "too many requests per second"
//! ELSE
//!     value = INCR(ip)
//!     IF value == 1 THEN
//!         EXPIRE(ip,1)
//!     END
//!     PERFORM_API_CALL()
//! END
//!
//! The counter is created in a way that it only will survive one second, starting from the first request performed in the current second.
//! If there are more than 10 requests in the same second the counter will reach a value greater than 10, otherwise it will expire and start again from 0.
//!
//! In the above code there is a race condition. If for some reason the client performs the INCR command but does not perform the EXPIRE the key will be leaked until we'll see the same IP address again.
//!
//! This can be fixed easily turning the INCR with optional EXPIRE into a Lua script that is send using the EVAL command (only available since Redis version 2.6).
//!
//! local current
//! current = redis.call("incr",KEYS[1])
//! if current == 1 then
//!     redis.call("expire",KEYS[1],1)
//! end
//!
//! There is a different way to fix this issue without using scripting, by using Redis lists instead of counters.
//! The implementation is more complex and uses more advanced features but has the advantage of remembering the IP addresses of the clients currently performing an API call,
//! that may be useful or not depending on the application.
//!
//! FUNCTION LIMIT_API_CALL(ip)
//! current = LLEN(ip)
//! IF current > 10 THEN
//!     ERROR "too many requests per second"
//! ELSE
//!     IF EXISTS(ip) == FALSE
//!         MULTI
//!             RPUSH(ip,ip)
//!             EXPIRE(ip,1)
//!         EXEC
//!     ELSE
//!         RPUSHX(ip,ip)
//!     END
//!     PERFORM_API_CALL()
//! END
//!
//! The RPUSHX command only pushes the element if the key already exists.
//!
//! Note that we have a race here, but it is not a problem: EXISTS may return false but the key may be created by another client before
//! we create it inside the MULTI / EXEC block. However this race will just miss an API call under rare conditions, so the rate limiting will still work correctly.
//!
//! RESP2/RESP3 Reply
//! Integer reply: the value of the key after the increment.
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
//! Redis command: INCRBY
//!
//! Syntax
//!
//! INCRBY key increment
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! Increments the number stored at key by increment. If the key does not exist, it is set to 0 before performing the operation.
//! An error is returned if the key contains a value of the wrong type or contains a string that can not be represented as integer.
//! This operation is limited to 64 bit signed integers.
//!
//! See INCR for extra information on increment/decrement operations.
//!
//! Examples
//! redis> SET mykey "10"
//! "OK"
//! redis> INCRBY mykey 5
//! (integer) 15
//!
//! RESP2/RESP3 Reply
//! Integer reply: the value of the key after the increment.
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
//! Redis command: INCRBYFLOAT
//!
//! Syntax
//!
//! INCRBYFLOAT key increment
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @string, @fast
//!
//! Increment the string representing a floating point number stored at key by the specified increment.
//! By using a negative increment value, the result is that the value stored at the key is decremented (by the obvious properties of addition).
//! If the key does not exist, it is set to 0 before performing the operation. An error is returned if one of the following conditions occur:
//!
//!     The key contains a value of the wrong type (not a string).
//!     The current key content or the specified increment are not parsable as a double precision floating point number.
//!
//! If the command is successful the new incremented value is stored as the new value of the key (replacing the old one), and returned to the caller as a string.
//!
//! Both the value already contained in the string key and the increment argument can be optionally provided in exponential notation,
//! however the value computed after the increment is stored consistently in the same format, that is, an integer number followed (if needed) by a dot,
//! and a variable number of digits representing the decimal part of the number. Trailing zeroes are always removed.
//!
//! The precision of the output is fixed at 17 digits after the decimal point regardless of the actual internal precision of the computation.
//!
//! Examples
//! redis> SET mykey 10.50
//! "OK"
//! redis> INCRBYFLOAT mykey 0.1
//! "10.6"
//! redis> INCRBYFLOAT mykey -5
//! "5.6"
//! redis> SET mykey 5.0e3
//! "OK"
//! redis> INCRBYFLOAT mykey 2.0e2
//! "5200"
//!
//! Implementation details
//!
//! The command is always propagated in the replication link and the Append Only File as a SET operation,
//! so that differences in the underlying floating point math implementation will not be sources of inconsistency.
//!
//! RESP2/RESP3 Reply
//! Bulk string reply: the value of the key after the increment.
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
//! Redis command: RENAME
//!
//! Syntax
//!
//! RENAME key newkey
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @slow
//!
//! Renames key to newkey. It returns an error when key does not exist. If newkey already exists it is overwritten,
//! when this happens RENAME executes an implicit DEL operation, so if the deleted key contains a very big value it may cause high latency even if RENAME itself is usually a constant-time operation.
//!
//! In Cluster mode, both key and newkey must be in the same hash slot, meaning that in practice only keys that have the same hash tag can be reliably renamed in cluster.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> RENAME mykey myotherkey
//! "OK"
//! redis> GET myotherkey
//! "Hello"
//!
//! Behavior change history
//!     >= 3.2.0: The command no longer returns an error when source and destination names are the same.
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
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
//! Redis command: RENAMENX
//!
//! Syntax
//!
//! RENAMENX key newkey
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! Renames key to newkey if newkey does not yet exist. It returns an error when key does not exist.
//!
//! In Cluster mode, both key and newkey must be in the same hash slot, meaning that in practice only keys that have the same hash tag can be reliably renamed in cluster.
//!
//! Examples
//! redis> SET mykey "Hello"
//! "OK"
//! redis> SET myotherkey "World"
//! "OK"
//! redis> RENAMENX mykey myotherkey
//! (integer) 0
//! redis> GET myotherkey
//! "World"
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 1 if key was renamed to newkey.
//!     Integer reply: 0 if newkey already exists.
//!
//! History
//!     Starting with Redis version 3.2.0: The command no longer returns an error when source and destination names are the same.
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
//! Redis command: TYPE
//!
//! Syntax
//!
//! TYPE key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @read, @fast
//!
//! Returns the string representation of the type of the value stored at key. The different types that can be returned are: string, list, set, zset, hash and stream.
//!
//! Examples
//! redis> SET key1 "value"
//! "OK"
//! redis> LPUSH key2 "value"
//! (integer) 1
//! redis> SADD key3 "value"
//! (integer) 1
//! redis> TYPE key1
//! "string"
//! redis> TYPE key2
//! "list"
//! redis> TYPE key3
//! "set"
//!
//! RESP2/RESP3 Reply
//! Simple string reply: the type of key, or none when key doesn't exist.
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
//! Redis command: MSET
//!
//! Syntax
//!
//! MSET key value [key value ...]
//!
//! Available since:
//!     1.0.1
//! Time complexity:
//!     O(N) where N is the number of keys to set.
//! ACL categories:
//!     @write, @string, @slow
//!
//! Sets the given keys to their respective values. MSET replaces existing values with new values, just as regular SET. See MSETNX if you don't want to overwrite existing values.
//!
//! MSET is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
//! Examples
//! redis> MSET key1 "Hello" key2 "World"
//! "OK"
//! redis> GET key1
//! "Hello"
//! redis> GET key2
//! "World"
//!
//! RESP2/RESP3 Reply
//! Simple string reply: always OK because MSET can't fail.
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
    return this->redisExecCommand(argv);
}

//!
//! \brief Установить ключи в соответствии со значениями, если ключи не существуют
//! \param keyValue Список ключ-значение
//! \return
//!
//! Redis command: MSETNX
//!
//! Syntax
//!
//! MSETNX key value [key value ...]
//!
//! Available since:
//!     1.0.1
//! Time complexity:
//!     O(N) where N is the number of keys to set.
//! ACL categories:
//!     @write, @string, @slow
//!
//! Sets the given keys to their respective values. MSETNX will not perform any operation at all even if just a single key already exists.
//!
//! Because of this semantic MSETNX can be used in order to set different keys representing different fields of a unique logic object in a way that ensures that either all the fields or none at all are set.
//!
//! MSETNX is atomic, so all given keys are set at once. It is not possible for clients to see that some of the keys were updated while others are unchanged.
//!
//! Examples
//! redis> MSETNX key1 "Hello" key2 "there"
//! (integer) 1
//! redis> MSETNX key2 "new" key3 "world"
//! (integer) 0
//! redis> MGET key1 key2 key3
//! 1) "Hello"
//! 2) "there"
//! 3) (nil)
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0 if no key was set (at least one key already existed).
//!     Integer reply: 1 if all the keys were set.
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
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить значения ключей
//! \param keyList Список ключей
//! \return
//!
//! Redis command: MGET
//!
//! Syntax
//!
//! MGET key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of keys to retrieve.
//! ACL categories:
//!     @read, @string, @fast
//!
//! Returns the values of all specified keys. For every key that does not hold a string value or does not exist, the special value nil is returned.
//! Because of this, the operation never fails.
//!
//! Examples
//! redis> SET key1 "Hello"
//! "OK"
//! redis> SET key2 "World"
//! "OK"
//! redis> MGET key1 key2 nonexisting
//! 1) "Hello"
//! 2) "World"
//! 3) (nil)
//!
//! RESP2/RESP3 Reply
//! Array reply: a list of values at the specified keys.
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
//! Redis command: MOVE
//!
//! Syntax
//!
//! MOVE key db
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @keyspace, @write, @fast
//!
//! Move key from the currently selected database (see SELECT) to the specified destination database.
//! When key already exists in the destination database, or it does not exist in the source database, it does nothing.
//! It is possible to use MOVE as a locking primitive because of this.
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 1 if key was moved.
//!     Integer reply: 0 if key wasn't moved.
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
//! Redis command: DUMP
//!
//! Syntax
//!
//! DUMP key
//!
//! Available since:
//!     2.6.0
//! Time complexity:
//!     O(1) to access the key and additional O(N*M) to serialize it, where N is the number of Redis objects composing the value and M their average size.
//!          For small string values the time complexity is thus O(1)+O(1*M) where M is small, so simply O(1).
//! ACL categories:
//!     @keyspace, @read, @slow
//!
//! Serialize the value stored at key in a Redis-specific format and return it to the user. The returned value can be synthesized back into a Redis key using the RESTORE command.
//!
//! The serialization format is opaque and non-standard, however it has a few semantic characteristics:
//!
//!     It contains a 64-bit checksum that is used to make sure errors will be detected. The RESTORE command makes sure to check the checksum before synthesizing a key using the serialized value.
//!     Values are encoded in the same format used by RDB.
//!     An RDB version is encoded inside the serialized value, so that different Redis versions with incompatible RDB formats will refuse to process the serialized value.
//!
//! The serialized value does NOT contain expire information. In order to capture the time to live of the current value the PTTL command should be used.
//!
//! If key does not exist a nil bulk reply is returned.
//!
//! Examples
//!
//! > SET mykey 10
//! OK
//! > DUMP mykey
//! "\x00\xc0\n\n\x00n\x9fWE\x0e\xaec\xbb"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: The serialized value of the key.
//!     Nil reply: the key does not exist.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Bulk string reply: the serialized value of the key.
//!     Null reply: the key does not exist.
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
//! Redis command: LINDEX
//!
//! Syntax
//!
//! LINDEX key index
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of elements to traverse to get to the element at index. This makes asking for the first or the last element of the list O(1).
//! ACL categories:
//!     @read, @list, @slow
//!
//! Returns the element at index index in the list stored at key. The index is zero-based, so 0 means the first element,
//! 1 the second element and so on. Negative indices can be used to designate elements starting at the tail of the list.
//! Here, -1 means the last element, -2 means the penultimate and so forth.
//!
//! When the value at key is not a list, an error is returned.
//!
//! Examples
//! redis> LPUSH mylist "World"
//! (integer) 1
//! redis> LPUSH mylist "Hello"
//! (integer) 2
//! redis> LINDEX mylist 0
//! "Hello"
//! redis> LINDEX mylist -1
//! "World"
//! redis> LINDEX mylist 3
//! (nil)
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: when index is out of range.
//!     Bulk string reply: the requested element.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: when index is out of range.
//!     Bulk string reply: the requested element.
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
//! Redis command: LINSERT
//!
//! Syntax
//!
//! LINSERT key <BEFORE | AFTER> pivot element
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(N) where N is the number of elements to traverse before seeing the value pivot.
//!          This means that inserting somewhere on the left end on the list (head) can be considered O(1)
//!          and inserting somewhere on the right end (tail) is O(N).
//! ACL categories:
//!     @write, @list, @slow
//!
//! Inserts element in the list stored at key either before or after the reference value pivot.
//!
//! When key does not exist, it is considered an empty list and no operation is performed.
//!
//! An error is returned when key exists but does not hold a list value.
//!
//! Examples
//! redis> RPUSH mylist "Hello"
//! (integer) 1
//! redis> RPUSH mylist "World"
//! (integer) 2
//! redis> LINSERT mylist BEFORE "World" "There"
//! (integer) 3
//! redis> LRANGE mylist 0 -1
//! 1) "Hello"
//! 2) "There"
//! 3) "World"
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: the list length after a successful insert operation.
//!     Integer reply: 0 when the key doesn't exist.
//!     Integer reply: -1 when the pivot wasn't found.
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
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить размер списка
//! \param key Ключ
//! \return
//!
//! Redis command: LLEN
//!
//! Syntax
//!
//! LLEN key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @list, @fast
//!
//! Returns the length of the list stored at key. If key does not exist, it is interpreted as an empty list and 0 is returned.
//! An error is returned when the value stored at key is not a list.
//!
//! Examples
//! redis> LPUSH mylist "World"
//! (integer) 1
//! redis> LPUSH mylist "Hello"
//! (integer) 2
//! redis> LLEN mylist
//! (integer) 2
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the list.
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
//! Redis command: LPOP
//!
//! Syntax
//!
//! LPOP key [count]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of elements returned
//! ACL categories:
//!     @write, @list, @fast
//!
//! Removes and returns the first elements of the list stored at key.
//!
//! By default, the command pops a single element from the beginning of the list.
//! When provided with the optional count argument, the reply will consist of up to count elements, depending on the list's length.
//!
//! Examples
//! redis> RPUSH mylist "one" "two" "three" "four" "five"
//! (integer) 5
//! redis> LPOP mylist
//! "one"
//! redis> LPOP mylist 2
//! 1) "two"
//! 2) "three"
//! redis> LRANGE mylist 0 -1
//! 1) "four"
//! 2) "five"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the value of the first element.
//!     Array reply: when called with the count argument, a list of popped elements.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the value of the first element.
//!     Array reply: when called with the count argument, a list of popped elements.
//!
//! History
//!     Starting with Redis version 6.2.0: Added the count argument.
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
//! Redis command: LPUSH
//!
//! Syntax
//!
//! LPUSH key element [element ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.
//! ACL categories:
//!     @write, @list, @fast
//!
//! Insert all the specified values at the head of the list stored at key.
//! If key does not exist, it is created as empty list before performing the push operations.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the head of the list, from the leftmost element to the rightmost element.
//! So for instance the command LPUSH mylist a b c will result into a list containing c as first element, b as second element and a as third element.
//!
//! Examples
//! redis> LPUSH mylist "world"
//! (integer) 1
//! redis> LPUSH mylist "hello"
//! (integer) 2
//! redis> LRANGE mylist 0 -1
//! 1) "hello"
//! 2) "world"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the list after the push operation.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple element arguments.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Вставить значение в начало списка, если ключ существует и он содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Redis command: LPUSHX
//!
//! Syntax
//!
//! LPUSHX key element [element ...]
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.
//! ACL categories:
//!     @write, @list, @fast
//!
//! Inserts specified values at the head of the list stored at key, only if key already exists and holds a list.
//! In contrary to LPUSH, no operation will be performed when key does not yet exist.
//!
//! Examples
//! redis> LPUSH mylist "World"
//! (integer) 1
//! redis> LPUSHX mylist "Hello"
//! (integer) 2
//! redis> LPUSHX myotherlist "Hello"
//! (integer) 0
//! redis> LRANGE mylist 0 -1
//! 1) "Hello"
//! 2) "World"
//! redis> LRANGE myotherlist 0 -1
//! (empty array)
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the list after the push operation.
//!
//! History
//!     Starting with Redis version 4.0.0: Accepts multiple element arguments.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LPUSHX" << key << value;
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить список значений ключа
//! \param key Ключ
//! \param start Начальный индекс
//! \param stop Конечный индекс
//! \return
//!
//! Redis command: LRANGE
//!
//! Syntax
//!
//! LRANGE key start stop
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(S+N) where S is the distance of start offset from HEAD for small lists, from nearest end (HEAD or TAIL) for large lists; and N is the number of elements in the specified range.
//! ACL categories:
//!     @read, @list, @slow
//!
//! Returns the specified elements of the list stored at key. The offsets start and stop are zero-based indexes,
//! with 0 being the first element of the list (the head of the list), 1 being the next element and so on.
//!
//! These offsets can also be negative numbers indicating offsets starting at the end of the list.
//! For example, -1 is the last element of the list, -2 the penultimate, and so on.
//!
//! Consistency with range functions in various programming languages
//!
//! Note that if you have a list of numbers from 0 to 100, LRANGE list 0 10 will return 11 elements, that is, the rightmost item is included.
//! This may or may not be consistent with behavior of range-related functions in your programming language of choice (think Ruby's Range.new, Array#slice or Python's range() function).
//!
//! Out-of-range indexes
//!
//! Out of range indexes will not produce an error. If start is larger than the end of the list, an empty list is returned.
//! If stop is larger than the actual end of the list, Redis will treat it like the last element of the list.
//!
//! Examples
//! redis> RPUSH mylist "one"
//! (integer) 1
//! redis> RPUSH mylist "two"
//! (integer) 2
//! redis> RPUSH mylist "three"
//! (integer) 3
//! redis> LRANGE mylist 0 0
//! 1) "one"
//! redis> LRANGE mylist -3 2
//! 1) "one"
//! 2) "two"
//! 3) "three"
//! redis> LRANGE mylist -100 100
//! 1) "one"
//! 2) "two"
//! 3) "three"
//! redis> LRANGE mylist 5 10
//! (empty array)
//!
//! RESP2/RESP3 Reply
//! Array reply: a list of elements in the specified range, or an empty array if the key doesn't exist.
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
//! Redis command: LREM
//!
//! Syntax
//!
//! LREM key count element
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N+M) where N is the length of the list and M is the number of elements removed.
//! ACL categories:
//!     @write, @list, @slow
//!
//! Removes the first count occurrences of elements equal to element from the list stored at key. The count argument influences the operation in the following ways:
//!
//!     count > 0: Remove elements equal to element moving from head to tail.
//!     count < 0: Remove elements equal to element moving from tail to head.
//!     count = 0: Remove all elements equal to element.
//!
//! For example, LREM list -2 "hello" will remove the last two occurrences of "hello" in the list stored at list.
//!
//! Note that non-existing keys are treated like empty lists, so when key does not exist, the command will always return 0.
//!
//! Examples
//! redis> RPUSH mylist "hello"
//! (integer) 1
//! redis> RPUSH mylist "hello"
//! (integer) 2
//! redis> RPUSH mylist "foo"
//! (integer) 3
//! redis> RPUSH mylist "hello"
//! (integer) 4
//! redis> LREM mylist -2 "hello"
//! (integer) 2
//! redis> LRANGE mylist 0 -1
//! 1) "hello"
//! 2) "foo"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of removed elements.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLRem(const QString &key, const QString &value, const int count)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LREM" << key << QString::number(count) << value;
    return this->redisExecCommand(argv);
}

//!
//! \brief Установить новое знаяение в списке по индексу
//! \param key Ключ
//! \param value Новое значение
//! \param index Индекс
//! \return
//!
//! Redis command: LSET
//!
//! Syntax
//!
//! LSET key index element
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the length of the list. Setting either the first or the last element of the list is O(1).
//! ACL categories:
//!     @write, @list, @slow
//!
//! Sets the list element at index to element. For more information on the index argument, see LINDEX.
//!
//! An error is returned for out of range indexes.
//!
//! Examples
//! redis> RPUSH mylist "one"
//! (integer) 1
//! redis> RPUSH mylist "two"
//! (integer) 2
//! redis> RPUSH mylist "three"
//! (integer) 3
//! redis> LSET mylist 0 "four"
//! "OK"
//! redis> LSET mylist -2 "five"
//! "OK"
//! redis> LRANGE mylist 0 -1
//! 1) "four"
//! 2) "five"
//! 3) "three"
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisLSet(const QString &key, const QString &value, const int index)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "LSET" << key << QString::number(index) << value;
    return this->redisExecCommand(argv);
}

//!
//! \brief Обрезать список, чтобы он содержал тоько указанный диапазон элементов
//! \param key Ключ
//! \param start Начало диапазона
//! \param stop Конец диапазона
//! \return
//!
//! Redis command: LTRIM
//!
//! Syntax
//!
//! LTRIM key start stop
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of elements to be removed by the operation.
//! ACL categories:
//!     @write, @list, @slow
//!
//! Trim an existing list so that it will contain only the specified range of elements specified.
//! Both start and stop are zero-based indexes, where 0 is the first element of the list (the head), 1 the next element and so on.
//!
//! For example: LTRIM foobar 0 2 will modify the list stored at foobar so that only the first three elements of the list will remain.
//!
//! start and end can also be negative numbers indicating offsets from the end of the list, where -1 is the last element of the list, -2 the penultimate element and so on.
//!
//! Out of range indexes will not produce an error: if start is larger than the end of the list, or start > end,
//! the result will be an empty list (which causes key to be removed). If end is larger than the end of the list, Redis will treat it like the last element of the list.
//!
//! A common use of LTRIM is together with LPUSH / RPUSH. For example:
//!
//! LPUSH mylist someelement
//! LTRIM mylist 0 99
//!
//! This pair of commands will push a new element on the list, while making sure that the list will not grow larger than 100 elements.
//! This is very useful when using Redis to store logs for example. It is important to note that when used in this way LTRIM is an O(1) operation
//! because in the average case just one element is removed from the tail of the list.
//!
//! Examples
//! redis> RPUSH mylist "one"
//! (integer) 1
//! redis> RPUSH mylist "two"
//! (integer) 2
//! redis> RPUSH mylist "three"
//! (integer) 3
//! redis> LTRIM mylist 1 -1
//! "OK"
//! redis> LRANGE mylist 0 -1
//! 1) "two"
//! 2) "three"
//!
//! RESP2/RESP3 Reply
//! Simple string reply: OK.
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
//! Redis command: RPOP
//!
//! Syntax
//!
//! RPOP key [count]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of elements returned
//! ACL categories:
//!     @write, @list, @fast
//!
//! Removes and returns the last elements of the list stored at key.
//!
//! By default, the command pops a single element from the end of the list.
//! When provided with the optional count argument, the reply will consist of up to count elements, depending on the list's length.
//!
//! Examples
//! redis> RPUSH mylist "one" "two" "three" "four" "five"
//! (integer) 5
//! redis> RPOP mylist
//! "five"
//! redis> RPOP mylist 2
//! 1) "four"
//! 2) "three"
//! redis> LRANGE mylist 0 -1
//! 1) "one"
//! 2) "two"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the value of the last element.
//!     Array reply: when called with the count argument, a list of popped elements.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the value of the last element.
//!     Array reply: when called with the count argument, a list of popped elements.
//!
//! History
//!     Starting with Redis version 6.2.0: Added the count argument.
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
//! Redis command: RPOPLPUSH (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by LMOVE with the RIGHT and LEFT arguments when migrating or writing new code.
//!
//! Syntax
//!
//! RPOPLPUSH source destination
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @list, @slow
//!
//! Atomically returns and removes the last element (tail) of the list stored at source, and pushes the element at the first element (head) of the list stored at destination.
//!
//! For example: consider source holding the list a,b,c, and destination holding the list x,y,z. Executing RPOPLPUSH results in source holding a,b and destination holding c,x,y,z.
//!
//! If source does not exist, the value nil is returned and no operation is performed. If source and destination are the same,
//! the operation is equivalent to removing the last element from the list and pushing it as first element of the list, so it can be considered as a list rotation command.
//!
//! Examples
//! redis> RPUSH mylist "one"
//! (integer) 1
//! redis> RPUSH mylist "two"
//! (integer) 2
//! redis> RPUSH mylist "three"
//! (integer) 3
//! redis> RPOPLPUSH mylist myotherlist
//! "three"
//! redis> LRANGE mylist 0 -1
//! 1) "one"
//! 2) "two"
//! redis> LRANGE myotherlist 0 -1
//! 1) "three"
//!
//! Pattern: Reliable queue
//!
//! Redis is often used as a messaging server to implement processing of background jobs or other kinds of messaging tasks.
//! A simple form of queue is often obtained pushing values into a list in the producer side, and waiting for this values in the consumer side using RPOP (using polling),
//! or BRPOP if the client is better served by a blocking operation.
//!
//! However in this context the obtained queue is not reliable as messages can be lost, for example in the case there is a network problem
//! or if the consumer crashes just after the message is received but before it can be processed.
//!
//! RPOPLPUSH (or BRPOPLPUSH for the blocking variant) offers a way to avoid this problem: the consumer fetches the message and at the same time pushes it into a processing list.
//! It will use the LREM command in order to remove the message from the processing list once the message has been processed.
//!
//! An additional client may monitor the processing list for items that remain there for too much time, pushing timed out items into the queue again if needed.
//!
//! Pattern: Circular list
//!
//! Using RPOPLPUSH with the same source and destination key, a client can visit all the elements of an N-elements list, one after the other,
//! in O(N) without transferring the full list from the server to the client using a single LRANGE operation.
//!
//! The above pattern works even if one or both of the following conditions occur:
//!
//!     There are multiple clients rotating the list: they'll fetch different elements, until all the elements of the list are visited, and the process restarts.
//!     Other clients are actively pushing new items at the end of the list.
//!
//! The above makes it very simple to implement a system where a set of items must be processed by N workers continuously as fast as possible.
//! An example is a monitoring system that must check that a set of web sites are reachable, with the smallest delay possible, using a number of parallel workers.
//!
//! Note that this implementation of workers is trivially scalable and reliable, because even if a message is lost the item is still in the queue and will be processed at the next iteration.
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: the element being popped and pushed.
//!     Nil reply: if the source list is empty.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Bulk string reply: the element being popped and pushed.
//!     Null reply: if the source list is empty.
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
//! Redis command: RPUSH
//!
//! Syntax
//!
//! RPUSH key element [element ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.
//! ACL categories:
//!     @write, @list, @fast
//!
//! Insert all the specified values at the tail of the list stored at key. If key does not exist, it is created as empty list before performing the push operation.
//! When key holds a value that is not a list, an error is returned.
//!
//! It is possible to push multiple elements using a single command call just specifying multiple arguments at the end of the command.
//! Elements are inserted one after the other to the tail of the list, from the leftmost element to the rightmost element.
//! So for instance the command RPUSH mylist a b c will result into a list containing a as first element, b as second element and c as third element.
//!
//! Examples
//! redis> RPUSH mylist "hello"
//! (integer) 1
//! redis> RPUSH mylist "world"
//! (integer) 2
//! redis> LRANGE mylist 0 -1
//! 1) "hello"
//! 2) "world"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the list after the push operation.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple element arguments.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Вставить значение в конец списка, если ключ существует и содержит список
//! \param key Ключ
//! \param value Значение
//! \return
//!
//! Redis command: RPUSHX
//!
//! Syntax
//!
//! RPUSHX key element [element ...]
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.
//! ACL categories:
//!     @write, @list, @fast
//!
//! Inserts specified values at the tail of the list stored at key, only if key already exists and holds a list.
//! In contrary to RPUSH, no operation will be performed when key does not yet exist.
//!
//! Examples
//! redis> RPUSH mylist "Hello"
//! (integer) 1
//! redis> RPUSHX mylist "World"
//! (integer) 2
//! redis> RPUSHX myotherlist "World"
//! (integer) 0
//! redis> LRANGE mylist 0 -1
//! 1) "Hello"
//! 2) "World"
//! redis> LRANGE myotherlist 0 -1
//! (empty array)
//!
//! RESP2/RESP3 Reply
//! Integer reply: the length of the list after the push operation.
//!
//! History
//!     Starting with Redis version 4.0.0: Accepts multiple element arguments.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisRPushX(const QString &key, const QString &value)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "RPUSHX" << key << value;
    return this->redisExecCommand(argv);
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
//! Redis command: SADD
//!
//! Syntax
//!
//! SADD key member [member ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1) for each element added, so O(N) to add N elements when the command is called with multiple arguments.
//! ACL categories:
//!     @write, @set, @fast
//!
//! Add the specified members to the set stored at key. Specified members that are already a member of this set are ignored.
//! If key does not exist, a new set is created before adding the specified members.
//!
//! An error is returned when the value stored at key is not a set.
//!
//! Examples
//! redis> SADD myset "Hello" "World"
//! (integer) 2
//! redis> SADD myset "World"
//! (integer) 0
//! redis> SMEMBERS myset
//! 1) "Hello"
//! 2) "World"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of elements that were added to the set, not including all the elements already present in the set.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple member arguments.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Получить количество элементов в наборе
//! \param key Ключ
//! \return
//!
//! Redis command: SCARD
//!
//! Syntax
//!
//! SCARD key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @set, @fast
//!
//! Returns the set cardinality (number of elements) of the set stored at key.
//!
//! Examples
//! redis> SADD myset "Hello"
//! (integer) 1
//! redis> SADD myset "World"
//! (integer) 1
//! redis> SCARD myset
//! (integer) 2
//!
//! RESP2 Reply
//! Integer reply: the cardinality (number of elements) of the set, or 0 if the key does not exist.
//!
//! RESP3 Reply
//! Integer reply: The cardinality (number of elements) of the set, or 0 if the key does not exist.
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
//! Redis command: SDIFF
//!
//! Syntax
//!
//! SDIFF key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of elements in all given sets.
//! ACL categories:
//!     @read, @set, @slow
//!
//! Returns the members of the set resulting from the difference between the first set and all the successive sets.
//!
//! For example:
//!
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SDIFF key1 key2 key3 = {b,d}
//!
//! Keys that do not exist are considered to be empty sets.
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SDIFF key1 key2
//! 1) "a"
//! 2) "b"
//!
//! RESP2 Reply
//! Array reply: a list with members of the resulting set.
//!
//! RESP3 Reply
//! Set reply: a set with the members of the resulting set.
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
//! Redis command: SDIFFSTORE
//!
//! Syntax
//!
//! SDIFFSTORE destination key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of elements in all given sets.
//! ACL categories:
//!     @write, @set, @slow
//!
//! This command is equal to SDIFF, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SDIFFSTORE key key1 key2
//! (integer) 2
//! redis> SMEMBERS key
//! 1) "a"
//! 2) "b"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of elements in the resulting set.
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
//! Redis command: SINTER
//!
//! Syntax
//!
//! SINTER key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N*M) worst case where N is the cardinality of the smallest set and M is the number of sets.
//! ACL categories:
//!     @read, @set, @slow
//!
//! Returns the members of the set resulting from the intersection of all the given sets.
//!
//! For example:
//!
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SINTER key1 key2 key3 = {c}
//!
//! Keys that do not exist are considered to be empty sets. With one of the keys being an empty set,
//! the resulting set is also empty (since set intersection with an empty set always results in an empty set).
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SINTER key1 key2
//! 1) "c"
//!
//! RESP2 Reply
//! Array reply: an array with the members of the resulting set.
//!
//! RESP3 Reply
//! Set reply: a set with the members of the resulting set.
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
//! Redis command: SINTERSTORE
//!
//! Syntax
//!
//! SINTERSTORE destination key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N*M) worst case where N is the cardinality of the smallest set and M is the number of sets.
//! ACL categories:
//!     @write, @set, @slow
//!
//! This command is equal to SINTER, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SINTERSTORE key key1 key2
//! (integer) 1
//! redis> SMEMBERS key
//! 1) "c"
//!
//! RESP2 Reply
//! Integer reply: the number of elements in the resulting set.
//!
//! RESP3 Reply
//! Integer reply: the number of elements in the result set.
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
//! Redis command: SISMEMBER
//!
//! Syntax
//!
//! SISMEMBER key member
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @set, @fast
//!
//! Returns if member is a member of the set stored at key.
//!
//! Examples
//! redis> SADD myset "one"
//! (integer) 1
//! redis> SISMEMBER myset "one"
//! (integer) 1
//! redis> SISMEMBER myset "two"
//! (integer) 0
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 0 if the element is not a member of the set, or when the key does not exist.
//!     Integer reply: 1 if the element is a member of the set.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisSIsMember(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "SISMEMBER" << key << member;
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить список всех элементов набора
//! \param key Ключ набора
//! \return
//!
//! Redis command: SMEMBERS
//!
//! Syntax
//!
//! SMEMBERS key
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the set cardinality.
//! ACL categories:
//!     @read, @set, @slow
//!
//! Returns all the members of the set value stored at key.
//!
//! This has the same effect as running SINTER with one argument key.
//!
//! Examples
//! redis> SADD myset "Hello"
//! (integer) 1
//! redis> SADD myset "World"
//! (integer) 1
//! redis> SMEMBERS myset
//! 1) "Hello"
//! 2) "World"
//!
//! RESP2 Reply
//! Array reply: an array with all the members of the set.
//!
//! RESP3 Reply
//! Set reply: a set with all the members of the set.
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
//! Redis command: SMOVE
//!
//! Syntax
//!
//! SMOVE source destination member
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @write, @set, @fast
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
//! Examples
//! redis> SADD myset "one"
//! (integer) 1
//! redis> SADD myset "two"
//! (integer) 1
//! redis> SADD myotherset "three"
//! (integer) 1
//! redis> SMOVE myset myotherset "two"
//! (integer) 1
//! redis> SMEMBERS myset
//! 1) "one"
//! redis> SMEMBERS myotherset
//! 1) "three"
//! 2) "two"
//!
//! RESP2/RESP3 Reply
//!
//! One of the following:
//!     Integer reply: 1 if the element is moved.
//!     Integer reply: 0 if the element is not a member of source and no operation was performed.
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
    return this->redisExecCommand(argv);
}

//!
//! \brief Удалить и вернуть один или несколько случайных элментов в наборе
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! Redis command: SPOP
//!
//! Syntax
//!
//! SPOP key [count]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     Without the count argument O(1), otherwise O(N) where N is the value of the passed count.
//! ACL categories:
//!     @write, @set, @fast
//!
//! Removes and returns one or more random members from the set value store at key.
//!
//! This operation is similar to SRANDMEMBER, that returns one or more random elements from a set but does not remove it.
//!
//! By default, the command pops a single member from the set. When provided with the optional count argument,
//! the reply will consist of up to count members, depending on the set's cardinality.
//!
//! Examples
//! redis> SADD myset "one"
//! (integer) 1
//! redis> SADD myset "two"
//! (integer) 1
//! redis> SADD myset "three"
//! (integer) 1
//! redis> SPOP myset
//! "three"
//! redis> SMEMBERS myset
//! 1) "one"
//! 2) "two"
//! redis> SADD myset "four"
//! (integer) 1
//! redis> SADD myset "five"
//! (integer) 1
//! redis> SPOP myset 3
//! 1) "one"
//! 2) "two"
//! 3) "five"
//! redis> SMEMBERS myset
//! 1) "four"
//!
//! Distribution of returned elements
//!
//! Note that this command is not suitable when you need a guaranteed uniform distribution of the returned elements.
//! For more information about the algorithms used for SPOP, look up both the Knuth sampling and Floyd sampling algorithms.
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the removed member.
//!     Array reply: when called with the count argument, a list of the removed members.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: if the key does not exist.
//!     Bulk string reply: when called without the count argument, the removed member.
//!     Array reply: when called with the count argument, a list of the removed members.
//!
//! History
//!     Starting with Redis version 3.2.0: Added the count argument.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Получить один или несколько случайных элементов из набора
//! \param key Ключ набора
//! \param count Количество элементов
//! \return
//!
//! Redis command: SRANDMEMBER
//!
//! Syntax
//!
//! SRANDMEMBER key [count]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     Without the count argument O(1), otherwise O(N) where N is the absolute value of the passed count.
//! ACL categories:
//!     @read, @set, @slow
//!
//! When called with just the key argument, return a random element from the set value stored at key.
//!
//! If the provided count argument is positive, return an array of distinct elements.
//! The array's length is either count or the set's cardinality (SCARD), whichever is lower.
//!
//! If called with a negative count, the behavior changes and the command is allowed to return the same element multiple times.
//! In this case, the number of returned elements is the absolute value of the specified count.
//!
//! Examples
//! redis> SADD myset one two three
//! (integer) 3
//! redis> SRANDMEMBER myset
//! "two"
//! redis> SRANDMEMBER myset 2
//! 1) "two"
//! 2) "three"
//! redis> SRANDMEMBER myset -5
//! 1) "two"
//! 2) "three"
//! 3) "one"
//! 4) "three"
//! 5) "two"
//!
//! Specification of the behavior when count is passed
//!
//! When the count argument is a positive value this command behaves as follows:
//!
//!     No repeated elements are returned.
//!     If count is bigger than the set's cardinality, the command will only return the whole set without additional elements.
//!     The order of elements in the reply is not truly random, so it is up to the client to shuffle them if needed.
//!
//! When the count is a negative value, the behavior changes as follows:
//!
//!     Repeating elements are possible.
//!     Exactly count elements, or an empty array if the set is empty (non-existing key), are always returned.
//!     The order of elements in the reply is truly random.
//!
//! Distribution of returned elements
//!
//! Note: this section is relevant only for Redis 5 or below, as Redis 6 implements a fairer algorithm.
//!
//! The distribution of the returned elements is far from perfect when the number of elements in the set is small,
//! this is due to the fact that we used an approximated random element function that does not really guarantees good distribution.
//!
//! The algorithm used, that is implemented inside dict.c, samples the hash table buckets to find a non-empty one.
//! Once a non empty bucket is found, since we use chaining in our hash table implementation, the number of elements inside the bucket is checked and a random element is selected.
//!
//! This means that if you have two non-empty buckets in the entire hash table, and one has three elements while one has just one,
//! the element that is alone in its bucket will be returned with much higher probability.
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: without the additional count argument, the command returns a randomly selected member, or a Nil reply when key doesn't exist.
//!     Array reply: when the optional count argument is passed, the command returns an array of members, or an empty array when key doesn't exist.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Bulk string reply: without the additional count argument, the command returns a randomly selected member, or a Null reply when key doesn't exist.
//!     Array reply: when the optional count argument is passed, the command returns an array of members, or an empty array when key doesn't exist.
//!
//! History
//!     Starting with Redis version 2.6.0: Added the optional count argument.
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
//! Redis command: SREM
//!
//! Syntax
//!
//! SREM key member [member ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the number of members to be removed.
//! ACL categories:
//!     @write, @set, @fast
//!
//! Remove the specified members from the set stored at key. Specified members that are not a member of this set are ignored.
//! If key does not exist, it is treated as an empty set and this command returns 0.
//!
//! An error is returned when the value stored at key is not a set.
//!
//! Examples
//! redis> SADD myset "one"
//! (integer) 1
//! redis> SADD myset "two"
//! (integer) 1
//! redis> SADD myset "three"
//! (integer) 1
//! redis> SREM myset "one"
//! (integer) 1
//! redis> SREM myset "four"
//! (integer) 0
//! redis> SMEMBERS myset
//! 1) "two"
//! 2) "three"
//!
//! RESP2 Reply
//! Integer reply: the number of members that were removed from the set, not including non existing members.
//!
//! RESP3 Reply
//! Integer reply: Number of members that were removed from the set, not including non existing members.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple member arguments.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Получить список элементов, полученных путем объединения всех заданных наборов
//! \param keyList Список ключей наборов
//! \return
//!
//! Redis command: SUNION
//!
//! Syntax
//!
//! SUNION key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of elements in all given sets.
//! ACL categories:
//!     @read, @set, @slow
//!
//! Returns the members of the set resulting from the union of all the given sets.
//!
//! For example:
//!
//! key1 = {a,b,c,d}
//! key2 = {c}
//! key3 = {a,c,e}
//! SUNION key1 key2 key3 = {a,b,c,d,e}
//!
//! Keys that do not exist are considered to be empty sets.
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SUNION key1 key2
//! 1) "a"
//! 2) "c"
//! 3) "b"
//! 4) "d"
//! 5) "e"
//!
//! RESP2 Reply
//! Array reply: a list with members of the resulting set.
//!
//! RESP3 Reply
//! Set reply: a set with the members of the resulting set.
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
//! Redis command: SUNIONSTORE
//!
//! Syntax
//!
//! SUNIONSTORE destination key [key ...]
//!
//! Available since:
//!     1.0.0
//! Time complexity:
//!     O(N) where N is the total number of elements in all given sets.
//! ACL categories:
//!     @write, @set, @slow
//!
//! This command is equal to SUNION, but instead of returning the resulting set, it is stored in destination.
//!
//! If destination already exists, it is overwritten.
//!
//! Examples
//! redis> SADD key1 "a"
//! (integer) 1
//! redis> SADD key1 "b"
//! (integer) 1
//! redis> SADD key1 "c"
//! (integer) 1
//! redis> SADD key2 "c"
//! (integer) 1
//! redis> SADD key2 "d"
//! (integer) 1
//! redis> SADD key2 "e"
//! (integer) 1
//! redis> SUNIONSTORE key key1 key2
//! (integer) 5
//! redis> SMEMBERS key
//! 1) "a"
//! 2) "b"
//! 3) "c"
//! 4) "d"
//! 5) "e"
//!
//! RESP2 Reply
//! Integer reply: the number of elements in the resulting set.
//!
//! RESP3 Reply
//! Integer reply: Number of the elements in the resulting set.
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
//! Redis command: ZADD
//!
//! Syntax
//!
//! ZADD key [NX | XX] [GT | LT] [CH] [INCR] score member [score member
//!   ...]
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(log(N)) for each item added, where N is the number of elements in the sorted set.
//! ACL categories:
//!     @write, @sortedset, @fast
//!
//! Adds all the specified members with the specified scores to the sorted set stored at key.
//! It is possible to specify multiple score / member pairs. If a specified member is already a member of the sorted set,
//! the score is updated and the element reinserted at the right position to ensure the correct ordering.
//!
//! If key does not exist, a new sorted set with the specified members as sole members is created, like if the sorted set was empty.
//! If the key exists but does not hold a sorted set, an error is returned.
//!
//! The score values should be the string representation of a double precision floating point number. +inf and -inf values are valid values as well.
//!
//! ZADD options
//!
//! ZADD supports a list of options, specified after the name of the key and before the first score argument. Options are:
//!
//!     XX: Only update elements that already exist. Don't add new elements.
//!     NX: Only add new elements. Don't update already existing elements.
//!     LT: Only update existing elements if the new score is less than the current score. This flag doesn't prevent adding new elements.
//!     GT: Only update existing elements if the new score is greater than the current score. This flag doesn't prevent adding new elements.
//!     CH: Modify the return value from the number of new elements added, to the total number of elements changed (CH is an abbreviation of changed).
//!         Changed elements are new elements added and elements already existing for which the score was updated.
//!         So elements specified in the command line having the same score as they had in the past are not counted.
//!         Note: normally the return value of ZADD only counts the number of new elements added.
//!     INCR: When this option is specified ZADD acts like ZINCRBY. Only one score-element pair can be specified in this mode.
//!
//! Note: The GT, LT and NX options are mutually exclusive.
//!
//!
//! Range of integer scores that can be expressed precisely
//!
//! Redis sorted sets use a double 64-bit floating point number to represent the score. In all the architectures we support, this is represented as an IEEE 754 floating point number,
//! that is able to represent precisely integer numbers between -(2^53) and +(2^53) included. In more practical terms, all the integers between -9007199254740992 and 9007199254740992 are perfectly representable.
//! Larger integers, or fractions, are internally represented in exponential form, so it is possible that you get only an approximation of the decimal number, or of the very big integer, that you set as score.
//!
//!
//! Sorted sets 101
//!
//! Sorted sets are sorted by their score in an ascending way. The same element only exists a single time, no repeated elements are permitted.
//! The score can be modified both by ZADD that will update the element score, and as a side effect, its position on the sorted set,
//! and by ZINCRBY that can be used in order to update the score relatively to its previous value.
//!
//! The current score of an element can be retrieved using the ZSCORE command, that can also be used to verify if an element already exists or not.
//!
//! For an introduction to sorted sets, see the data types page on sorted sets.
//!
//!
//! Elements with the same score
//!
//! While the same element can't be repeated in a sorted set since every element is unique, it is possible to add multiple different elements having the same score.
//! When multiple elements have the same score, they are ordered lexicographically (they are still ordered by score as a first key, however, locally,
//! all the elements with the same score are relatively ordered lexicographically).
//!
//! The lexicographic ordering used is binary, it compares strings as array of bytes.
//!
//! If the user inserts all the elements in a sorted set with the same score (for example 0), all the elements of the sorted set are sorted lexicographically,
//! and range queries on elements are possible using the command ZRANGEBYLEX (Note: it is also possible to query sorted sets by range of scores using ZRANGEBYSCORE).
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 1 "uno"
//! (integer) 1
//! redis> ZADD myzset 2 "two" 3 "three"
//! (integer) 2
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "one"
//! 2) "1"
//! 3) "uno"
//! 4) "1"
//! 5) "two"
//! 6) "2"
//! 7) "three"
//! 8) "3"
//!
//! RESP2 Reply
//!
//! Any of the following:
//!     Nil reply: if the operation was aborted because of a conflict with one of the XX/NX/LT/GT options.
//!     Integer reply: the number of new members when the CH option is not used.
//!     Integer reply: the number of new or updated members when the CH option is used.
//!     Bulk string reply: the updated score of the member when the INCR option is used.
//!
//! RESP3 Reply
//!
//! Any of the following:
//!     Null reply: if the operation was aborted because of a conflict with one of the XX/NX/LT/GT options.
//!     Integer reply: the number of new members when the CH option is not used.
//!     Integer reply: the number of new or updated members when the CH option is used.
//!     Double reply: the updated score of the member when the INCR option is used.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple elements.
//!     Starting with Redis version 3.0.2: Added the XX, NX, CH and INCR options.
//!     Starting with Redis version 6.2.0: Added the GT and LT options.
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
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить размер отсортированного набора
//! \param key Ключ
//! \return
//!
//! Redis command: ZCARD
//!
//! Syntax
//!
//! ZCARD key
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! Returns the sorted set cardinality (number of elements) of the sorted set stored at key.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZCARD myzset
//! (integer) 2
//!
//! RESP2/RESP3 Reply
//! Integer reply: the cardinality (number of members) of the sorted set, or 0 if the key doesn't exist.
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
//! Redis command: ZCOUNT
//!
//! Syntax
//!
//! ZCOUNT key min max
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(log(N)) with N being the number of elements in the sorted set.
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! Returns the number of elements in the sorted set at key with a score between min and max.
//!
//! The min and max arguments have the same semantic as described for ZRANGEBYSCORE.
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range.
//! Because of this there is no need to do a work proportional to the size of the range.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZCOUNT myzset -inf +inf
//! (integer) 3
//! redis> ZCOUNT myzset (1 3
//! (integer) 2
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of members in the specified score range.
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
//! Redis command: ZINCRBY
//!
//! Syntax
//!
//! ZINCRBY key increment member
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(log(N)) where N is the number of elements in the sorted set.
//! ACL categories:
//!     @write, @sortedset, @fast
//!
//! Increments the score of member in the sorted set stored at key by increment. If member does not exist in the sorted set,
//! it is added with increment as its score (as if its previous score was 0.0).
//! If key does not exist, a new sorted set with the specified member as its sole member is created.
//!
//! An error is returned when key exists but does not hold a sorted set.
//!
//! The score value should be the string representation of a numeric value, and accepts double precision floating point numbers.
//! It is possible to provide a negative value to decrement the score.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZINCRBY myzset 2 "one"
//! "3"
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "two"
//! 2) "2"
//! 3) "one"
//! 4) "3"
//!
//! RESP2 Reply
//! Bulk string reply: the new score of member as a double precision floating point number.
//!
//! RESP3 Reply
//! Double reply: the new score of member.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZIncrBy(const QString &key, const QString &member, const qint64 incr)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZINCRBY" << key << QString::number(incr) << member;
    return this->redisExecCommand(argv);
}

//!
//! \brief Вычислить пересечения отсортированных наборов и результат записать в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей
//! \param weightList Список значений
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Redis command: ZINTERSTORE
//!
//! Syntax
//!
//! ZINTERSTORE destination numkeys key [key ...] [WEIGHTS weight
//!   [weight ...]] [AGGREGATE <SUM | MIN | MAX>]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N*K)+O(M*log(M)) worst case with N being the smallest input sorted set, K being the number of input sorted sets and M being the number of elements in the resulting sorted set.
//! ACL categories:
//!     @write, @sortedset, @slow
//!
//! Computes the intersection of numkeys sorted sets given by the specified keys, and stores the result in destination.
//! It is mandatory to provide the number of input keys (numkeys) before passing the input keys and the other (optional) arguments.
//!
//! By default, the resulting score of an element is the sum of its scores in the sorted sets where it exists.
//! Because intersection requires an element to be a member of every given sorted set,
//! this results in the score of every element in the resulting sorted set to be equal to the number of input sorted sets.
//!
//! For a description of the WEIGHTS and AGGREGATE options, see ZUNIONSTORE.
//!
//! If destination already exists, it is overwritten.
//!
//! Examples
//! redis> ZADD zset1 1 "one"
//! (integer) 1
//! redis> ZADD zset1 2 "two"
//! (integer) 1
//! redis> ZADD zset2 1 "one"
//! (integer) 1
//! redis> ZADD zset2 2 "two"
//! (integer) 1
//! redis> ZADD zset2 3 "three"
//! (integer) 1
//! redis> ZINTERSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 2
//! redis> ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "two"
//! 4) "10"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of members in the resulting sorted set at the destination.
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
//! Redis command: ZLEXCOUNT
//!
//! Syntax
//!
//! ZLEXCOUNT key min max
//!
//! Available since:
//!     2.8.9
//! Time complexity:
//!     O(log(N)) with N being the number of elements in the sorted set.
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command returns the number of elements in the sorted set at key with a value between min and max.
//!
//! The min and max arguments have the same meaning as described for ZRANGEBYLEX.
//!
//! Note: the command has a complexity of just O(log(N)) because it uses elements ranks (see ZRANK) to get an idea of the range.
//! Because of this there is no need to do a work proportional to the size of the range.
//!
//! Examples
//! redis> ZADD myzset 0 a 0 b 0 c 0 d 0 e
//! (integer) 5
//! redis> ZADD myzset 0 f 0 g
//! (integer) 2
//! redis> ZLEXCOUNT myzset - +
//! (integer) 7
//! redis> ZLEXCOUNT myzset [b [f
//! (integer) 5
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of members in the specified score range.
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
//! Redis command: ZREMRANGEBYRANK
//!
//! Syntax
//!
//! ZREMRANGEBYRANK key start stop
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.
//! ACL categories:
//!     @write, @sortedset, @slow
//!
//! Removes all elements in the sorted set stored at key with rank between start and stop.
//! Both start and stop are 0 -based indexes with 0 being the element with the lowest score.
//! These indexes can be negative numbers, where they indicate offsets starting at the element with the highest score.
//! For example: -1 is the element with the highest score, -2 the element with the second highest score and so forth.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREMRANGEBYRANK myzset 0 1
//! (integer) 2
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "three"
//! 2) "3"
//!
//! RESP2 Reply
//! Integer reply: the number of members removed.
//!
//! RESP3 Reply
//! Integer reply: Number of members removed.
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
//! Redis command: ZRANGEBYLEX (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by ZRANGE with the BYLEX argument when migrating or writing new code.
//!
//! Syntax
//!
//! ZRANGEBYLEX key min max [LIMIT offset count]
//!
//! Available since:
//!     2.8.9
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned.
//!                 If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).
//! ACL categories:
//!     @read, @sortedset, @slow
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command returns all the elements in the sorted set at key with a value between min and max.
//!
//! If the elements in the sorted set have different scores, the returned elements are unspecified.
//!
//! The elements are considered to be ordered from lower to higher strings as compared byte-by-byte using the memcmp() C function.
//! Longer strings are considered greater than shorter strings if the common part is identical.
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! A negative count returns all elements from the offset. Keep in mind that if offset is large,
//! the sorted set needs to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! How to specify intervals
//!
//! Valid start and stop must start with ( or [, in order to specify if the range item is respectively exclusive or inclusive.
//! The special values of + or - for start and stop have the special meaning or positively infinite and negatively infinite strings,
//! so for instance the command ZRANGEBYLEX myzset - + is guaranteed to return all the elements in the sorted set, if all the elements have the same score.
//!
//! Details on strings comparison
//!
//! Strings are compared as binary array of bytes. Because of how the ASCII character set is specified,
//! this means that usually this also have the effect of comparing normal ASCII characters in an obvious dictionary way.
//! However this is not true if non plain ASCII strings are used (for example utf8 strings).
//!
//! However the user can apply a transformation to the encoded string so that the first part of the element inserted
//! in the sorted set will compare as the user requires for the specific application.
//! For example if I want to add strings that will be compared in a case-insensitive way,
//! but I still want to retrieve the real case when querying, I can add strings in the following way:
//!
//! ZADD autocomplete 0 foo:Foo 0 bar:BAR 0 zap:zap
//!
//! Because of the first normalized part in every element (before the colon character), we are forcing a given comparison,
//! however after the range is queries using ZRANGEBYLEX the application can display to the user the second part of the string, after the colon.
//!
//! The binary nature of the comparison allows to use sorted sets as a general purpose index, for example
//! the first part of the element can be a 64 bit big endian number: since big endian numbers have the most significant bytes in the initial positions,
//! the binary comparison will match the numerical comparison of the numbers. This can be used in order to implement range queries on 64 bit values.
//! As in the example below, after the first 8 bytes we can store the value of the element we are actually indexing.
//!
//! Examples
//! redis> ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis> ZRANGEBYLEX myzset - [c
//! 1) "a"
//! 2) "b"
//! 3) "c"
//! redis> ZRANGEBYLEX myzset - (c
//! 1) "a"
//! 2) "b"
//! redis> ZRANGEBYLEX myzset [aaa (g
//! 1) "b"
//! 2) "c"
//! 3) "d"
//! 4) "e"
//! 5) "f"
//!
//! RESP2/RESP3 Reply
//! Array reply: a list of elements in the specified score range.
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
//! Redis command: ZRANGEBYSCORE (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by ZRANGE with the BYSCORE argument when migrating or writing new code.
//!
//! Syntax
//!
//! ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
//!
//! Available since:
//!     1.0.5
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned.
//!                 If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).
//! ACL categories:
//!     @read, @sortedset, @slow
//!
//! Returns all the elements in the sorted set at key with a score between min and max (including elements with score equal to min or max).
//! The elements are considered to be ordered from low to high scores.
//!
//! The elements having the same score are returned in lexicographical order (this follows from a property of the sorted
//! set implementation in Redis and does not involve further computation).
//!
//! The optional LIMIT argument can be used to only get a range of the matching elements (similar to SELECT LIMIT offset, count in SQL).
//! A negative count returns all elements from the offset. Keep in mind that if offset is large, the sorted set needs
//! to be traversed for offset elements before getting to the elements to return, which can add up to O(N) time complexity.
//!
//! The optional WITHSCORES argument makes the command return both the element and its score, instead of the element alone. This option is available since Redis 2.0.
//!
//! Exclusive intervals and infinity
//!
//! min and max can be -inf and +inf, so that you are not required to know the highest or lowest score in the sorted set to get all elements from or up to a certain score.
//!
//! By default, the interval specified by min and max is closed (inclusive). It is possible to specify an open interval (exclusive) by prefixing the score with the character (. For example:
//!
//! ZRANGEBYSCORE zset (1 5
//!
//! Will return all elements with 1 < score <= 5 while:
//!
//! ZRANGEBYSCORE zset (5 (10
//!
//! Will return all the elements with 5 < score < 10 (5 and 10 excluded).
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZRANGEBYSCORE myzset -inf +inf
//! 1) "one"
//! 2) "two"
//! 3) "three"
//! redis> ZRANGEBYSCORE myzset 1 2
//! 1) "one"
//! 2) "two"
//! redis> ZRANGEBYSCORE myzset (1 2
//! 1) "two"
//! redis> ZRANGEBYSCORE myzset (1 (2
//! (empty array)
//!
//! Pattern: weighted random selection of an element
//!
//! Normally ZRANGEBYSCORE is simply used in order to get range of items where the score is the indexed integer key, however it is possible to do less obvious things with the command.
//!
//! For example a common problem when implementing Markov chains and other algorithms is to select an element at random from a set,
//! but different elements may have different weights that change how likely it is they are picked.
//!
//! This is how we use this command in order to mount such an algorithm:
//!
//! Imagine you have elements A, B and C with weights 1, 2 and 3. You compute the sum of the weights, which is 1+2+3 = 6
//!
//! At this point you add all the elements into a sorted set using this algorithm:
//!
//! SUM = ELEMENTS.TOTAL_WEIGHT // 6 in this case.
//! SCORE = 0
//! FOREACH ELE in ELEMENTS
//!     SCORE += ELE.weight / SUM
//!     ZADD KEY SCORE ELE
//! END
//!
//! This means that you set:
//!
//! A to score 0.16
//! B to score .5
//! C to score 1
//!
//! Since this involves approximations, in order to avoid C is set to, like, 0.998 instead of 1,
//! we just modify the above algorithm to make sure the last score is 1 (left as an exercise for the reader...).
//!
//! At this point, each time you want to get a weighted random element, just compute a random number between 0 and 1 (which is like calling rand() in most languages), so you can just do:
//!
//! RANDOM_ELE = ZRANGEBYSCORE key RAND() +inf LIMIT 0 1
//!
//! RESP2/RESP3 Reply
//!     Array reply: a list of the members with, optionally, their scores in the specified score range.
//!
//! History
//!     Starting with Redis version 2.0.0: Added the WITHSCORES modifier.
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
//! Redis command: ZRANK
//!
//! Syntax
//!
//! ZRANK key member [WITHSCORE]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(log(N))
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from low to high.
//! The rank (or index) is 0-based, which means that the member with the lowest score has rank 0.
//!
//! The optional WITHSCORE argument supplements the command's reply with the score of the element returned.
//!
//! Use ZREVRANK to get the rank of an element with the scores ordered from high to low.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZRANK myzset "three"
//! (integer) 2
//! redis> ZRANK myzset "four"
//! (nil)
//! redis> ZRANK myzset "three" WITHSCORE
//! 1) (integer) 2
//! 2) "3"
//! redis> ZRANK myzset "four" WITHSCORE
//! (nil)
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: if the key does not exist or the member does not exist in the sorted set.
//!     Integer reply: the rank of the member when WITHSCORE is not used.
//!     Array reply: the rank and score of the member when WITHSCORE is used.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: if the key does not exist or the member does not exist in the sorted set.
//!     Integer reply: the rank of the member when WITHSCORE is not used.
//!     Array reply: the rank and score of the member when WITHSCORE is used.
//!
//! History
//!     Starting with Redis version 7.2.0: Added the optional WITHSCORE argument.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZRANK" << key << member;
    return this->redisExecCommand(argv);
}

//!
//! \brief Удалить элемент(ы) отсортированного набора
//! \param key Ключ
//! \param members Значения
//! \return
//!
//! Redis command: ZREM
//!
//! Syntax
//!
//! ZREM key member [member ...]
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(M*log(N)) with N being the number of elements in the sorted set and M the number of elements to be removed.
//! ACL categories:
//!     @write, @sortedset, @fast
//!
//! Removes the specified members from the sorted set stored at key. Non existing members are ignored.
//!
//! An error is returned when key exists and does not hold a sorted set.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREM myzset "two"
//! (integer) 1
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "one"
//! 2) "1"
//! 3) "three"
//! 4) "3"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of members removed from the sorted set, not including non-existing members.
//!
//! History
//!     Starting with Redis version 2.4.0: Accepts multiple elements.
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

    return this->redisExecCommand(argv);
}

//!
//! \brief Когда все элементы в отсортированном наборе вставляются с одинаковой оценкой, удаляет все элементы между min и max.
//! \param key Ключ
//! \param min Мин
//! \param max Макс
//! \return
//!
//! Redis command: ZREMRANGEBYLEX
//!
//! Syntax
//!
//! ZREMRANGEBYLEX key min max
//!
//! Available since:
//!     2.8.9
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.
//! ACL categories:
//!     @write, @sortedset, @slow
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command removes all elements in the sorted set stored at key between the lexicographical range specified by min and max.
//!
//! The meaning of min and max are the same of the ZRANGEBYLEX command. Similarly,
//! this command actually removes the same elements that ZRANGEBYLEX would return if called with the same min and max arguments.
//!
//! Examples
//! redis> ZADD myzset 0 aaaa 0 b 0 c 0 d 0 e
//! (integer) 5
//! redis> ZADD myzset 0 foo 0 zap 0 zip 0 ALPHA 0 alpha
//! (integer) 5
//! redis> ZRANGE myzset 0 -1
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
//! redis> ZREMRANGEBYLEX myzset [alpha [omega
//! (integer) 6
//! redis> ZRANGE myzset 0 -1
//! 1) "ALPHA"
//! 2) "aaaa"
//! 3) "zap"
//! 4) "zip"
//!
//! RESP2 Reply
//! Integer reply: the number of members removed.
//!
//! RESP3 Reply
//! Integer reply: Number of members removed.
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
//! Redis command: ZREMRANGEBYRANK
//!
//! Syntax
//!
//! ZREMRANGEBYRANK key start stop
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.
//! ACL categories:
//!     @write, @sortedset, @slow
//!
//! Removes all elements in the sorted set stored at key with rank between start and stop.
//! Both start and stop are 0 -based indexes with 0 being the element with the lowest score.
//! These indexes can be negative numbers, where they indicate offsets starting at the element with the highest score.
//! For example: -1 is the element with the highest score, -2 the element with the second highest score and so forth.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREMRANGEBYRANK myzset 0 1
//! (integer) 2
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "three"
//! 2) "3"
//!
//! RESP2 Reply
//! Integer reply: the number of members removed.
//!
//! RESP3 Reply
//! Integer reply: Number of members removed.
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
//! Redis command: ZREMRANGEBYSCORE
//!
//! Syntax
//!
//! ZREMRANGEBYSCORE key min max
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements removed by the operation.
//! ACL categories:
//!     @write, @sortedset, @slow
//!
//! Removes all elements in the sorted set stored at key with a score between min and max (inclusive).
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREMRANGEBYSCORE myzset -inf (2
//! (integer) 1
//! redis> ZRANGE myzset 0 -1 WITHSCORES
//! 1) "two"
//! 2) "2"
//! 3) "three"
//! 4) "3"
//!
//! RESP2 Reply
//! Integer reply: the number of members removed.
//!
//! RESP3 Reply
//! Integer reply: Number of members removed.
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
//! Redis command: ZREVRANGE (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by ZRANGE with the REV argument when migrating or writing new code.
//!
//! Syntax
//!
//! ZREVRANGE key start stop [WITHSCORES]
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements returned.
//! ACL categories:
//!     @read, @sortedset, @slow
//!
//! Returns the specified range of elements in the sorted set stored at key.
//! The elements are considered to be ordered from the highest to the lowest score.
//! Descending lexicographical order is used for elements with equal score.
//!
//! Apart from the reversed ordering, ZREVRANGE is similar to ZRANGE.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREVRANGE myzset 0 -1
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis> ZREVRANGE myzset 2 3
//! 1) "one"
//! redis> ZREVRANGE myzset -2 -1
//! 1) "two"
//! 2) "one"
//!
//! RESP2/RESP3 Reply
//!     Array reply: a list of members in the specified range, optionally with their scores if WITHSCORE was used.
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
//! Redis command: ZREVRANGEBYLEX (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by ZRANGE with the REV and BYLEX arguments when migrating or writing new code.
//!
//! Syntax
//!
//! ZREVRANGEBYLEX key max min [LIMIT offset count]
//!
//! Available since:
//!     2.8.9
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned.
//!                 If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).
//! ACL categories:
//!     @read, @sortedset, @slow
//!
//! When all the elements in a sorted set are inserted with the same score, in order to force lexicographical ordering,
//! this command returns all the elements in the sorted set at key with a value between max and min.
//!
//! Apart from the reversed ordering, ZREVRANGEBYLEX is similar to ZRANGEBYLEX.
//!
//! Examples
//! redis> ZADD myzset 0 a 0 b 0 c 0 d 0 e 0 f 0 g
//! (integer) 7
//! redis> ZREVRANGEBYLEX myzset [c -
//! 1) "c"
//! 2) "b"
//! 3) "a"
//! redis> ZREVRANGEBYLEX myzset (c -
//! 1) "b"
//! 2) "a"
//! redis> ZREVRANGEBYLEX myzset (g [aaa
//! 1) "f"
//! 2) "e"
//! 3) "d"
//! 4) "c"
//! 5) "b"
//!
//! RESP2 Reply
//! Array reply: a list of members in the specified score range.
//!
//! RESP3 Reply
//! Array reply: List of the elements in the specified score range.
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
//! Redis command: ZREVRANGEBYSCORE (deprecated)
//!
//! As of Redis version 6.2.0, this command is regarded as deprecated.
//!
//! It can be replaced by ZRANGE with the REV and BYSCORE arguments when migrating or writing new code.
//!
//! Syntax
//!
//! ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
//!
//! Available since:
//!     2.2.0
//! Time complexity:
//!     O(log(N)+M) with N being the number of elements in the sorted set and M the number of elements being returned.
//!                 If M is constant (e.g. always asking for the first 10 elements with LIMIT), you can consider it O(log(N)).
//! ACL categories:
//!     @read, @sortedset, @slow
//!
//! Returns all the elements in the sorted set at key with a score between max and min (including elements with score equal to max or min).
//! In contrary to the default ordering of sorted sets, for this command the elements are considered to be ordered from high to low scores.
//!
//! The elements having the same score are returned in reverse lexicographical order.
//!
//! Apart from the reversed ordering, ZREVRANGEBYSCORE is similar to ZRANGEBYSCORE.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREVRANGEBYSCORE myzset +inf -inf
//! 1) "three"
//! 2) "two"
//! 3) "one"
//! redis> ZREVRANGEBYSCORE myzset 2 1
//! 1) "two"
//! 2) "one"
//! redis> ZREVRANGEBYSCORE myzset 2 (1
//! 1) "two"
//! redis> ZREVRANGEBYSCORE myzset (2 (1
//! (empty array)
//!
//! RESP2/RESP3 Reply
//!     Array reply: a list of the members and, optionally, their scores in the specified score range.
//!
//! History
//!     Starting with Redis version 2.1.6: min and max can be exclusive.
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
//! Redis command: ZREVRANK
//!
//! Syntax
//!
//! ZREVRANK key member [WITHSCORE]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(log(N))
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! Returns the rank of member in the sorted set stored at key, with the scores ordered from high to low.
//! The rank (or index) is 0-based, which means that the member with the highest score has rank 0.
//!
//! The optional WITHSCORE argument supplements the command's reply with the score of the element returned.
//!
//! Use ZRANK to get the rank of an element with the scores ordered from low to high.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZADD myzset 2 "two"
//! (integer) 1
//! redis> ZADD myzset 3 "three"
//! (integer) 1
//! redis> ZREVRANK myzset "one"
//! (integer) 2
//! redis> ZREVRANK myzset "four"
//! (nil)
//! redis> ZREVRANK myzset "three" WITHSCORE
//! 1) (integer) 0
//! 2) "3"
//! redis> ZREVRANK myzset "four" WITHSCORE
//! (nil)
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Nil reply: if the key does not exist or the member does not exist in the sorted set.
//!     Integer reply: The rank of the member when WITHSCORE is not used.
//!     Array reply: The rank and score of the member when WITHSCORE is used.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Null reply: if the key does not exist or the member does not exist in the sorted set.
//!     Integer reply: The rank of the member when WITHSCORE is not used.
//!     Array reply: The rank and score of the member when WITHSCORE is used.
//!
//! History
//!     Starting with Redis version 7.2.0: Added the optional WITHSCORE argument.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZRevRank(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZREVRANK" << key << member;
    return this->redisExecCommand(argv);
}

//!
//! \brief Получить счет элемента отсортированного набора
//! \param key Ключ
//! \param member Значение
//! \return
//!
//! Redis command: ZSCORE
//!
//! Syntax
//!
//! ZSCORE key member
//!
//! Available since:
//!     1.2.0
//! Time complexity:
//!     O(1)
//! ACL categories:
//!     @read, @sortedset, @fast
//!
//! Returns the score of member in the sorted set at key.
//!
//! If member does not exist in the sorted set, or key does not exist, nil is returned.
//!
//! Examples
//! redis> ZADD myzset 1 "one"
//! (integer) 1
//! redis> ZSCORE myzset "one"
//! "1"
//!
//! RESP2 Reply
//!
//! One of the following:
//!     Bulk string reply: the score of the member (a double-precision floating point number), represented as a string.
//!     Nil reply: if member does not exist in the sorted set, or the key does not exist.
//!
//! RESP3 Reply
//!
//! One of the following:
//!     Double reply: the score of the member (a double-precision floating point number).
//!     Nil reply: if member does not exist in the sorted set, or the key does not exist.
//!
template<typename __CLIENT_IMPL, typename __RESULT_IMPL>
__RESULT_IMPL QtRedisBase<__CLIENT_IMPL, __RESULT_IMPL>::redisZScore(const QString &key, const QString &member)
{
    if (key.isEmpty() || key.indexOf(" ") != -1)
        return make_error("Invalid key!");

    QStringList argv;
    argv << "ZSCORE" << key << member;
    return this->redisExecCommand(argv);
}

//!
//! \brief Вычислить объединение отсортированных наборов и сохранить результат в новый набор
//! \param destKey Название ключа нового набора
//! \param keyList Список ключей наборов
//! \param weightList Список "весов"
//! \param aggregateFlag Флаг агрегации (SUM|MIN|MAX)
//! \return
//!
//! Redis command: ZUNIONSTORE
//!
//! Syntax
//!
//! ZUNIONSTORE destination numkeys key [key ...] [WEIGHTS weight
//!   [weight ...]] [AGGREGATE <SUM | MIN | MAX>]
//!
//! Available since:
//!     2.0.0
//! Time complexity:
//!     O(N)+O(M log(M)) with N being the sum of the sizes of the input sorted sets, and M being the number of elements in the resulting sorted set.
//! ACL categories:
//!     @write, @sortedset, @slow
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
//! Examples
//! redis> ZADD zset1 1 "one"
//! (integer) 1
//! redis> ZADD zset1 2 "two"
//! (integer) 1
//! redis> ZADD zset2 1 "one"
//! (integer) 1
//! redis> ZADD zset2 2 "two"
//! (integer) 1
//! redis> ZADD zset2 3 "three"
//! (integer) 1
//! redis> ZUNIONSTORE out 2 zset1 zset2 WEIGHTS 2 3
//! (integer) 3
//! redis> ZRANGE out 0 -1 WITHSCORES
//! 1) "one"
//! 2) "5"
//! 3) "three"
//! 4) "9"
//! 5) "two"
//! 6) "10"
//!
//! RESP2/RESP3 Reply
//! Integer reply: the number of elements in the resulting sorted set.
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

#ifndef QTREDISREPLY_H
#define QTREDISREPLY_H

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QDebug>
#include <utility>

//!
//! \file QtRedisReply.h
//! \class QtRedisReply
//! \brief Класс, описывающий ответ от сервера Redis
//!
//! Если тип:
//! - String  - QtRedisReply.strValue()
//! - Array   - QtRedisReply.arrayValue()
//! - Integer - QtRedisReply.intValue()
//! - Nil     - nothing
//! - Status  - QtRedisReply.strValue()
//! - Error   - QtRedisReply.strValue()
//!
class QtRedisReply
{
public:
    //!
    //! \brief Перечисление типов объекта
    //!
    enum class ReplyType {
        String = 0,   //!< строка
        Array,        //!< массив
        Integer,      //!< число
        Nil,          //!< нулевой объект
        Status,       //!< состояние
        Error         //!< ошибка
    };

    //!
    //! \brief Конструктор класса
    //! \param type Тип объекта
    //!
    explicit QtRedisReply(const ReplyType &type = ReplyType::Nil)
        : _type (type) {
    }

    //!
    //! \brief Деструктор класса
    //!
    ~QtRedisReply() {
        _rawValue.clear();
        _arrayValue.clear();
    }

    //!
    //! \brief Конструктор копирования
    //! \param object Объект копирования
    //!
    QtRedisReply(const QtRedisReply &object)
        : _type (object.type())
        , _rawValue (object.rawValue_ref())
        , _arrayValue (object.arrayValue_ref()) {
    }

    //!
    //! \brief Оператор присваивания
    //! \param object Класс элемента
    //! \return
    //!
    QtRedisReply& operator=(const QtRedisReply &object) {
        if (this == &object)
            return *this;
        _type = object.type();
        _rawValue = object.rawValue_ref();
        _arrayValue = object.arrayValue_ref();
        return *this;
    }

    //!
    //! \brief Строковое представление типа объекта
    //! \param type Тип объекта
    //! \return
    //!
    static QString typeToStr(const ReplyType &type) {
        switch (type) {
            case ReplyType::String:
                return QString("String");
            case ReplyType::Array:
                return QString("Array");
            case ReplyType::Integer:
                return QString("Integer");
            case ReplyType::Nil:
                return QString("Nil");
            case ReplyType::Status:
                return QString("Status");
            case ReplyType::Error:
                return QString("Error");
            default:
                break;
        }
        return QString();
    }

    //!
    //! \brief Содержит ли значение результата
    //! \return
    //!
    bool isValue() const {
        return (_type == ReplyType::String
                || _type == ReplyType::Array
                || _type == ReplyType::Integer);
    }

    //!
    //! \brief Является ли типом ReplyType::Status
    //! \return
    //!
    bool isStatus() const {
        return (_type == ReplyType::Status);
    }

    //!
    //! \brief Является ли типом ReplyType::Nil
    //! \return
    //!
    bool isNil() const {
        return (_type == ReplyType::Nil);
    }

    //!
    //! \brief Является ли типом ReplyType::Error
    //! \return
    //!
    bool isError() const {
        return (_type == ReplyType::Error);
    }

    //!
    //! \brief Является ли типом ReplyType::String
    //! \return
    //!
    bool isString() const {
        return (_type == ReplyType::String);
    }

    //!
    //! \brief Является ли типом ReplyType::Array
    //! \return
    //!
    bool isArray() const {
        return (_type == ReplyType::Array);
    }

    //!
    //! \brief Является ли типом ReplyType::Integer
    //! \return
    //!
    bool isInteger() const {
        return (_type == ReplyType::Integer);
    }

    //!
    //! \brief Является ли пустым объектом
    //! \return
    //!
    bool isEmpty() const {
        return (_type != ReplyType::Nil
                && _rawValue.isEmpty());
    }

    //!
    //! \brief Тип объекта
    //! \return
    //!
    ReplyType type() const { return _type; }

    //!
    //! \brief "Сырое" значение
    //! \return
    //!
    //! Note: Return copy of value!
    //!
    QByteArray rawValue() const { return _rawValue; }

    //!
    //! \brief "Сырое" значение
    //! \return
    //!
    //! Note: Return reference value!
    //!
    const QByteArray &rawValue_ref() const { return _rawValue; }

    //!
    //! \brief Строковое значение
    //! \return
    //!
    QString strValue() const {
        if (_type == ReplyType::Error
            || _type == ReplyType::Status
            || _type == ReplyType::String)
            return QString::fromUtf8(_rawValue.constData());

        return QString();
    }

    //!
    //! \brief Числовое значение
    //! \return
    //!
    qlonglong intValue() const {
        if (_type == ReplyType::Integer)
            return _rawValue.toLongLong();

        return 0;
    }

    //!
    //! \brief Размер массива значений
    //! \return
    //!
    int arrayValueSize() const {
        if (_type == ReplyType::Array)
            return _arrayValue.size();

        return -1;
    }

    //!
    //! \brief Пуст ли массив значений
    //! \return
    //!
    bool isArrayValueEmpty() const {
        return _arrayValue.isEmpty();
    }

    //!
    //! \brief Массив значений
    //! \return
    //!
    //! Note: Return copy of value!
    //!
    QVector<QtRedisReply> arrayValue() const {
        if (_type == ReplyType::Array)
            return _arrayValue;

        return QVector<QtRedisReply>();
    }

    //!
    //! \brief Массив значений
    //! \return
    //!
    //! Note: Not check type! Return reference value!
    //!
    const QVector<QtRedisReply> &arrayValue_ref() const {
        return _arrayValue;
    }

    //!
    //! \brief Значение из массива значений
    //! \param index Индекс
    //! \return
    //!
    //! Note: Return copy of value!
    //!
    QtRedisReply arrayValueAt(const int index) const {
        if (index < 0
            || _type != ReplyType::Array
            || _arrayValue.size() <= index)
            return QtRedisReply();

        return _arrayValue.at(index);
    }

    //!
    //! \brief Значение из массива значений
    //! \param index Индекс
    //! \return
    //!
    //! Note: Not check type! Return reference value!
    //! Warn: index must be a valid index position in the _arrayValue vector (i.e., 0 <= index < size())!
    //!
    const QtRedisReply &arrayValueAt_ref(const int index) const {
        return _arrayValue.at(index);
    }

    //!
    //! \brief Первое значение из массива значений
    //! \return
    //!
    //! Note: Return copy of value!
    //!
    QtRedisReply arrayValueFirst() const {
        if (_type != ReplyType::Array
            || _arrayValue.isEmpty())
            return QtRedisReply();

        return _arrayValue.constFirst();
    }

    //!
    //! \brief Первое значение из массива значений
    //! \return
    //!
    //! Note: Not check type! Return reference value!
    //! Warn: This function assumes that the _arrayValue vector isn't empty!
    //!
    const QtRedisReply &arrayValueFirst_ref() const {
        return _arrayValue.constFirst();
    }

    //!
    //! \brief Последнее значение из массива значений
    //! \return
    //!
    //! Note: Return copy of value!
    //!
    QtRedisReply arrayValueLast() const {
        if (_type != ReplyType::Array
            || _arrayValue.isEmpty())
            return QtRedisReply();

        return _arrayValue.constLast();
    }

    //!
    //! \brief Последнее значение из массива значений
    //! \return
    //!
    //! Note: Not check type! Return reference value!
    //! Warn: This function assumes that the _arrayValue vector isn't empty!
    //!
    const QtRedisReply &arrayValueLast_ref() const {
        return _arrayValue.constLast();
    }

    //!
    //! \brief Задать тип объекта
    //! \param type Тип объекта
    //!
    void setType(const ReplyType &type) { _type = type; }

    //!
    //! \brief Задать "сырое" значение
    //! \param value "Сырое" значение
    //!
    void setRawValue(const QByteArray &value) { _rawValue = value; }

    //!
    //! \brief Добавить элемент в массив
    //! \param object Объект ответа
    //!
    void appendArrayValue(const QtRedisReply &object) { _arrayValue.append(std::move(object)); }

    //!
    //! \brief operator <<
    //! \param dbg QDebug
    //! \param object QtRedisReply
    //! \return
    //!
    friend QDebug operator<<(QDebug dbg, const QtRedisReply &object) {
        dbg.nospace() << "QtRedisReply[ type: "
                      << QtRedisReply::typeToStr(object.type())
                      << "; raw: \""
                      << object.rawValue_ref().constData()
                      << "\"; to-str: \""
                      << object.strValue().toStdString().c_str()
                      << "\"; to-int: "
                      << object.intValue()
                      << "; to-array: "
                      << object.arrayValue()
                      << "]";
        return dbg.space();
    }


    // ------------------------------------------------------------------------
    // -- TOOLS COMMANDS ------------------------------------------------------
    // ------------------------------------------------------------------------

    //!
    //! \brief Проверить результат на корректность
    //! \param reply
    //! \return
    //!
    //! Данный метод проверяет тип результирующего объекта, и если он:
    //! - replyType_Error   - return false
    //! - replyType_Nil     - return false
    //! - replyType_Status  - если ответ != "OK" и != "QUEUED", то return false
    //!
    static bool isReplySuccess(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Error
            || reply.type() == QtRedisReply::ReplyType::Nil)
            return false;
        else if (reply.type() == QtRedisReply::ReplyType::Status
                 && reply.strValue() != "OK"
                 && reply.strValue() != "QUEUED")
            return false;
        else if (reply.type() == QtRedisReply::ReplyType::Array) {
            for (const QtRedisReply &rpl : reply.arrayValue_ref()) {
                if (!QtRedisReply::isReplySuccess(rpl))
                    return false;
            }
        }
        return true;
    }

    //!
    //! \brief Преобразовать ответ от сервера в qlonglong
    //! \param reply Ответ от сервера
    //! \return
    //!
    static qlonglong replyToLong(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Integer)
            return reply.intValue();
        else if (reply.type() == QtRedisReply::ReplyType::Array && !reply.isArrayValueEmpty())
            return QtRedisReply::replyToLong(reply.arrayValueFirst_ref());

        return -1;
    }

    //!
    //! \brief Преобразовать ответ от сервера в строку
    //! \param reply Ответ от сервера
    //! \return
    //!
    static QString replyToString(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::String
            || reply.type() == QtRedisReply::ReplyType::Status)
            return reply.strValue();
        else if (reply.type() == QtRedisReply::ReplyType::Array && !reply.isArrayValueEmpty())
            return QtRedisReply::replyToString(reply.arrayValueFirst_ref());

        return QString();
    }

    //!
    //! \brief Преобразовать ответ от сервера в строку
    //! \param reply Ответ от сервера
    //! \return
    //!
    static QByteArray replyToByteArray(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::String
            || reply.type() == QtRedisReply::ReplyType::Status)
            return reply.rawValue_ref();
        else if (reply.type() == QtRedisReply::ReplyType::Array && !reply.isArrayValueEmpty())
            return QtRedisReply::replyToByteArray(reply.arrayValueFirst_ref());

        return QByteArray();
    }

    //!
    //! \brief Преобразовать ответ от сервера в список
    //! \param reply Ответ от сервера
    //! \param singleValueForNonArray Добавить в список единичный объект, если reply не является массивом
    //! \return
    //!
    static QStringList replyToStringList(const QtRedisReply &reply, const bool singleValueForNonArray = false)
    {
        if (reply.type() != QtRedisReply::ReplyType::Array) {
            if (singleValueForNonArray)
                return QStringList({reply.strValue()});

            return QStringList();
        }

        QStringList array;
        for (const QtRedisReply &replyObj : reply.arrayValue_ref()) {
            if (replyObj.type() == QtRedisReply::ReplyType::String) {
                array.append(replyObj.strValue());
            } else if (reply.type() == QtRedisReply::ReplyType::Array) {
                const QStringList tmpLst = QtRedisReply::replyToStringList(reply, singleValueForNonArray);
                if (!tmpLst.isEmpty())
                    array.append(tmpLst);
            }
        }
        return array;
    }

    //!
    //! \brief Преобразовать ответ от сервера в список
    //! \param reply Ответ от сервера
    //! \param singleValueForNonArray Добавить в список единичный объект, если reply не является массивом
    //! \return
    //!
    static QList<QByteArray> replyToByteArrayList(const QtRedisReply &reply, const bool singleValueForNonArray = false)
    {
        if (reply.type() != QtRedisReply::ReplyType::Array) {
            if (singleValueForNonArray)
                return QList<QByteArray>({reply.rawValue_ref()});

            return QList<QByteArray>();
        }

        QList<QByteArray> array;
        for (const QtRedisReply &replyObj : reply.arrayValue_ref()) {
            if (replyObj.type() == QtRedisReply::ReplyType::String) {
                array.append(replyObj.rawValue_ref());
            } else if (reply.type() == QtRedisReply::ReplyType::Array) {
                const QList<QByteArray> tmpLst = QtRedisReply::replyToByteArrayList(reply, singleValueForNonArray);
                if (!tmpLst.isEmpty())
                    array.append(tmpLst);
            }
        }
        return array;
    }

    //!
    //! \brief Преобразовать ответ от сервера (replyType_Integer) в bool
    //! \param reply Ответ от сервера
    //! \return
    //!
    static bool replyIntToBool(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Integer)
            return (reply.intValue() == 0) ? false : true;
        else if (reply.type() == QtRedisReply::ReplyType::Array && !reply.isArrayValueEmpty())
            return QtRedisReply::replyIntToBool(reply.arrayValueFirst_ref());

        return false;
    }

    //!
    //! \brief Преобразовать ответ от сервера (replyType_Status|replyType_String) в bool
    //! \param reply Ответ от сервера
    //! \return
    //!
    static bool replySimpleStringToBool(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Status
            || reply.type() == QtRedisReply::ReplyType::String)
            return (reply.strValue() == "OK" || reply.strValue() == "QUEUED");
        else if (reply.type() == QtRedisReply::ReplyType::Array && !reply.isArrayValueEmpty())
            return QtRedisReply::replySimpleStringToBool(reply.arrayValueFirst_ref());

        return false;
    }

protected:
    ReplyType               _type {ReplyType::Nil}; //!< тип объекта
    QByteArray              _rawValue;              //!< "сырое" значение
    QVector<QtRedisReply>   _arrayValue;            //!< массив значений
};

#endif // QTREDISREPLY_H

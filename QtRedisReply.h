#ifndef QTREDISREPLY_H
#define QTREDISREPLY_H

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QDebug>

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
        , _rawValue (object.rawValue())
        , _arrayValue (object.arrayValue()) {
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
        _rawValue = object.rawValue();
        _arrayValue = object.arrayValue();
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
    //! \brief Содержит ли значение статуса
    //! \return
    //!
    bool isStatus() const {
        return (_type == ReplyType::Status);
    }

    //!
    //! \brief Содержит ли значение Nil
    //! \return
    //!
    bool isNil() const {
        return (_type == ReplyType::Nil);
    }

    //!
    //! \brief Содержит ли значение Error
    //! \return
    //!
    bool isError() const {
        return (_type == ReplyType::Error);
    }

    bool isString() const {
        return (_type == ReplyType::String);
    }

    bool isArray() const {
        return (_type == ReplyType::Array);
    }

    bool isInteger() const {
        return (_type == ReplyType::Integer);
    }

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
    QByteArray rawValue() const { return _rawValue; }

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
    //! \brief Массив значений
    //! \return
    //!
    QVector<QtRedisReply> arrayValue() const {
        if (_type == ReplyType::Array)
            return _arrayValue;

        return QVector<QtRedisReply>();
    }

    const QVector<QtRedisReply> &arrayValue_ref() const {
        return _arrayValue;
    }

    //!
    //! \brief Значиение из массива значений
    //! \param index Индекс
    //! \return
    //!
    QtRedisReply arrayValueAt(const int index) const {
        if (index < 0
            || _type != ReplyType::Array
            || _arrayValue.size() <= index)
            return QtRedisReply();

        return _arrayValue[index];
    }

    QtRedisReply arrayValueFirst() const {
        if (_type != ReplyType::Array
            || _arrayValue.isEmpty())
            return QtRedisReply();

        return _arrayValue.constFirst();
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
    void appendArrayValue(const QtRedisReply &object) { _arrayValue.append(object); }

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
                      << object.rawValue().constData()
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
    //! - replyType_Status  - если ответ != "OK", то return false
    //!
    static bool isReplySuccess(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Error
            || reply.type() == QtRedisReply::ReplyType::Nil)
            return false;
        else if (reply.type() == QtRedisReply::ReplyType::Status
                 && reply.strValue() != "OK")
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
        else if (reply.type() == QtRedisReply::ReplyType::Array)
            return QtRedisReply::replyToLong(reply.arrayValueFirst());

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
        else if (reply.type() == QtRedisReply::ReplyType::Array)
            return QtRedisReply::replyToString(reply.arrayValueFirst());

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
            return reply.rawValue();
        else if (reply.type() == QtRedisReply::ReplyType::Array)
            return QtRedisReply::replyToByteArray(reply.arrayValueFirst());

        return QByteArray();
    }

    //!
    //! \brief Преобразовать ответ от сервера в список
    //! \param reply Ответ от сервера
    //! \return
    //!
    static QStringList replyToStringList(const QtRedisReply &reply)
    {
        if (reply.type() == QtRedisReply::ReplyType::Array) {
            QStringList array;
            for (const QtRedisReply &replyObj : reply.arrayValue_ref()) {
                if (replyObj.type() == QtRedisReply::ReplyType::String) {
                    array.append(replyObj.strValue());
                } else if (reply.type() == QtRedisReply::ReplyType::Array) {
                    const QStringList tmpLst = QtRedisReply::replyToStringList(reply);
                    if (!tmpLst.isEmpty())
                        array.append(tmpLst);
                }
            }
            return array;
        }
        return QStringList();
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
        else if (reply.type() == QtRedisReply::ReplyType::Array)
            return QtRedisReply::replyIntToBool(reply.arrayValueFirst());

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
            return (reply.strValue() == "OK");
        else if (reply.type() == QtRedisReply::ReplyType::Array)
            return QtRedisReply::replySimpleStringToBool(reply.arrayValueFirst());

        return false;
    }

protected:
    ReplyType               _type {ReplyType::Nil}; //!< тип объекта
    QByteArray              _rawValue;              //!< "сырое" значение
    QVector<QtRedisReply>   _arrayValue;            //!< массив значений
};

#endif // QTREDISREPLY_H

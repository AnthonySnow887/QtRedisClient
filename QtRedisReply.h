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
                      << "; raw: "
                      << object.rawValue().constData()
                      << "; str: "
                      << object.strValue()
                      << "; int: "
                      << object.intValue()
                      << "; array: "
                      << object.arrayValue()
                      << "]";
        return dbg.space();
    }

protected:
    ReplyType               _type {ReplyType::Nil}; //!< тип объекта
    QByteArray              _rawValue;              //!< "сырое" значение
    QVector<QtRedisReply>   _arrayValue;            //!< массив значений
};

#endif // QTREDISREPLY_H

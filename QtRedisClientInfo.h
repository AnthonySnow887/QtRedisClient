#ifndef QTREDISCLIENTINFO_H
#define QTREDISCLIENTINFO_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QDebug>

//!
//! \file QtRedisClientInfo.h
//! \class QtRedisClientInfo
//! \brief Класс, описывающий информацию о клиенте Redis-a
//!
class QtRedisClientInfo
{
public:
    //!
    //! \brief Конструктор класса
    //!
    QtRedisClientInfo() = default;

    //!
    //! \brief Конструктор класса
    //! \param info Информация
    //!
    explicit QtRedisClientInfo(const QMap <QString, QVariant> &info)
        : _info (info) {
    }

    //!
    //! \brief Деструктор класса
    //!
    ~QtRedisClientInfo() {
        _info.clear();
    }

    //!
    //! \brief Конструктор копирования
    //! \param object Объект копирования
    //!
    QtRedisClientInfo(const QtRedisClientInfo &object)
        : _info (object.info()) {
    }

    //!
    //! \brief Оператор присваивания
    //! \param object Класс элемента
    //! \return
    //!
    QtRedisClientInfo& operator=(const QtRedisClientInfo &object) {
        if (this == &object)
            return *this;
        _info = object.info();
        return *this;
    }

    //!
    //! \brief Информация
    //! \return
    //!
    QMap <QString, QVariant> info() const { return _info; }

    //!
    //! \brief operator <<
    //! \param dbg QDebug
    //! \param object QtRedisClientInfo
    //! \return
    //!
    friend QDebug operator<<(QDebug dbg, const QtRedisClientInfo &object) {
        dbg.nospace() << "QtRedisClientInfo[ info: "
                      << object.info()
                      << "]";
        return dbg.space();
    }

protected:
    QMap <QString, QVariant> _info; //!< информация
};

#endif // QTREDISCLIENTINFO_H

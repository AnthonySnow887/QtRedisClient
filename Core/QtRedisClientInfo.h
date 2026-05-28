#ifndef QTREDISCLIENTINFO_H
#define QTREDISCLIENTINFO_H

#include <QMap>
#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QList>
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
    //! \brief Информация
    //! \return
    //!
    const QMap <QString, QVariant> &info_ref() const { return _info; }

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

    //!
    //! \brief Получить список QtRedisClientInfo из "сырого" формата данных
    //! \param data "Сырой" формат данных
    //! \return
    //!
    static QList<QtRedisClientInfo> fromStringData(const QString &data) {
        if (data.isEmpty())
            return QList<QtRedisClientInfo>();
        QList<QtRedisClientInfo> buffList;
        QStringList buffReplyList = data.split("\n");
        for (int i = 0; i < buffReplyList.size(); i++) {
            buffReplyList[i] = buffReplyList[i].trimmed();
            if (buffReplyList[i].isEmpty())
                continue;

            QMap<QString, QVariant> buffInfo;
            const QStringList buffSplitValue = buffReplyList[i].split(" ");
            for (const QString &str : buffSplitValue) {
                const QStringList strSplit = str.split("=");
                if (strSplit.size() != 2)
                    continue;
                buffInfo.insert(strSplit.at(0), strSplit.at(1));
            }
            buffList.append(QtRedisClientInfo(buffInfo));
        }
        return buffList;
    }

    //!
    //! \brief Получить список QtRedisClientInfo из строкового формата данных
    //! \param data Строковый формат данных
    //! \return
    //!
    static QList<QtRedisClientInfo> fromRawData(const QByteArray &data) {
        if (data.isEmpty())
            return QList<QtRedisClientInfo>();
        QList<QtRedisClientInfo> buffList;
        QList<QByteArray> buffReplyList = data.split('\n');
        for (int i = 0; i < buffReplyList.size(); i++) {
            buffReplyList[i] = buffReplyList[i].trimmed();
            if (buffReplyList[i].isEmpty())
                continue;

            QMap<QString, QVariant> buffInfo;
            const QList<QByteArray> buffSplitValue = buffReplyList[i].split(' ');
            for (const QByteArray &str : buffSplitValue) {
                const QList<QByteArray> strSplit = str.split('=');
                if (strSplit.size() != 2)
                    continue;
                buffInfo.insert(QString::fromUtf8(strSplit.at(0)), strSplit.at(1));
            }
            buffList.append(QtRedisClientInfo(buffInfo));
        }
        return buffList;
    }

protected:
    QMap <QString, QVariant> _info; //!< информация
};

#endif // QTREDISCLIENTINFO_H

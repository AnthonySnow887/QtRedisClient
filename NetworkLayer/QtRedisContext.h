#ifndef QTREDISCONTEXT_H
#define QTREDISCONTEXT_H

#include <QObject>

//!
//! \file QtRedisContext.h
//! \class QtRedisContext
//! \brief Класс, описывающий контекст Redis-a
//!
class QtRedisContext : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtRedisContext)

public:
    //!
    //! \brief Конструктор класса
    //!
    QtRedisContext() : QObject()
    {}

    //!
    //! \brief Конструктор класса
    //! \param host Хост
    //! \param port Порт
    //!
    explicit QtRedisContext(const QString &host, const uint port)
        : _host (host)
        , _port (port)
    {}

    //!
    //! \brief Деструктор класса
    //!
    virtual ~QtRedisContext() {
        _host.clear();
    }

    //!
    //! \brief Получить хост
    //! \return
    //!
    QString host() const { return _host; }

    //!
    //! \brief Получить порт
    //! \return
    //!
    int port() const { return _port; }

    //!
    //! \brief Текущий индекс БД
    //! \return
    //!
    int currentDbIndex() const { return _currentDbIndex; }

    //!
    //! \brief Задать текущий индекс БД
    //! \param index Индекс БД
    //!
    void setCurrentDbIndex(const int index) { _currentDbIndex = index; }

    //!
    //! \brief Подключиться к серверу
    //! \param msecs Время ожидания мсек
    //! \return
    //!
    virtual bool connectToServer(const int msecs = 30000) = 0;

    //!
    //! \brief Переподключиться к серверу
    //! \param msecs Время ожидания мсек
    //! \return
    //!
    virtual bool reconnectToServer(const int msecs = 30000) = 0;

    //!
    //! \brief Отключиться от сервера
    //!
    virtual void disconnectFromServer() = 0;

    //!
    //! \brief Подключен ли к серверу
    //! \return
    //!
    virtual bool isConnected() = 0;

    //!
    //! \brief Доступны ли данные для чтения
    //! \return
    //!
    virtual bool canReadRawData() const = 0;

    //!
    //! \brief Количество байт, доступных для чтения
    //! \return
    //!
    virtual qint64 bytesAvailable() const = 0;

    //!
    //! \brief Записать данные
    //! \param data Данные
    //! \return
    //!
    virtual qint64 writeRawData(const QByteArray &data) = 0;

    //!
    //! \brief Прочитать данные
    //! \return
    //!
    virtual QByteArray readRawData() = 0;

    //!
    //! \brief Ожидать поступления данных
    //! \param msecs Время ожидания мсек
    //! \return
    //!
    virtual bool waitForReadyRead(const int msecs = 30000) = 0;

protected:
    QString _host;                  //!< хост
    uint    _port {0};              //!< порт
    int     _currentDbIndex {0};    //!< текущий индекс БД
};

#endif // QTREDISCONTEXT_H

#ifndef QTREDISCOMMAND_H
#define QTREDISCOMMAND_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QStringList>

//!
//! \file QtRedisCommand.h
//! \class QtRedisCommand
//! \brief Класс, описывающий команду для Redis-a
//!
class QtRedisCommand
{
public:
    QtRedisCommand() {}
    ~QtRedisCommand() {}

    //!
    //! \brief Конструктор класса
    //! \param command Команда
    //!
    explicit QtRedisCommand(const QByteArray &command)
        : _command(command)
    {}

    //!
    //! \brief Конструктор класса
    //! \param command Команда
    //! \param commandArgv Аргументы команды
    //!
    explicit QtRedisCommand(const QByteArray &command,
                            const QList<QByteArray> &commandArgv)
        : _command(command)
        , _commandArgv(commandArgv)
    {}

    QtRedisCommand(const QtRedisCommand &object)
        : _command(object.command())
        , _commandArgv(object.commandArgv())
    {}

    QtRedisCommand& operator=(const QtRedisCommand &object) {
        if (this == &object)
            return *this;
        _command = object.command();
        _commandArgv = object.commandArgv();
        return *this;
    }

    //!
    //! \brief Команда
    //! \return
    //!
    const QByteArray &command() const {
        return _command;
    }

    //!
    //! \brief Аргументы команды
    //! \return
    //!
    const QList<QByteArray> &commandArgv() const {
        return _commandArgv;
    }

    //!
    //! \brief Является ли объект корректным
    //! \return
    //!
    bool isValid() const {
        return !_command.isEmpty();
    }

    //!
    //! \brief Размер команды (COMMAND + COMMAND_ARGS)
    //! \return
    //!
    int size() const {
        if (!isValid())
            return 0;
        return _commandArgv.size() + 1;
    }

    //!
    //! \brief Создать объект команды из строки QString
    //! \param command Команда в формате строки QString
    //! \return
    //!
    static QtRedisCommand fromString(const QString &command) {
        if (command.trimmed().isEmpty())
            return QtRedisCommand();
        const QStringList cmdLst = command.trimmed().split(" ");
        const QByteArray tmpCmd = cmdLst.constFirst().toUtf8();
        QList<QByteArray> tmpCmdArgv;
        for (int i = 1; i < cmdLst.size(); i++)
            tmpCmdArgv.append(cmdLst.at(i).toUtf8());

        return QtRedisCommand(tmpCmd, tmpCmdArgv);
    }

    //!
    //! \brief Создать объект команды из списка QStringList
    //! \param command Команда в формате списка QStringList
    //! \return
    //!
    static QtRedisCommand fromStringList(const QStringList &commandArgv) {
        if (commandArgv.isEmpty())
            return QtRedisCommand();
        const QByteArray tmpCmd = commandArgv.first().toUtf8();
        QList<QByteArray> tmpCmdArgv;
        for (int i = 1; i < commandArgv.size(); i++)
            tmpCmdArgv.append(commandArgv.at(i).toUtf8());

        return QtRedisCommand(tmpCmd, tmpCmdArgv);
    }

    //!
    //! \brief Создать объект команды из строки QByteArray
    //! \param command Команда в формате строки QByteArray
    //! \return
    //!
    static QtRedisCommand fromByteArray(const QByteArray &command) {
        if (command.trimmed().isEmpty())
            return QtRedisCommand();
        const QList<QByteArray> cmdLst = command.split(' ');
        const QByteArray tmpCmd = cmdLst.constFirst();
        QList<QByteArray> tmpCmdArgv;
        for (int i = 1; i < cmdLst.size(); i++)
            tmpCmdArgv.append(cmdLst.at(i));

        return QtRedisCommand(tmpCmd, tmpCmdArgv);
    }

    //!
    //! \brief Создать объект команды из списка QByteArray
    //! \param command Команда в формате списка QByteArray
    //! \return
    //!
    static QtRedisCommand fromByteArrayList(const QList<QByteArray> &commandArgv) {
        if (commandArgv.isEmpty())
            return QtRedisCommand();
        const QByteArray tmpCmd = commandArgv.first();
        QList<QByteArray> tmpCmdArgv;
        for (int i = 1; i < commandArgv.size(); i++)
            tmpCmdArgv.append(commandArgv.at(i));

        return QtRedisCommand(tmpCmd, tmpCmdArgv);
    }

private:
    QByteArray _command;            //!< команда
    QList<QByteArray> _commandArgv; //!< аргументы команды
};

#endif // QTREDISCOMMAND_H

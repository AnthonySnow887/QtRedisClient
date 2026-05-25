#ifndef QTREDISCOMMAND_H
#define QTREDISCOMMAND_H

#include <QByteArray>
#include <QList>

#include <QString>
#include <QStringList>
#include <QVariantList>

class QtRedisCommand
{
public:
    QtRedisCommand() {}
    ~QtRedisCommand() {}

    explicit QtRedisCommand(const QByteArray &command)
        : _command(command)
    {}

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

    const QByteArray &command() const {
        return _command;
    }

    const QList<QByteArray> &commandArgv() const {
        return _commandArgv;
    }

    bool isValid() const {
        return !_command.isEmpty();
    }

    int size() const {
        if (!isValid())
            return 0;
        return _commandArgv.size() + 1;
    }

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

private:
    QByteArray _command;
    QList<QByteArray> _commandArgv;
};

#endif // QTREDISCOMMAND_H

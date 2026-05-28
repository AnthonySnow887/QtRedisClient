#include "QtRedisPipeline.h"
#include <utility>

//!
//! \brief Конструктор класса
//! \param transporter Класс транспорта для работы с Redis
//!
QtRedisPipeline::QtRedisPipeline(std::shared_ptr<QtRedisTransporter> transporter)
    : QtRedisBase<QtRedisPipeline, bool>()
    , _transporter(transporter)
{
}

//!
//! \brief Деструктор класса
//!
QtRedisPipeline::~QtRedisPipeline()
{
}

//!
//! \brief Отправить все команды одним пакетом
//! \return
//!
QtRedisReply QtRedisPipeline::exec()
{
    QMutexLocker lock(&_mutex);
    if (_commandList.isEmpty()) {
        this->setLastError_safe("Commands list is Empty!");
        return QtRedisReply();
    }
    if (!_transporter) {
        this->setLastError_safe("QtRedisTransporter is NULL!");
        return QtRedisReply();
    }
    if (!_transporter->isConnected()) {
        this->setLastError_safe("Client is not connected!");
        return QtRedisReply();
    }
    this->clearLastError_safe();
    const QtRedisReply reply = _transporter->sendCommands(_commandList);
    if (reply.isError()) {
        this->setLastError_safe(reply.strValue());
    } else if (reply.isArray()) {
        for (const QtRedisReply &replyObj : reply.arrayValue_ref()) {
            if (replyObj.isError())
                this->setLastError_safe(replyObj.strValue());
        }
    }
    _commandList.clear();
    return reply;
}

//!
//! \brief Отправить все команды одним пакетом
//! \return
//!
//! Note: This is a wrapper over function QtRedisPipeline::exec()
//!
bool QtRedisPipeline::execToBool()
{
    this->exec();
    return !this->hasLastError();
}

//!
//! \brief Отменить все внесенные изменения
//!
//! Note: This method clears the entire command queue.
//!
void QtRedisPipeline::discard()
{
    QMutexLocker lock(&_mutex);
    _commandList.clear();
}

//!
//! \brief Выполнить команду
//! \param command Команда
//! \return
//!
bool QtRedisPipeline::processCommand(const QtRedisCommand &command)
{
    _commandList.append(std::move(command));
    return true;
}


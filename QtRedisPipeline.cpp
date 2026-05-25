#include "QtRedisPipeline.h"
#include <QDebug>

//!
//! \brief Деструктор класса
//!
QtRedisPipeline::QtRedisPipeline()
    : QObject()
{
}

QtRedisPipeline::~QtRedisPipeline()
{
    _lastError.clear();
    if (_transporter)
        delete _transporter;
}


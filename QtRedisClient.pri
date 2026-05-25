INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

QT += core network

HEADERS +=  $$PWD/QtRedisClient.h \
            $$PWD/QtRedisBase.h \
            $$PWD/QtRedisClientVersion.h \
            $$PWD/QtRedisCommand.h \
            $$PWD/QtRedisPipeline.h \
            $$PWD/QtRedisReply.h \
            $$PWD/QtRedisClientInfo.h \
            $$PWD/NetworkLayer/QtRedisParser.h \
            $$PWD/NetworkLayer/QtRedisContext.h \
            $$PWD/NetworkLayer/QtRedisContextTcp.h \
            $$PWD/NetworkLayer/QtRedisTransporter.h \
            $$PWD/NetworkLayer/QtRedisContextSsl.h \
            $$PWD/NetworkLayer/QtRedisContextUnix.h


SOURCES +=  $$PWD/QtRedisClient.cpp \
            $$PWD/NetworkLayer/QtRedisParser.cpp \
            $$PWD/NetworkLayer/QtRedisContextTcp.cpp \
            $$PWD/NetworkLayer/QtRedisTransporter.cpp \
            $$PWD/NetworkLayer/QtRedisContextSsl.cpp \
            $$PWD/NetworkLayer/QtRedisContextUnix.cpp \
            $$PWD/QtRedisPipeline.cpp
    

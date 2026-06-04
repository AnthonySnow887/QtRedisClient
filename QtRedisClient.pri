INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

QT += core network

CONFIG += c++14
equals(QMAKE_COMPILER, "gcc"): QMAKE_CXXFLAGS += -std=c++14
equals(QMAKE_COMPILER, "msvc"): QMAKE_CXXFLAGS += /std:c++14

HEADERS +=  $$PWD/QtRedisClient.h \
            $$PWD/QtRedisClientVersion.h \
            $$PWD/Core/QtRedisCommand.h \
            $$PWD/Core/QtRedisReply.h \
            $$PWD/Core/QtRedisClientInfo.h \
            $$PWD/Core/QtRedisBase.h \
            $$PWD/Core/QtRedisPipeline.h \
            $$PWD/Core/QtRedisTransaction.h \
            $$PWD/Core/NetworkLayer/QtRedisParser.h \
            $$PWD/Core/NetworkLayer/QtRedisContext.h \
            $$PWD/Core/NetworkLayer/QtRedisContextTcp.h \
            $$PWD/Core/NetworkLayer/QtRedisTransporter.h \
            $$PWD/Core/NetworkLayer/QtRedisContextSsl.h \
            $$PWD/Core/NetworkLayer/QtRedisContextUnix.h


SOURCES +=  $$PWD/QtRedisClient.cpp \
            $$PWD/Core/QtRedisPipeline.cpp \
            $$PWD/Core/QtRedisTransaction.cpp \
            $$PWD/Core/NetworkLayer/QtRedisParser.cpp \
            $$PWD/Core/NetworkLayer/QtRedisContextTcp.cpp \
            $$PWD/Core/NetworkLayer/QtRedisTransporter.cpp \
            $$PWD/Core/NetworkLayer/QtRedisContextSsl.cpp \
            $$PWD/Core/NetworkLayer/QtRedisContextUnix.cpp
    

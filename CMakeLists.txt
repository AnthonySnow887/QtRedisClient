cmake_minimum_required(VERSION 3.7)

#project(QtRedisClient VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

add_library(QtRedisClient STATIC
    QtRedisClient.h
    QtRedisClientVersion.h
    Core/QtRedisCommand.h
    Core/QtRedisReply.h
    Core/QtRedisClientInfo.h
    Core/QtRedisBase.h
    Core/QtRedisPipeline.h
    Core/QtRedisTransaction.h
    Core/NetworkLayer/QtRedisParser.h
    Core/NetworkLayer/QtRedisContext.h
    Core/NetworkLayer/QtRedisContextTcp.h
    Core/NetworkLayer/QtRedisTransporter.h
    Core/NetworkLayer/QtRedisContextSsl.h
    Core/NetworkLayer/QtRedisContextUnix.h
    QtRedisClient.cpp
    Core/QtRedisPipeline.cpp
    Core/QtRedisTransaction.cpp
    Core/NetworkLayer/QtRedisParser.cpp
    Core/NetworkLayer/QtRedisContextTcp.cpp
    Core/NetworkLayer/QtRedisTransporter.cpp
    Core/NetworkLayer/QtRedisContextSsl.cpp
    Core/NetworkLayer/QtRedisContextUnix.cpp)

target_link_libraries(QtRedisClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network)

target_include_directories(QtRedisClient INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

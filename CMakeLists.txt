cmake_minimum_required(VERSION 3.7)

#project(QtRedisClient VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

add_library(QtRedisClient STATIC
    QtRedisClientVersion.h
    QtRedisReply.h
    QtRedisClientInfo.h
    QtRedisClient.h
    QtRedisClient.cpp
    NetworkLayer/QtRedisParser.h
    NetworkLayer/QtRedisContext.h
    NetworkLayer/QtRedisContextTcp.h
    NetworkLayer/QtRedisTransporter.h
    NetworkLayer/QtRedisContextSsl.h
    NetworkLayer/QtRedisContextUnix.h
    NetworkLayer/QtRedisParser.cpp
    NetworkLayer/QtRedisContextTcp.cpp
    NetworkLayer/QtRedisTransporter.cpp
    NetworkLayer/QtRedisContextSsl.cpp
    NetworkLayer/QtRedisContextUnix.cpp)

target_link_libraries(QtRedisClient PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network)

target_include_directories(QtRedisClient INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

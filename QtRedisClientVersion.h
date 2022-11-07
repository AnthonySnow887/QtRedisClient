#ifndef QTREDISCLIENTVERSION_H
#define QTREDISCLIENTVERSION_H

#define QT_REDIS_CLIENT_VERSION_STR "1.1.1"
/*
   QT_REDIS_CLIENT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define QT_REDIS_CLIENT_VERSION 0x010101
/*
   can be used like #if (QT_REDIS_CLIENT_VERSION >= QT_REDIS_CLIENT_VERSION_CHECK(1, 0, 1))
*/
#define QT_REDIS_CLIENT_VERSION_CHECK(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

#endif // QTREDISCLIENTVERSION_H

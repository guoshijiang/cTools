//
// Created by wanghaiyang on 4/21/17.
//

#ifndef CROSS_PLATFORM_TYPE_H
#define CROSS_PLATFORM_TYPE_H

#include "cross_platform.h"
#include <stdint.h>

#if CROSS_PLATFORM == PLATFORM_WINDOWS
#elif CROSS_PLATFORM == PLATFORM_LINUX
#elif CROSS_PLATFORM == PLATFORM_APPLE
#elif CROSS_PLATFORM == PLATFORM_FREEBSD
#else
#endif


#if CROSS_PLATFORM == PLATFORM_WINDOWS
    #include <intsafe.h>
    #include <WinSock2.h>
    typedef INT32 cp_int;
    typedef INT8 cp_int8;
    typedef INT16 cp_int16;
    typedef INT32 cp_int32;
    typedef INT64 cp_int64;

    typedef UINT32 cp_uint;
    typedef UINT8 cp_uint8;
    typedef UINT16 cp_uint16;
    typedef UINT32 cp_uint32;
    typedef UINT64 cp_uint64;

    typedef UINT64 cp_size_t;

    typedef SOCKET cp_socket_t;
#elif CROSS_PLATFORM == PLATFORM_LINUX
    typedef int32_t cp_int;
    typedef int8_t  cp_int8;
    typedef int16_t cp_int16;
    typedef int32_t cp_int32;
    typedef int64_t cp_int64;

    typedef uint32_t cp_uint;
    typedef uint8_t  cp_uint8;
    typedef uint16_t cp_uint16;
    typedef uint32_t cp_uint32;
    typedef uint64_t cp_uint64;

    typedef uint64_t cp_size_t;

    typedef int32_t cp_socket_t;
#elif CROSS_PLATFORM == PLATFORM_APPLE
#elif CROSS_PLATFORM == PLATFORM_FREEBSD
#else
#endif

#endif //CROSS_PLATFORM_TYPE_H

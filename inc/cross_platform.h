//
// Created by wanghaiyang on 4/21/17.
//

#ifndef CROSS_PLATFORM_H
#define CROSS_PLATFORM_H

#define PLATFORM_LINUX 0X0010
#define PLATFORM_WINDOWS 0X0020
#define PLATFORM_APPLE 0X0030
#define PLATFORM_FREEBSD 0X0040
#define CROSS_PLATFORM PLATFORM_LINUX

#if CROSS_PLATFORM == PLATFORM_WINDOWS
#elif CROSS_PLATFORM == PLATFORM_LINUX
#elif CROSS_PLATFORM == PLATFORM_APPLE
#elif CROSS_PLATFORM == PLATFORM_FREEBSD
#else
#endif

#endif //CROSS_PLATFORM_H
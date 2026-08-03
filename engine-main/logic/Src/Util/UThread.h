#ifndef __UTHREAD_HEADER__
#define __UTHREAD_HEADER__

#include "Core.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include "UThreadWindows.h"
#elif (LORD_PLATFORM == LORD_PLATFORM_LINUX || LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_MAC_OSX)
#include "UThreadLinux.h"
#elif (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
#include "UThreadIOS.h"
#else
#error "UThread is not written for this platform"
#endif

#endif

//
//  UThreadIOS.cpp
//  Logic
//
//  Created by KiBen on 2017/11/23.
//

#include "UThreadIOS.h"
#ifdef __APPLE__
void UThread::setName(const String& name) {
}

int UThread::getNativeId() {
    return -999;
}

void UThread::setThisThreadName(const char* name)
{
}

void UThread::setThisThreadPriority(ThreadPriority priority)
{
}

int UThread::getThisThreadNativeId()
{
    return -999;
}
#endif

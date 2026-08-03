//
//  UThreadIOS.h
//  Logic
//
//  Created by KiBen on 2017/11/23.
//

#ifndef UThreadIOS_h
#define UThreadIOS_h

#include "UThreadBase.h"

#ifdef __APPLE__
class UThread: public UThreadBase {
    using UThreadBase::UThreadBase;
    
public:
    static void setThisThreadName(const char* name);
    static void setThisThreadPriority(ThreadPriority priority);
    static int getThisThreadNativeId();
    
    void setName(const String& name) override;
    void setPriority(ThreadPriority priority) override {
        m_priority = priority;
    }
    int getNativeId() override;
};
#endif
#endif /* UThreadIOS_h */

#include "Core.h"
#if (LORD_PLATFORM == LORD_PLATFORM_LINUX || LORD_PLATFORM == LORD_PLATFORM_ANDROID)

#include "UThreadLinux.h"
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#if(LORD_PLATFORM != LORD_PLATFORM_ANDROID)
	#define gettid() syscall(SYS_gettid)
#endif

int UThread::normalNiceValue = UThread::getNormalNiceValue();

int UThread::getNormalNiceValue()
{
	long tid = gettid();
	return getpriority(PRIO_PROCESS, tid);
}

void UThread::setThreadPriorityImpl(int threadId, ThreadPriority priority)
{
	int niceValue;
	switch (priority)
	{
	case ThreadPriority::LOW:
		niceValue = normalNiceValue + 2;
		break;
	case ThreadPriority::NORMAL:
		niceValue = normalNiceValue;
		break;
	case ThreadPriority::HIGH:
		niceValue = normalNiceValue - 2;
		break;
	default:
		LordLogError("unsupported priority value %d", priority);
		return;
	}
	if (setpriority(PRIO_PROCESS, threadId, niceValue) != 0)
	{
		LordLogError("Failed to set thread nice value: %s", strerror(errno));
	}
}

void UThread::setThisThreadName(const char* name)
{
#if LORD_PLATFORM != LORD_PLATFORM_MAC_IOS
	pthread_setname_np(pthread_self(), name);
#endif
}

void UThread::setThisThreadPriority(ThreadPriority priority)
{
	setThreadPriorityImpl(gettid(), priority);
}

int UThread::getThisThreadNativeId()
{
	return gettid();
}

void UThread::setName(const String& name)
{
	m_name = name;
	if (m_status == Status::RUNNING)
	{
#if LORD_PLATFORM != LORD_PLATFORM_MAC_IOS
        pthread_setname_np(m_thread.native_handle(), name.c_str());
#endif
	}
}

void UThread::execute()
{
	{   
		std::lock_guard<std::mutex> lock(m_mutex);
		m_nativeId = gettid();
		m_nativeIdAssigned.notify_all();
	}
	m_threadFunc();
}

int UThread::getNativeId()
{
	if (m_status != Status::RUNNING)
	{
		return -1;
	}
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_nativeId >= 0)
	{
		return m_nativeId;
	}
	m_nativeIdAssigned.wait(lock, [this] { return m_nativeId >= 0; });
	return m_nativeId;
}

#endif

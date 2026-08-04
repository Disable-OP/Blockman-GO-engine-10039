#include "Core.h"
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS

#include <windows.h>
#include "UThread.h"

const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.  
	LPCSTR szName; // Pointer to name (in user addr space).  
	DWORD dwThreadID; // Thread ID (-1=caller thread).  
	DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  
void UThread::setThreadNameImpl(NativeHandle threadHandle, const char* name)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = GetThreadId(threadHandle);
	info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
#pragma warning(pop)  
}

void UThread::setThreadPriorityImpl(NativeHandle threadHandle, ThreadPriority priority)
{
	int windowsPriority;
	switch (priority)
	{
	case ThreadPriority::LOW:
		windowsPriority = THREAD_PRIORITY_LOWEST;
		break;
	case ThreadPriority::NORMAL:
		windowsPriority = THREAD_PRIORITY_NORMAL;
		break;
	case ThreadPriority::HIGH:
		windowsPriority = THREAD_PRIORITY_HIGHEST;
		break;
	default:
		LordLogError("unsupported priority value %d", priority);
		return;
	}
	SetThreadPriority(threadHandle, windowsPriority);
}

int UThread::getThisThreadNativeId()
{
	return GetThreadId(GetCurrentThread());
}

int UThread::getNativeId()
{
	if (m_status != Status::RUNNING)
	{
		return -1;
	}
	return GetThreadId(m_thread.native_handle());
}

void UThread::setThisThreadName(const char* name)
{
	setThreadNameImpl(GetCurrentThread(), name);
}

void UThread::setThisThreadPriority(ThreadPriority priority)
{
	setThreadPriorityImpl(GetCurrentThread(), priority);
}

#endif
#ifndef __UTHREAD_LINUX_HEADER__
#define __UTHREAD_LINUX_HEADER__

#include <mutex>
#include <condition_variable>
#include "UThreadBase.h"

class UThread : public UThreadBase
{
	using UThreadBase::UThreadBase;

private:
	static int normalNiceValue;
	int m_nativeId = -1;
	std::mutex m_mutex;
	std::condition_variable m_nativeIdAssigned;

	static int getNormalNiceValue();
	static void setThreadPriorityImpl(int threadId, ThreadPriority priority);

public:
	static void setThisThreadName(const char* name);
	static void setThisThreadPriority(ThreadPriority priority);
	static int getThisThreadNativeId();

	void setName(const String& name) override;
	void setPriority(ThreadPriority priority) override
	{
		m_priority = priority;
		if (m_status == Status::RUNNING)
		{
#if LORD_PLATFORM != LORD_PLATFORM_MAC_IOS
			setThreadPriorityImpl(getNativeId(), priority);
#endif
		}
	}
	void execute() override;
	int getNativeId() override;
};

#endif

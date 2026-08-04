#ifndef __UTHREAD_WINDOWS_HEADER__
#define __UTHREAD_WINDOWS_HEADER__

#include "UThreadBase.h"

class UThread : public UThreadBase
{
	using UThreadBase::UThreadBase;

private:
	static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
	static void setThreadNameImpl(NativeHandle threadHandle, const char* name); 
	static void setThreadPriorityImpl(NativeHandle threadHandle, ThreadPriority priority);

public:
	static void setThisThreadName(const char* name);
	static void setThisThreadPriority(ThreadPriority priority);
	static int getThisThreadNativeId();

	void setName(const String& name) override
	{
		m_name = name;
		if (m_status == Status::RUNNING)
		{
			setThreadNameImpl(m_thread.native_handle(), name.c_str());
		}
	}

	void setPriority(ThreadPriority priority) override
	{
		m_priority = priority;
		if (m_status == Status::RUNNING)
		{
			setThreadPriorityImpl(m_thread.native_handle(), priority);
		}
	}

	int getNativeId() override;
};

#endif
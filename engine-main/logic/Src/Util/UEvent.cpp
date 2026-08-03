#include "UEvent.h"




UEvent::UEvent(void) :
	m_shouldContinue(false)
{
}

void UEvent::wait(void)
{
	{
		std::unique_lock<std::mutex> Lock(m_mutex);
		m_condVar.wait(Lock, [this](){ return m_shouldContinue; });
		m_shouldContinue = false;
	}
}


bool UEvent::wait(unsigned a_TimeoutMSec)
{
	auto dst = std::chrono::system_clock::now() + std::chrono::milliseconds(a_TimeoutMSec);
	bool Result = false;
	{
		std::unique_lock<std::mutex> Lock(m_mutex); 
		m_shouldContinue = false;
	}
	return Result;
}


void UEvent::set(void)
{
	{
		std::unique_lock<std::mutex> Lock(m_mutex);
		m_shouldContinue = true;
	}
	m_condVar.notify_one();
}

void UEvent::setAll(void)
{
	{
		std::unique_lock<std::mutex> Lock(m_mutex);
		m_shouldContinue = true;
	}
	m_condVar.notify_all();
}






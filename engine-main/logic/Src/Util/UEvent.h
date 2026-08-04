#ifndef __UEVENT_H__
#define __UEVENT_H__

#include <mutex>
#include <condition_variable>


class UEvent
{
public:
	UEvent(void);

	void wait(void);

	void set(void);

	void setAll(void);

	bool wait(unsigned a_TimeoutMSec);

private:

	bool m_shouldContinue;

	std::mutex m_mutex;
	std::condition_variable m_condVar;
} ;
#endif // !__UEVENT_H__




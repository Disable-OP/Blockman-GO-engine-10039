#include "Threading.h"
#include "TypeDef.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#define INFINITE 0xFFFFFFFF
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#endif

//------------------------------------------------------------------------------
namespace LORD
{

	//------------------------------------------------------------------------------
	WaitEvent::WaitEvent(bool manualReset /*= false*/)
	{
		m_bManualReset = manualReset;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		m_hEvent = CreateEvent(nullptr, manualReset, FALSE, nullptr);
#else
		pthread_cond_init(&m_hCond, 0);
		pthread_mutex_init(&m_hMutex, nullptr);
		m_bManualReset = manualReset;
		m_bSignal = false;
#endif
	}
	//------------------------------------------------------------------------------

	WaitEvent::~WaitEvent()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		CloseHandle(m_hEvent);
		m_hEvent = nullptr;
#else
		pthread_mutex_destroy(&m_hMutex);
		pthread_cond_destroy(&m_hCond);
#endif
	}

	//------------------------------------------------------------------------------
	void WaitEvent::Signal()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		SetEvent(m_hEvent);
#else
		pthread_mutex_lock(&m_hMutex);
		m_bSignal = true;
		if (m_bManualReset)
		{
			pthread_mutex_unlock(&m_hMutex);
			pthread_cond_broadcast(&m_hCond);
		}
		else
		{
			pthread_mutex_unlock(&m_hMutex);
			pthread_cond_signal(&m_hCond);
		}
#endif
	}

	//------------------------------------------------------------------------------
	void WaitEvent::Reset()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		ResetEvent(m_hEvent);
#else
		pthread_mutex_lock(&m_hMutex);
		m_bSignal = false;
		pthread_mutex_unlock(&m_hMutex);
#endif
	}

	//------------------------------------------------------------------------------
	void WaitEvent::Wait()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		WaitForSingleObject(m_hEvent, INFINITE);
#else
		Wait(INFINITE);
#endif
	}

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	int WaitEvent::UnlockedWaitForEvent(unsigned int ms)
	{
		int result = 0;
		if (!m_bSignal)
		{
			if (ms == 0)
			{
				return ETIMEDOUT;
			}

			timespec tm;
			if (INFINITE != ms)
			{
				timeval tv;
				gettimeofday(&tv, nullptr);
				ui64 nanoseconds = ((ui64)tv.tv_sec) * 1000 * 1000 * 1000 + ms * 1000 * 1000 + ((ui64)tv.tv_usec) * 1000;
				tm.tv_sec = nanoseconds / 1000 / 1000 / 1000;
				tm.tv_nsec = (nanoseconds - ((ui64)tm.tv_sec) * 1000 * 1000 * 1000);
			}

			do
			{
				if (ms != INFINITE)
				{
					result = pthread_cond_timedwait(&m_hCond, &m_hMutex, &tm);
				}
				else
				{
					result = pthread_cond_wait(&m_hCond, &m_hMutex);
				}
			} while (result == 0 && !m_bSignal);

			if (result == 0 && (!m_bManualReset))
			{
				m_bSignal = false;
			}
		}
		else if (!m_bManualReset)
		{
			result = 0;
			m_bSignal = false;
		}

		return result;
	}
#endif

	//------------------------------------------------------------------------------
	bool WaitEvent::Wait(unsigned int ms)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		DWORD res = WaitForSingleObject(m_hEvent, ms);
		return (WAIT_TIMEOUT == res) ? false : true;
#else
		int tempResult;
		if (ms == 0)
		{
			tempResult = pthread_mutex_trylock(&m_hMutex);
			if (tempResult == EBUSY)
			{
				return false;
			}
		}
		else
		{
			tempResult = pthread_mutex_lock(&m_hMutex);
		}

		assert(tempResult == 0);

		int result = UnlockedWaitForEvent(ms);

		tempResult = pthread_mutex_unlock(&m_hMutex);
		assert(tempResult == 0);

		return (result == 0) ? true : false;
#endif
	}

	//------------------------------------------------------------------------------
	bool WaitEvent::IsSignalled() const
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		DWORD res = WaitForSingleObject(m_hEvent, 0);
		return (WAIT_TIMEOUT == res) ? false : true;
#else
		return m_bSignal;
#endif
	}

} // Namespace LORD

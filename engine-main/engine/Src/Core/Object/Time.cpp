#include "Core.h"
#include "Time.h"
#include "Util/StringUtil.h"

#if (LORD_PLATFORM != LORD_PLATFORM_WINDOWS)
#include <sys/time.h>
#endif

#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS || LORD_PLATFORM == LORD_PLATFORM_MAC_OSX)
#include <mach/mach_time.h>
#endif

namespace LORD
{
	//---------------------------------------------------------------------
	Time* Time::Instance()
	{
		return ms_pSingleton;
	}

	Time::Time()
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		: mTimerMask( 0 )
#endif
	{
		reset();
	}

	Time::~Time()
	{

	}

	//--------------------------------------------------------------------------------//
	void Time::reset()
	{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		DWORD_PTR procMask;
		DWORD_PTR sysMask;
		GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

		// If procMask is 0, consider there is only one core available
		// (using 0 as procMask will cause an infinite loop below)
		if (procMask == 0)
			procMask = 1;

		// Find the lowest core that this process uses
		if( mTimerMask == 0 )
		{
			mTimerMask = 1;
			while( ( mTimerMask & procMask ) == 0 )
			{
				mTimerMask <<= 1;
			}
		}

		HANDLE thread = GetCurrentThread();

		// Set affinity to the first core
		DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

		// Get the constant frequency
		QueryPerformanceFrequency(&mFrequency);

		// Query the timer
		QueryPerformanceCounter(&mStartTime);
		mStartTick = GetTickCount();

		// Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		mLastTime = 0;
		mZeroClock = clock();

#else
		mZeroClock = clock();
		gettimeofday(&m_startTime, NULL);

#endif
	}

	unsigned long Time::getMilliseconds()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		// Set affinity to the first core
		DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

		// Query the timer
		QueryPerformanceCounter(&curTime);

		// Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

		// scale by 1000 for milliseconds
		unsigned long newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);

		// detect and compensate for performance counter leaps
		// (surprisingly common, see Microsoft KB: Q274323)
		unsigned long check = GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			// We must keep the timer running forward :)
			LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
			mStartTime.QuadPart += adjust;
			newTime -= adjust;

			// Re-calculate milliseconds
			newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);
		}

		// Record last time for adjust
		mLastTime = newTime;

		return newTicks;
#else

		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-m_startTime.tv_sec)*1000+(now.tv_usec-m_startTime.tv_usec)/1000;
#endif
	}

	//--------------------------------------------------------------------------------//
	unsigned long Time::getMicroseconds()
	{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		LARGE_INTEGER curTime;

		HANDLE thread = GetCurrentThread();

		// Set affinity to the first core
		DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

		// Query the timer
		QueryPerformanceCounter(&curTime);

		// Reset affinity
		SetThreadAffinityMask(thread, oldMask);

		LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;

		// get milliseconds to check against GetTickCount
		unsigned long newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);

		// detect and compensate for performance counter leaps
		// (surprisingly common, see Microsoft KB: Q274323)
		unsigned long check = GetTickCount() - mStartTick;
		signed long msecOff = (signed long)(newTicks - check);
		if (msecOff < -100 || msecOff > 100)
		{
			// We must keep the timer running forward :)
			LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
			mStartTime.QuadPart += adjust;
			newTime -= adjust;
		}

		// Record last time for adjust
		mLastTime = newTime;

		// scale by 1000000 for microseconds
		unsigned long newMicro = (unsigned long) (1000000 * newTime / mFrequency.QuadPart);

		return newMicro;
#else

		struct timeval now;
		gettimeofday(&now, NULL);
		return (now.tv_sec-m_startTime.tv_sec)*1000000+(now.tv_usec-m_startTime.tv_usec);
#endif
	}

	//-- Common Across All Timers ----------------------------------------------------//
	unsigned long Time::getMillisecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock-mZeroClock) / ((float)CLOCKS_PER_SEC/1000.0)) ;
	}

	//-- Common Across All Timers ----------------------------------------------------//
	unsigned long Time::getMicrosecondsCPU()
	{
		clock_t newClock = clock();
		return (unsigned long)((float)(newClock-mZeroClock) / ((float)CLOCKS_PER_SEC/1000000.0)) ;
	}
}

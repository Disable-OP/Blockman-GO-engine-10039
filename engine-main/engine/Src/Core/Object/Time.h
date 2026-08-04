/********************************************************************
filename: 	Time.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_TIME_H__
#define __LORD_TIME_H__

#include "Memory/LordMemory.h"
#include "Singleton.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace LORD
{
	class LORD_CORE_API Time: public Singleton<Time>, public ObjectAlloc
	{	
	public:
		static Time* Instance();

	public:
		Time();
		~Time();

		void reset();
		/** Returns milliseconds since initialisation or last reset */
		unsigned long getMilliseconds();

		/** Returns microseconds since initialisation or last reset */
		unsigned long getMicroseconds();

		/** Returns milliseconds since initialisation or last reset, only CPU time measured */	
		unsigned long getMillisecondsCPU();

		/** Returns microseconds since initialisation or last reset, only CPU time measured */	
		unsigned long getMicrosecondsCPU();

	protected:
		clock_t mZeroClock;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		DWORD mStartTick;
		LONGLONG mLastTime;
		LARGE_INTEGER mStartTime;
		LARGE_INTEGER mFrequency;

		DWORD_PTR mTimerMask;
#else 
		struct timeval	m_startTime;
#endif
	};
}

#endif
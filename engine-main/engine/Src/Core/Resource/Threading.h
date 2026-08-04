/********************************************************************
filename: 	Threading.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_THREADING_H__
#define __LORD_THREADING_H__

#include "CoreDef.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif 


namespace LORD
{

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	//////////////////////////////////////////////////////////////////////////
	class Mutex
	{
	public:
		Mutex()			{ InitializeCriticalSection(&mCriticalSection);	}
		~Mutex()		{ DeleteCriticalSection(&mCriticalSection);		}
		void Lock()		{ EnterCriticalSection(&mCriticalSection);		}
		void Unlock()	{ LeaveCriticalSection(&mCriticalSection);		}

	private:
		CRITICAL_SECTION mCriticalSection;
	};
#else
	//////////////////////////////////////////////////////////////////////////
	class Mutex
	{
	public:
		Mutex()
		{
			pthread_mutexattr_t MutexAttributes;
			pthread_mutexattr_init(&MutexAttributes);
			pthread_mutexattr_settype(&MutexAttributes, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mutex, &MutexAttributes);
		}
		~Mutex(void) { pthread_mutex_destroy(&mutex);}
		void Lock() { pthread_mutex_lock(&mutex);}
		void Unlock() { pthread_mutex_unlock(&mutex);}
	private:
		pthread_mutex_t mutex;
	};

#endif

	class MutexLock
	{
	public:
		MutexLock(Mutex& mutex) : mMutex(mutex) { mMutex.Lock();	}
		~MutexLock()							{ mMutex.Unlock();	}

	private:
		Mutex& mMutex;
	};

	class WaitEvent
	{
	public:
		WaitEvent(bool manualReset = false);
		~WaitEvent();
		void Signal();
		void Reset();
		void Wait();
		bool Wait(unsigned int ms);
		bool IsSignalled() const;

	private:
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		void* m_hEvent;
#else
		int UnlockedWaitForEvent(unsigned int ms);
		pthread_mutex_t m_hMutex;
		pthread_cond_t m_hCond;
		bool  m_bSignal;
#endif
		bool  m_bManualReset;
	};


#define AUTO_MUTEX_NAME auto_mutex

#define EE_AUTO_MUTEX mutable Mutex AUTO_MUTEX_NAME;
#define EE_LOCK_AUTO_MUTEX MutexLock NameLock(AUTO_MUTEX_NAME);
#define EE_LOCK_MUTEX_NAMED(mutexName, lockName) MutexLock lockName(mutexName);

#define EE_MUTEX(name)	mutable Mutex name;
#define EE_STATIC_MUTEX(name)	static Mutex name;
#define EE_LOCK_MUTEX(mutexName) MutexLock _lockName(mutexName);

}

#endif

/********************************************************************
filename: 	LordThread.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_THREAD_H__
#define __LORD_THREAD_H__

#include "Core.h"
#include "Threading.h"

namespace LORD
{
	//////////////////////////////////////////////////////////////////////////
	enum EVENT_SCHEDULER
	{
		ES_LOW		= 3,
		ES_NORMAL	= 6,
		ES_HIGHT	= 0x0fffffff
	};
	enum EVENT_LEVEL
	{
		E_NORMAL	= 0,
		E_HIGHT		= 1,

		E_MAX_COUNT	= 2
	};

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	class Event
	{
		HANDLE mEvent;

	public:
		Event(void);
		~Event(void);
		void Create();
		void Destroy();
		void Trigger(void);
		void Wait();
	};
#else
	class Event
	{
		inline void LockEventMutex();
		inline void UnlockEventMutex();
		bool bInitialized;
		pthread_mutex_t Mutex;
		pthread_cond_t Condition;

	public:
		Event(void);
		~Event(void);
		void Create();
		void Destroy();
		void Trigger(void);
#ifdef _DEBUG
		bool Wait();
#else 
		void Wait();
#endif
	private:
		unsigned short state;
	};
#endif

	//////////////////////////////////////////////////////////////////////////
	class StreamThread;
	class ThreadBase;

	class ThreadFunction
	{
	public:
		ThreadFunction();
		virtual ~ThreadFunction();

		void operator()();

		void Run();

	public:
		void SetThreadManager(ThreadBase* pThreadManager)	{ m_pThreadManager = pThreadManager;	}
		ThreadBase* GetThreadManager()					{ return m_pThreadManager;				}

	protected:
		ThreadBase* m_pThreadManager;
	};

	//////////////////////////////////////////////////////////////////////////
	// EEThread
	class Thread : public ObjectAlloc
	{
	public:
		Thread();
		virtual ~Thread();

		int Init(ThreadFunction* pThreadFunction);

		void Uninit();

	protected:
		void*				m_hThread;
		ThreadFunction*		m_pThreadFunction;
	};

	//////////////////////////////////////////////////////////////////////////
	class Request;
	class ThreadEvent : public ObjectAlloc
	{
	public:
		ThreadEvent() : m_pRequest(NULL)
		{}
		virtual ~ThreadEvent(){ m_pRequest = NULL; }

		virtual bool GetRepeatRespondIfFail()	{ return false; }

		virtual bool IsAutoDestroy()			{ return false; }

		//virtual bool IsEqual(ThreadEvent* pThreadEvent) { return false; }

		// 返回该事件的优先级别
		virtual EVENT_LEVEL GetLevel()			{ return E_NORMAL;}
	public:
		virtual bool ProcessEvent() = 0;

		virtual bool RespondEvent() = 0;

		Request* m_pRequest;
	};

	class Request : public ObjectAlloc
	{
	public:
		Request(unsigned int requestID);
		virtual ~Request();

		virtual void SetThreadEvent(ThreadEvent* pThreadEvent);

		virtual ThreadEvent* GetThreadEvent();

		virtual void Abort();

		virtual bool NeedAborted();

		virtual unsigned int GetID();

	protected:
		bool				mAborted;
		unsigned int		mRequestID;
		ThreadEvent*		mThreadEvent;
	};

	//////////////////////////////////////////////////////////////////////////
	class LORD_CORE_API ThreadBase : public ObjectAlloc
	{
	public:
		ThreadBase();
		virtual ~ThreadBase();
		virtual void ProcThread();
	
	public:
		virtual void Startup();

		virtual void Shutdown();

		virtual bool IsRunning();

		virtual unsigned int AddRequest(ThreadEvent* pThreadEvent);

		virtual void AbortAllRequests();

		virtual void RespondEvents(); 

		virtual void SetRespondingEventTimeLimit(unsigned int uTimeLimit);

		virtual unsigned int GetRespondingEventTimeLimit();

		virtual int GetRequestCount();

		//virtual int GetProcessCount();

		virtual int GetRespondCount();

		virtual void EnableSingleThread(bool bEnable);
		bool isSelf();
		bool HasTask();
	protected:
		void WaitForNextRequest();

		virtual void ProcessNextRequest();
	protected:
		//typedef list<Thread*>::type				ThreadList;
		typedef deque<Request*>::type			RequestQueue;

		ThreadFunction		mThreadFunc;
		int					mThreadID;
		bool				mIsRunning;
		bool				mShuttingDown;
		bool				mIsSingleThread;
		unsigned int		mRequestCount;
		unsigned int		mTimeLimitMS;

		//bool				mHasTask;
		/*int					mInitSchedulingCount;
		int					mRequestSchedulingCount;
		int					mRespondSchedulingCount;*/

		Thread*				m_pThread;
		RequestQueue		mRequestQueue;
		//RequestQueue		mProcessQueue;
		RequestQueue		mResponseQueue[E_MAX_COUNT];

		Event m_MutexAddRequest;
		Event m_MutexThreadEnd;

		EE_MUTEX(mRequestMutex);
		//EE_MUTEX(mProcessMutex);
		EE_MUTEX(mResponseMutex[E_MAX_COUNT]);
	};

	class LORD_CORE_API StreamThread : public ThreadBase, public Singleton<StreamThread>
	{
	public:
		static StreamThread* Instance();

	public:
		StreamThread();
		virtual ~StreamThread();
	};

	class LORD_CORE_API CalcThread : public ThreadBase
	{
	public:
		CalcThread(Event* pMainThreadEvent);
		virtual ~CalcThread();

		virtual void ProcThread();
	public:
		virtual void Startup();

		virtual void Shutdown();

		virtual bool IsRunning();

		virtual unsigned int AddRequest(ThreadEvent* pThreadEvent);

		virtual void RespondEvents(); 
		
		virtual int GetEventCount();
		
		virtual Event* GetThreadEvent();

	protected:
		virtual void ProcessNextRequest() {};
	protected:
		typedef set<ThreadEvent*>::type			ThreadEventSet;

		ThreadFunction		mThreadFunc;
		bool				mIsRunning;
		bool				mShuttingDown;
		unsigned int		mEventCount;

		Thread*				m_pThread;
		ThreadEventSet		mEventSet;
		Event*				m_MainThreadEvent;
		Event				m_ProcessEvent;
		Event				m_CloseEvent;
	};
}

#endif

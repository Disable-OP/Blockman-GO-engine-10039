#include "Core.h"
#include "LordThread.h"
#include "Object/Exception.h"
#include "Audio/SoundSystem.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Util/AssertX.h"

#if (LORD_PLATFORM != LORD_PLATFORM_WINDOWS)
#   include <unistd.h>
#   include <sys/syscall.h>
#   include <unistd.h>
#   if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS || LORD_PLATFORM == LORD_PLATFORM_MAC_OSX)
#       define gettid() syscall(SYS_gettid)
#   else
#       define gettid() syscall(__NR_gettid)
#   endif
#endif

namespace LORD
{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
	static unsigned __stdcall ThreadCallBackProc(void *pvArg)
	{
		(static_cast<ThreadFunction *>(pvArg))->Run();
		return 0;
	}
	/////////////////////////////////////////////////////////////////////////
	//Wiindows Event
	Event::Event(void)
	{
		mEvent = NULL;
	}
	Event::~Event(void)
	{
		Destroy();
	}
	void Event::Create()
	{
		mEvent = CreateEvent(NULL, true, 0, NULL);
	}
	void Event::Destroy()
	{
		if (mEvent != NULL)
		{
			CloseHandle(mEvent);
			mEvent = NULL;	
		}
	}
	void Event::Trigger(void)
	{
		SetEvent(mEvent);
	}
	void Event::Wait()
	{
		WaitForSingleObject(mEvent, INFINITE);
		ResetEvent(mEvent);
	}
#else
	static void* ThreadCallBackProc(void *pvArg)
	{
		(static_cast<ThreadFunction *>(pvArg))->Run();
		return NULL;
	}
	/////////////////////////////////////////////////////////////////////////
	//Iphone Event
	Event::Event(void)
	{
		state = 0;
		bInitialized = false;
	}
	Event::~Event(void)
	{
		Destroy();
	}
	void Event::LockEventMutex()
	{
		pthread_mutex_lock(&Mutex);
	}
	void Event::UnlockEventMutex()
	{
		pthread_mutex_unlock(&Mutex);
	}
	void Event::Create()
	{
		LordAssert(!bInitialized);

		if (pthread_mutex_init(&Mutex, NULL) == 0)
		{
			if (pthread_cond_init(&Condition, NULL) == 0)
			{
				bInitialized = true;
			}
			else
			{
				pthread_mutex_destroy(&Mutex);
				LordException("Error: error in Event::Create");
			}
		}
	}
	void Event::Destroy()
	{
		if (bInitialized)
		{
			LockEventMutex();
			pthread_cond_destroy(&Condition);
			UnlockEventMutex();
			pthread_mutex_destroy(&Mutex);
			bInitialized = false;
		}
	}
	void Event::Trigger(void)
	{
		LockEventMutex();
		pthread_cond_signal(&Condition);
		state = 1;
		UnlockEventMutex();
	}

#ifdef _DEBUG
	bool Event::Wait()
	{
		bool ret = false;
		LockEventMutex();
		if(state == 0)
		{
			pthread_cond_wait(&Condition, &Mutex);
			ret = true;
		}
		state = 0;
		UnlockEventMutex();
		return ret;
	}
#else
	void Event::Wait()
	{
		LockEventMutex();
		if(state == 0)
		{
			pthread_cond_wait(&Condition, &Mutex);
		}
		state = 0;
		UnlockEventMutex();
	}
#endif

#endif
	//////////////////////////////////////////////////////////////////////////
	ThreadFunction::ThreadFunction()
		: m_pThreadManager(NULL)
	{

	}
	//-----------------------------------------------------------------------
	ThreadFunction::~ThreadFunction()
	{

	}
	//-----------------------------------------------------------------------
	void ThreadFunction::operator()()
	{
		LordAssert(m_pThreadManager);
		m_pThreadManager->ProcThread();
	}
	//-----------------------------------------------------------------------
	void ThreadFunction::Run()
	{
		LordAssert(m_pThreadManager);
		m_pThreadManager->ProcThread();
	}
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// EEThread
	Thread::Thread()
		: m_hThread(NULL)
		, m_pThreadFunction(NULL)
	{
	}
	//-----------------------------------------------------------------------
	Thread::~Thread()
	{
		Uninit();
	}
	//-----------------------------------------------------------------------
	int Thread::Init(ThreadFunction* pThreadFunction)
	{
		if (!pThreadFunction)
			return false;

		if (m_hThread)
			return false;

		m_pThreadFunction = pThreadFunction;

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		m_hThread = (HANDLE)_beginthreadex(
			NULL,			        	// SD
			0,				        	// initial stack size
			ThreadCallBackProc,			// thread function
			(void*)m_pThreadFunction, 	// thread argument
			0,				        	// creation option
			NULL						// thread identifier
			);	
		if (!m_hThread)
		{
			LordException("Error: _beginthreadex fail! Thread::Init");
			return false;
		}
#else
		int error;
		error = pthread_create((pthread_t*)&m_hThread, NULL, ThreadCallBackProc, (void*)m_pThreadFunction);
		if(error != 0)
		{
			LordException("Error: _beginthreadex fail! Thread::Init");
			return false;
		}
#endif
		return true;
	}
	//-----------------------------------------------------------------------
	void Thread::Uninit()
	{
		if (m_hThread)
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
#endif
			m_hThread = NULL;
			m_pThreadFunction = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Request
	Request::Request(unsigned int requestID)
		: mAborted(false)
		, mRequestID(requestID)
		, mThreadEvent(NULL)
	{

	}
	//-----------------------------------------------------------------------
	Request::~Request()	
	{
		if (mThreadEvent && mThreadEvent->IsAutoDestroy())
		{
			LordDelete mThreadEvent;
			mThreadEvent = NULL;
		}
	}
	//-----------------------------------------------------------------------
	void Request::SetThreadEvent(ThreadEvent* pThreadEvent)	
	{ 
		mThreadEvent = pThreadEvent; 
		mThreadEvent->m_pRequest = this;
	}
	//-----------------------------------------------------------------------
	ThreadEvent* Request::GetThreadEvent()	
	{ 
		return mThreadEvent;
	}
	//-----------------------------------------------------------------------
	void Request::Abort()		
	{ 
		mAborted = true;	
	}
	//-----------------------------------------------------------------------
	bool Request::NeedAborted()	
	{ 
		return mAborted;	
	}
	//-----------------------------------------------------------------------
	unsigned int Request::GetID()		
	{ 
		return mRequestID;	
	}
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	ThreadBase::ThreadBase()
		: mIsRunning(false)
		, mShuttingDown(false)
		, mIsSingleThread(false)
		, mRequestCount(0)
		, mTimeLimitMS(0)
		, m_pThread(NULL)
		, mThreadID(0)
	{

	}

	ThreadBase::~ThreadBase()
	{

	}

	void ThreadBase::Startup()
	{	
		if (mIsRunning)
			return;

		mShuttingDown = false;

		mThreadFunc.SetThreadManager(this);

		m_MutexAddRequest.Create();
		m_MutexThreadEnd.Create();

		m_pThread = LordNew Thread;
		LordAssert(m_pThread);

		if (!m_pThread->Init(&mThreadFunc))
		{
			LordException("Error: Init thread fail! EEThreadManager::Startup");
			return ;
		}

		mIsRunning = true;

		LordLogInfo("Thread Pool Startuped");
	}
	//-----------------------------------------------------------------------
	void ThreadBase::Shutdown()
	{
		if (!mIsRunning)
			return;

		AbortAllRequests();

		Request* pRequest = NULL;

		{
			EE_LOCK_MUTEX(mRequestMutex);
			while (!mRequestQueue.empty())
			{
				pRequest = mRequestQueue.front();
				mRequestQueue.pop_front();	

				if (pRequest)
				{
					LordDelete pRequest;
				}
			}
		}

		mShuttingDown = true;
		m_MutexAddRequest.Trigger();
		m_MutexThreadEnd.Wait();

		m_pThread->Uninit();
		LordDelete m_pThread;
		m_pThread = NULL;

		m_MutexAddRequest.Destroy();
		m_MutexThreadEnd.Destroy();

		for (int i = E_MAX_COUNT-1; i >= 0; --i)
		{
			EE_LOCK_MUTEX(mResponseMutex[i]);

			// deal with all the responses.
			while (!mResponseQueue[i].empty())
			{
				pRequest = mResponseQueue[i].front();
				mResponseQueue[i].pop_front();

				if (pRequest)
				{
					ThreadEvent* pThreadEvent = pRequest->GetThreadEvent();
					LordAssert(pThreadEvent);
					if (!pThreadEvent->RespondEvent())
					{
						EE_LOCK_MUTEX(mResponseMutex[pThreadEvent->GetLevel()]);
						mResponseQueue[pThreadEvent->GetLevel()].push_front(pRequest);
					}
					else
					{
						LordDelete pRequest;
					}
				}
			}
		}


		mIsRunning = false;
		LordLogInfo("Stream thread Ended.");
	}
	//-----------------------------------------------------------------------
	bool ThreadBase::IsRunning()
	{
		return mIsRunning && !mShuttingDown;
	}
	//-----------------------------------------------------------------------
	unsigned int ThreadBase::AddRequest(ThreadEvent* pThreadEvent)
	{
		LordAssert(pThreadEvent);

		if (mShuttingDown)
			return 0;

		if (mIsSingleThread)
		{
			pThreadEvent->ProcessEvent();
			pThreadEvent->RespondEvent();

			if (pThreadEvent->IsAutoDestroy())
			{
				LordDelete pThreadEvent;
				pThreadEvent = NULL;
			}

			return 0;
		}

		mRequestCount++;

		Request* pRequest = LordNew Request(mRequestCount);
		if ( pRequest == NULL )
		{
			return 0;
		}
		LordAssert(pRequest);

		pRequest->SetThreadEvent(pThreadEvent);

		{
			EE_LOCK_MUTEX(mRequestMutex);
			mRequestQueue.push_back(pRequest);
		}

		m_MutexAddRequest.Trigger();

		return mRequestCount;
	}
	//-----------------------------------------------------------------------
	void ThreadBase::AbortAllRequests()
	{
		{
			EE_LOCK_MUTEX(mRequestMutex);

			for (RequestQueue::iterator iter = mRequestQueue.begin(); iter != mRequestQueue.end(); ++iter)
			{
				(*iter)->Abort();
			}
		}

		for (int i = E_MAX_COUNT-1; i >= 0; --i)
		{
			EE_LOCK_MUTEX(mResponseMutex[i]);

			for (RequestQueue::iterator iter = mResponseQueue[i].begin(); iter != mResponseQueue[i].end(); ++iter)
			{
				(*iter)->Abort();
			}
		}
	}
	//-----------------------------------------------------------------------
	void ThreadBase::RespondEvents()
	{
		if ( mResponseQueue[E_NORMAL].size() ||
			mResponseQueue[E_HIGHT].size() )
		{
			Request* pRequest	= NULL;
			ThreadEvent* pThreadEvent = NULL;

			for (int i=E_MAX_COUNT-1; i>=0; --i)
			{
				ui8 t = 0;
				while (t++ < 2)
				{
					EE_LOCK_MUTEX(mResponseMutex[i]);
					if (mResponseQueue[i].empty())
						break;

					pRequest = mResponseQueue[i].front();
					mResponseQueue[i].pop_front();

					if (pRequest)
					{
						if (!pRequest->NeedAborted())
						{
							pThreadEvent = pRequest->GetThreadEvent();
							LordAssert(pThreadEvent);

							if (pThreadEvent->RespondEvent())
							{
								LordDelete pRequest;
							}
							else
							{
								if (pThreadEvent->GetRepeatRespondIfFail())
								{
									LordDelete pRequest;
								}
								else
								{
									mResponseQueue[i].push_front(pRequest);
								}
							}
						}
						else
						{
							LordDelete pRequest;
						}
					}
				}//while
			}//for
		}//if size
	}
	//-----------------------------------------------------------------------
	void ThreadBase::SetRespondingEventTimeLimit(unsigned int uTimeLimit)
	{
		mTimeLimitMS = uTimeLimit;
	}
	//-----------------------------------------------------------------------
	unsigned int ThreadBase::GetRespondingEventTimeLimit()
	{
		return mTimeLimitMS;
	}
	//-----------------------------------------------------------------------
	int ThreadBase::GetRequestCount()
	{
		EE_LOCK_MUTEX(mRequestMutex);
		return (int)mRequestQueue.size();
	}
	//-----------------------------------------------------------------------
	/*int ThreadBase::GetProcessCount()
	{
		return (int)mProcessQueue.size();
	}*/
	//-----------------------------------------------------------------------
	int ThreadBase::GetRespondCount()
	{
		int nSum = 0;
		for (int i = E_MAX_COUNT - 1; i >= 0; --i)
		{
			EE_LOCK_MUTEX(mResponseMutex[i]);
			nSum += (int)(mResponseQueue[i].size());
		}
		return nSum;
	}
	////-----------------------------------------------------------------------
	//void EEThreadManager::SetHasTask(bool bHasTask)
	//{
	//	mHasTask = bHasTask;
	//}
	////-----------------------------------------------------------------------
	bool ThreadBase::isSelf()
	{
		int threadID = 0;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		threadID = ::GetCurrentThreadId();
#else 
		threadID = gettid();
#endif 
		return threadID == mThreadID;
	}
	bool ThreadBase::HasTask()
	{
		return GetRespondCount() || GetRequestCount();
	}
	//-----------------------------------------------------------------------
	void ThreadBase::WaitForNextRequest()
	{
		mRequestMutex.Lock();

		if (!mRequestQueue.empty())
		{
			mRequestMutex.Unlock();
			return ;
		}

		mRequestMutex.Unlock();

		m_MutexAddRequest.Wait();
	}
	//-----------------------------------------------------------------------
	void ThreadBase::ProcessNextRequest()
	{
		Request* pRequest				= NULL;
		ThreadEvent* pThreadEvent		= NULL;

		// :::::::::::::::::处理请求队列:::::::::::::::::::
		do
		{
			EE_LOCK_MUTEX(mRequestMutex);

			if (mRequestQueue.empty())
				continue;

			pRequest = mRequestQueue.front();
			mRequestQueue.pop_front();

			if (pRequest->NeedAborted())
			{
				LordDelete pRequest;
				pRequest = NULL;
			}
			else
			{
				break;
			}		
		} while(0);

		if (!pRequest)
			return;

		pThreadEvent = pRequest->GetThreadEvent();
		LordAssert(pThreadEvent);

		pThreadEvent->ProcessEvent();

		{
			EE_LOCK_MUTEX(mResponseMutex[pThreadEvent->GetLevel()]);
			if (!mShuttingDown)
				mResponseQueue[pThreadEvent->GetLevel()].push_back(pRequest);
		}

	}
	//-----------------------------------------------------------------------
	void ThreadBase::ProcThread()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		mThreadID = ::GetCurrentThreadId();
#else
		mThreadID = gettid();
#endif

		while (!mShuttingDown)
		{
			WaitForNextRequest();
			if(mShuttingDown)
				break;
			ProcessNextRequest();
		}
		m_MutexThreadEnd.Trigger();
	}

	void ThreadBase::EnableSingleThread( bool bEnable )
	{
		mIsSingleThread = bEnable;
	}

	// StreamThread
	//---------------------------------------------------------------------
	StreamThread* StreamThread::Instance()
	{
		return ms_pSingleton;
	}

	StreamThread::StreamThread()
	{
	}
	//-----------------------------------------------------------------------
	StreamThread::~StreamThread()
	{
	}
	//-----------------------------------------------------------------------

	// CalcThread
	//---------------------------------------------------------------------
	CalcThread::CalcThread(Event* pMainThreadEvent)
		: mIsRunning(false)
		, mShuttingDown(false)
		, mEventCount(0) 
		, m_MainThreadEvent(pMainThreadEvent)
		, m_pThread(NULL)
	{

	}

	CalcThread::~CalcThread()
	{
		Shutdown();
	}

	void CalcThread::ProcThread()
	{
		m_MainThreadEvent->Trigger();
		m_ProcessEvent.Wait();
		while(!mShuttingDown)
		{
			for (ThreadEventSet::iterator it=mEventSet.begin(); it!=mEventSet.end(); )
			{
				ThreadEvent* pEvent = *it;
				if (!pEvent->ProcessEvent())
				{
					ThreadEventSet::iterator tempIt = it; ++it;
					mEventSet.erase(tempIt);
					--mEventCount;
					LordSafeDelete(pEvent);
				}
				else
				{
					++it;
				}
			}
			m_MainThreadEvent->Trigger();
			m_ProcessEvent.Wait();
		}
		m_CloseEvent.Trigger();
	}

	void CalcThread::RespondEvents()
	{
		for (ThreadEventSet::iterator it=mEventSet.begin(); it!=mEventSet.end(); ++it)
		{
			(*it)->RespondEvent();
		}
	}
	
	void CalcThread::Startup()
	{
		if (mIsRunning)
			return;

		mShuttingDown = false;

		mThreadFunc.SetThreadManager(this);

		m_ProcessEvent.Create();
		m_CloseEvent.Create();

		m_pThread = LordNew Thread;
		LordAssert(m_pThread);

		if (!m_pThread->Init(&mThreadFunc))
		{
			LordException("Error: Init thread fail! CalcThread::Startup");
			return ;
		}

		mIsRunning = true;

		LordLogInfo("CalcThread Started");
	}

	void CalcThread::Shutdown()
	{
		if (!mIsRunning)
			return;
		
		mShuttingDown = true;

		m_ProcessEvent.Trigger();
		m_CloseEvent.Wait();

		m_pThread->Uninit();
		LordDelete m_pThread;
		m_pThread = NULL;

		m_ProcessEvent.Destroy();
		m_CloseEvent.Destroy();

		for (ThreadEventSet::iterator it=mEventSet.begin(); it!=mEventSet.end(); ++it)
		{
			LordDelete *it;
		}
		mEventSet.clear();

		mIsRunning = false;
		LordLogInfo("CalcThread thread Ended.");
	}

	bool CalcThread::IsRunning()
	{
		return mIsRunning;
	}

	unsigned int CalcThread::AddRequest( ThreadEvent* pThreadEvent )
	{
		LordAssert(mEventSet.find(pThreadEvent) == mEventSet.end());
		mEventSet.insert(pThreadEvent);
		
		return ++mEventCount;
	}

	int CalcThread::GetEventCount()
	{
		return (int)mEventSet.size();
	}

	Event* CalcThread::GetThreadEvent()
	{
		return &m_ProcessEvent;
	}

}

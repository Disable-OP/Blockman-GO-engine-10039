#include "VisualTessThread.h"
#include "RenderGlobal.h"
#include "cChunk/ChunkClient.h"
#include "Chunk/ChunkService.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#include <string.h>
#include <unistd.h>
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include <sys/prctl.h>
#endif
#endif

namespace BLOCKMAN
{
	//------------------------------------------------------------------------------
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
DWORD THREAD_CALLBACK TessThread::ThreadProc(void* t)
{
	TessThread* pThread = reinterpret_cast<TessThread*>(t);
	SetThreadName(pThread->GetThreadName());
	pThread->Run();
	return 0;
}
ui32 g_MainThreadId = ::GetCurrentThreadId();
#else
void* TessThread::ThreadProc(void* pParam)
{
	TessThread* pThread = reinterpret_cast<TessThread*>(pParam);
	pThread->Run();
	return 0;
}
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
ui32 g_MainThreadId = ui32(pthread_self());
#else
ui32 g_MainThreadId = pthread_mach_thread_np(pthread_self());
#endif
#endif
//------------------------------------------------------------------------------
ui32 TessThread::GetMainThreadId()
{
	return g_MainThreadId;
}

//------------------------------------------------------------------------------
ui32 TessThread::GetCurrentThreadId()
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	return ::GetCurrentThreadId();
#elif LORD_PLATFORM == LORD_PLATFORM_ANDROID
	return ui32(pthread_self());
#else
	return pthread_mach_thread_np(pthread_self());
#endif
}

//------------------------------------------------------------------------------
TessThread::TessThread()
	: m_priority(Normal)
	, m_stackSize(0)
	, m_stopEvent(true)
{
	memset(&m_hThread, 0, sizeof(m_hThread));
}

//------------------------------------------------------------------------------
TessThread::~TessThread()
{
	if (this->IsRunning())
	{
		// force to exit
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		TerminateThread(m_hThread, 0);
#else
		pthread_kill(m_hThread, 9);
#endif
	}
}

//------------------------------------------------------------------------------
void
TessThread::SetPriority(Priority p)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	int nPriority = THREAD_PRIORITY_NORMAL;

	if (p == Low)
		nPriority = THREAD_PRIORITY_BELOW_NORMAL;
	else if (p == Normal)
		nPriority = THREAD_PRIORITY_NORMAL;
	else if (p == High)
		nPriority = THREAD_PRIORITY_ABOVE_NORMAL;

	::SetThreadPriority(m_hThread, nPriority);
#endif
	m_priority = p;
}

//------------------------------------------------------------------------------
void TessThread::Start()
{
	assert(!this->IsRunning());
	m_stopEvent.Reset();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	m_hThread = ::CreateThread(0, m_stackSize, ThreadProc, this, CREATE_SUSPENDED, nullptr);
	assert(m_hThread);
	// apply thread priority
	SetPriority(m_priority);
	ResumeThread((HANDLE)m_hThread);
#else
	if (m_hThread != 0) return;

	pthread_attr_t attr;
	sched_param param;
	pthread_attr_init(&attr);

	switch (m_priority)
	{
	case Low:
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int min_priority = sched_get_priority_min(SCHED_RR);
		param.sched_priority = min_priority;
		pthread_attr_setschedparam(&attr, &param);
	}
	break;
	case High:
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		int max_priority = sched_get_priority_max(SCHED_RR);
		param.sched_priority = max_priority;
		pthread_attr_setschedparam(&attr, &param);
	}
	break;
	case Normal:
		break;
	}
	pthread_t nVal;
	pthread_create(&nVal, &attr, ThreadProc, (void *)this);
	pthread_attr_destroy(&attr);
	if (nVal == 0)
	{
		return;
	}
	m_hThread = nVal;
#endif
}

//------------------------------------------------------------------------------
bool
TessThread::IsRunning() const
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	if (nullptr != m_hThread)
	{
		DWORD exitCode = 0;
		if (GetExitCodeThread(m_hThread, &exitCode))
		{
			if (STILL_ACTIVE == exitCode)
			{
				return true;
			}
		}
	}
#else
	if (0 != m_hThread)
	{
		return !m_stopEvent.IsSignalled();
	}
#endif
	return false;
}

//------------------------------------------------------------------------------
void
TessThread::Stop()
{
	//  assert(this->IsRunning());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	assert(m_hThread != nullptr);
#else
	assert(m_hThread != 0);
#endif
	m_stopEvent.Signal();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// wait for the thread to terminate
	WaitForSingleObject((HANDLE)m_hThread, INFINITE);
	CloseHandle((HANDLE)m_hThread);
	m_hThread = nullptr;
#else
	if (m_hThread != 0)
	{
		bool ret = pthread_join(m_hThread, nullptr);
		m_hThread = 0;
		if (0 == ret)
		{
			return;
		}
		else
		{
			pthread_kill(m_hThread, 9);
		}
	}
#endif
}

//------------------------------------------------------------------------------
void
TessThread::SetThreadName(const char* name)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// update the Windows thread name so that it shows up correctly
	// in the Debugger
	struct THREADNAME_INFO
	{
		DWORD dwType;     // must be 0x1000
		LPCSTR szName;    // pointer to name (in user address space)
		DWORD dwThreadID; // thread ID (-1 = caller thread)
		DWORD dwFlags;    // reserved for future use, must be zero
	};

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = ::GetCurrentThreadId();
	info.dwFlags = 0;
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#elif LORD_PLATFORM == LORD_PLATFORM_ANDROID
	//prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
#else
	pthread_setname_np(name);
#endif
}
//------------------------------------------------------------------------------
const char*
TessThread::GetThreadName()
{
	return "MayThread";
}

void TessThread::Sleep(int ms)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	::Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}


VisualTessThread::VisualTessThread()
	: m_startEvent(true)
	, m_finishEvent(true)
	, m_timeBegin(0)
	, m_timeEnd(0)
	, m_renderGlobal(nullptr)
	, m_actived(false)
{
	SetPriority(Normal);
}

VisualTessThread::~VisualTessThread()
{
}

void VisualTessThread::Update(RenderGlobal* world)
{
	m_actived = true;
	m_renderGlobal = world;
	m_startEvent.Signal();
}

bool VisualTessThread::WaitForFinish(unsigned int ms)
{
	bool ret = m_finishEvent.Wait(ms);
	m_finishEvent.Reset();
	return ret;
}

ui64 VisualTessThread::GetRuntime()
{
	return m_timeEnd - m_timeBegin;
}

void VisualTessThread::Stop()
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	assert(m_hThread != nullptr);
#else
	assert(m_hThread != 0);
#endif
	m_stopEvent.Signal();
	m_startEvent.Signal();

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// wait for the thread to terminate
	WaitForSingleObject((HANDLE)m_hThread, INFINITE);
	CloseHandle((HANDLE)m_hThread);
	m_hThread = nullptr;
#else
	if (m_hThread != 0)
	{
		bool ret = pthread_join(m_hThread, nullptr);
		m_hThread = 0;
		if (0 == ret)
		{
			return;
		}
		else
		{
			pthread_kill(m_hThread, 9);
		}
	}
#endif
}

void VisualTessThread::Run()
{
	SetThreadName("VisualTessThread");

	while (true)
	{
		if (m_stopEvent.Wait(1))
		{
			break;
		}

		try
		{
			if (m_startEvent.Wait(0xFFFFFFFF))
			{
				m_startEvent.Reset();
				m_timeBegin = LORD::Time::Instance()->getMicroseconds();
				if (m_renderGlobal)
				{
					m_renderGlobal->setupTerrain(0.005f);
				}
				m_timeEnd = LORD::Time::Instance()->getMicroseconds();;
				m_finishEvent.Signal();
			}
		}
		catch (...)
		{
			LordLogError("Error in VisualTessThread");
		}
	}
}

LightTransferThread::LightTransferThread()
	: m_transferdStarted(0)
	, m_transferdFinished(0)
	, m_pChunkService(nullptr)
	, m_startEvent(true)
	, m_finishEvent(true)
	, m_actived(false)
{
}

LightTransferThread::~LightTransferThread()
{}

void LightTransferThread::SetChunkService(ChunkService* pChunkService)
{
	m_pChunkService = pChunkService;
}

void LightTransferThread::Stop()
{
	assert(m_hThread != nullptr);

	m_stopEvent.Signal();
	m_startEvent.Signal();

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// wait for the thread to terminate
	WaitForSingleObject((HANDLE)m_hThread, INFINITE);
	CloseHandle((HANDLE)m_hThread);
	m_hThread = nullptr;
#else
	if (m_hThread != 0)
	{
		bool ret = pthread_join(m_hThread, nullptr);
		m_hThread = 0;
		if (0 == ret)
		{
			return;
		}
		else
		{
			pthread_kill(m_hThread, 9);
		}
	}
#endif
}

void LightTransferThread::Update()
{
	m_actived = true;
	m_startEvent.Signal();
}

bool LightTransferThread::CheckFinish()
{
	bool ret = m_finishEvent.Wait(0);
	if(ret)
		m_finishEvent.Reset();

	return ret;
}

bool LightTransferThread::addTask(ChunkPtr chunk)
{
	bool ret = false;

	if (!chunk)
		return ret;

	Vector2i sectionID = Vector2i(chunk->m_posX, chunk->m_posZ);
	constexpr int neighbors[8][2] = 
	{
		{ -1, 0 },
		{ 1, 0 },
		{ 0, -1 },
		{ 0, 1 },
		{ -1, -1 },
		{ -1, 1 },
		{ 1, -1 },
		{ 1, 1 }
	};

	// check neighbor.
	for (size_t i = 0; i < 8; ++i)
	{
		auto pNeighbor = m_pChunkService->getChunk(sectionID.x + neighbors[i][0], sectionID.y + neighbors[i][1]);
		if (!pNeighbor)
			return false;
	}

	m_cs.Lock();
	auto findIt = m_tasks.find(sectionID);
	if (findIt == m_tasks.end())
	{
		m_tasks.insert({ sectionID, chunk });
	}
	m_cs.Unlock();

	return ret;
}


void LightTransferThread::Run()
{
	SetThreadName("LightTransferThread");
	
	constexpr int neighbors[8][2] =
	{
		{ -1, 0 },
		{ 1, 0 },
		{ 0, -1 },
		{ 0, 1 },
		{ -1, -1 },
		{ -1, 1 },
		{ 1, -1 },
		{ 1, 1 }
	};

	while (true)
	{
		if (m_stopEvent.Wait(1))
		{
			break;
		}
		Sleep(5);
		
		try
		{
			if (m_startEvent.Wait(0xFFFFFFFF))
			{
				m_startEvent.Reset();
				
				m_cs.Lock();
				ChunkPtr pChunk = GetPriorityChunk();
				m_cs.Unlock();
				
				if (pChunk != nullptr)
				{
					Vector2i sectionID = Vector2i(pChunk->m_posX, pChunk->m_posZ);
					// hold another 8 chunks
					ChunkPtr pNeighbors[8];
					bool neighborExist = true;
					for (size_t i = 0; i < 8; ++i)
					{
						pNeighbors[i] = m_pChunkService->getChunk(sectionID.x + neighbors[i][0], sectionID.y + neighbors[i][1]);
						if (!pNeighbors[i])
							neighborExist = false;
					}

					if (neighborExist && rebuildChunkImpl(pChunk))
					{
						m_cs.Lock();
						m_tasks.erase(sectionID);
						m_cs.Unlock();

						m_histroy.push_back(sectionID);
						if (m_histroy.size() > 20)
							m_histroy.pop_front();
					}
				}
				else
				{
					Vector2i chooseKey(Math::MAX_I32, Math::MAX_I32);
					m_histroy.push_back(chooseKey);
					if (m_histroy.size() > 20)
						m_histroy.pop_front();
				}

				m_finishEvent.Signal();
			}
		}
		catch (...)
		{
			LordLogError("Error in LightTransferThread");
		}
	}
}

ChunkPtr LightTransferThread::GetPriorityChunk()
{
	ChunkPtr pResult(nullptr);

	Camera* mainCamera = SceneManager::Instance()->getMainCamera();
	if (!mainCamera)
		return pResult;

	Vector2 camPosProjY(mainCamera->getPosition().x, mainCamera->getPosition().z);

	const Frustum& frustum = mainCamera->getFrustum();

	float shortestDistance = Math::MAX_FLOAT;
	Vector2i chooseKey(Math::MAX_I32, Math::MAX_I32);
	for (const auto& it : m_tasks)
	{
		float x = it.first.x * 16.f;
		float z = it.first.y * 16.f;
		Vector2 chunkPosProjY(x + 8.f, z + 8.f);
		Box aabb(Vector3(x, 0.f, z), Vector3(x + 16.f, 256.f, z + 16.f));
		if (frustum.intersect(aabb))
		{
			float distance = (camPosProjY - chunkPosProjY).lenSqr();
			if (distance > 16*16 && std::find(m_histroy.begin(), m_histroy.end(), it.first) != m_histroy.end())
				continue;
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
				pResult = it.second;
				chooseKey = it.first;
			}
		}
	}
	
	return pResult;
}


bool LightTransferThread::rebuildChunkImpl(ChunkPtr ptrChunk)
{
	m_transferdStarted++;

	auto pChunk = std::static_pointer_cast<ChunkClient>(ptrChunk);
	pChunk->m_needRebuildAll = false;
	
	unsigned long t0 = Time::Instance()->getMicroseconds();
	pChunk->generateSkylightMap();

	if (pChunk->checkNeedRebuildAll())
		return false;

	unsigned long t1 = Time::Instance()->getMicroseconds();
	pChunk->updateSkylight();
	if (pChunk->checkNeedRebuildAll())
		return false;
	if (!pChunk->m_isSkyLightingUpdated) // not finish
		return false;

	// add by zhouyou. client need rebuild the block light data!
	unsigned long t2 = Time::Instance()->getMicroseconds();
	pChunk->updateBlocklight();
	if (pChunk->checkNeedRebuildAll())
		return false;
	if (!pChunk->m_isBlockLightingUpdated)
		return false;

	// second rebuild the visibility.
	unsigned long t3 = Time::Instance()->getMicroseconds();
	pChunk->reBuildVisibility();
	if (pChunk->checkNeedRebuildAll())
		return false;
	unsigned long t4 = Time::Instance()->getMicroseconds();

	m_transferdFinished++;
	/*LordLogInfo("******* RebuildChunkImpl[%4lld] pos(%4d,%4d) map:%5d sky:%10d block:%10d visibility:%10d", 
		m_transferdFinished, pChunk->m_posX, pChunk->m_posZ, t1 - t0, t2 - t1, t3 - t2, t4 - t3);*/
	return true;
}
}// namespace BLOCKMAN
#ifndef __WORKER_THREAD_HEADER__
#define __WORKER_THREAD_HEADER__

#include <atomic>
#include "Std/functional.h"
#include "Core.h"
#include "Util/Concurrent/Future.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Util/UThread.h"
#include "PackagedTask.h"

using namespace LORD;

namespace BLOCKMAN
{

	namespace WORKER_THREAD
	{
		class WorkerTask : public LORD::ObjectAlloc
		{
		public:
			virtual ~WorkerTask() = default;
			virtual bool run() = 0;
			virtual bool cancel() = 0;
		};

		template<typename Ret>
		class WorkerTaskImpl : public WorkerTask
		{
		private:
			PackagedTask<Ret()> m_task;

		public:
			WorkerTaskImpl(std::function<Ret()> func)
				: m_task(func)
			{}

			virtual bool run() override
			{
				return m_task();
			}

			virtual bool cancel() override
			{
				return m_task.cancel();
			}

			Future<Ret> getFuture()
			{
				return m_task.getFuture();
			}
		};

		using WorkerTaskPtr = std::shared_ptr<WorkerTask>;

		/* this class is NOT thread-safe */
		class WorkerThread : public ObjectAlloc
		{
		private:
			enum class Status : ui8
			{
				READY,
				RUNNING,
				STOPPED
			};

			// run in the same thread in debug mode
			const bool m_isDebug = false;
			// for statistics and debugging
			std::atomic_uint m_finishedTaskCount{ 0 };
			std::atomic<Status> m_status{ Status::READY };
			UThread m_thread;
			ThreadSafeQueue<WorkerTaskPtr> m_taskQueue;

			void mainLoop(StopFlag shouldStop);
			void addTaskImpl(const WorkerTaskPtr& task);

		public:
			static WorkerThread& getDefault();

			WorkerThread(const String& name = "WorkerThread", ThreadPriority priority = ThreadPriority::NORMAL, bool isDebug = false)
				: m_isDebug(isDebug)
				, m_thread(name, &WorkerThread::mainLoop, this)
			{
				m_thread.setPriority(priority);
			}

			~WorkerThread()
			{
				if (m_status == Status::RUNNING)
				{
					LordLogWarning("WorkerThread %s is destructed while it's still running", m_thread.getName().c_str());
					stopSync();
				}
			}

			void start();
			void stopAsync();
			void stopSync();
			void cancelAllTasks();

			// This method is safe to be called from other threads
			size_t getTaskCount()
			{
				return m_taskQueue.size();
			}

			// This method is safe to be called from other threads
			unsigned getFinishedTaskCount()
			{
				return m_finishedTaskCount.load();
			}

			// This method is safe to be called from other threads
			template<typename Func, typename ... Args>
			auto addTask(Func&& func, Args&& ... args) -> Future<typename function_traits<std::remove_reference_t<Func>>::return_type>
			{
				using ReturnType = typename function_traits<std::remove_reference_t<Func>>::return_type;
				auto wrappedFunc = [func, args ...]() mutable -> ReturnType {
					return std::invoke(std::forward<Func>(func), std::forward<Args>(args) ...);
				};
				auto task = LORD::make_shared<WorkerTaskImpl<ReturnType>>(wrappedFunc);
				addTaskImpl(task);
				return task->getFuture();
			}
		};
	}

	using WORKER_THREAD::WorkerThread;
}

#endif // !__WORKER_THREAD_HEADER__

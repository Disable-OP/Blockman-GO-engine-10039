#ifndef __SPIN_MUTEX_HEADER__
#define __SPIN_MUTEX_HEADER__

#include <thread>
#include <atomic>

#define DEBUG_LOCK_CONTENTION 0

#if DEBUG_LOCK_CONTENTION
#define LOG_CALLSTACK(msg, ...)  LordLogDebug(msg, ##__VA_ARGS__);  Log::printCallStack(); 
#else
#define LOG_CALLSTACK 
#endif

namespace BLOCKMAN
{
	class SpinMutex
	{
	private:
		std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

	public:
		SpinMutex() = default;
		SpinMutex(const SpinMutex&) = delete;
		SpinMutex& operator=(const SpinMutex&) = delete;

		void lock() noexcept
		{
			while (m_flag.test_and_set(std::memory_order::memory_order_acquire))
			{
				LOG_CALLSTACK("SpinLock contention");
				std::this_thread::yield();
			}
		}

		bool try_lock() noexcept
		{
			return !m_flag.test_and_set(std::memory_order::memory_order_acquire);
		}

		void unlock() noexcept
		{
			m_flag.clear(std::memory_order::memory_order_release);
		}
	};
}

#undef DEBUG_LOCK_CONTENTION
#undef LOG_CALLSTACK

#endif // !__SPIN_LOCK_HEADER__

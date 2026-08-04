#ifndef __SHARED_SPIN_MUTEX_HEADER__
#define __SHARED_SPIN_MUTEX_HEADER__

#include <atomic>
#include <mutex>
#include <thread>
#include <limits>

#define DEBUG_LOCK_CONTENTION 0

#if DEBUG_LOCK_CONTENTION
#define LOG_CALLSTACK(msg, ...)  LordLogDebug(msg, ##__VA_ARGS__);  Log::printCallStack(); 
#else
#define LOG_CALLSTACK 
#endif

namespace BLOCKMAN
{
	class SharedSpinMutex
	{
	private:
		using CountType = int;
		static constexpr CountType MIN_NUM = (std::numeric_limits<CountType>::min)();
		std::mutex m_writeMutex;
		std::atomic<CountType> m_readCount{ 0 };

	public:
		void lock()
		{
			m_writeMutex.lock();
			CountType expected = 0;
			while (!m_readCount.compare_exchange_weak(expected, MIN_NUM, 
				std::memory_order::memory_order_acquire, std::memory_order::memory_order_relaxed))
			{
				LOG_CALLSTACK("SharedSpinMutex contention at lock()");
				expected = 0;
				std::this_thread::yield();
			}
		}

		bool try_lock()
		{
			if (!m_writeMutex.try_lock())
			{
				return false;
			}
			CountType expected = 0;
			if (m_readCount.compare_exchange_strong(expected, MIN_NUM,
				std::memory_order::memory_order_acquire, std::memory_order::memory_order_relaxed))
			{
				return true;
			}
			m_writeMutex.unlock();
			return false;
		}

		void unlock()
		{
			m_readCount.fetch_sub(MIN_NUM, std::memory_order::memory_order_release);
			m_writeMutex.unlock();
		}

		void lock_shared()
		{
			if (m_readCount.fetch_add(1, std::memory_order::memory_order_acquire) >= 0)
			{
				return;
			}
			while (m_readCount.load(std::memory_order::memory_order_acquire) <= 0)
			{
				LOG_CALLSTACK("SharedSpinMutex contention at lock_shared()");
				std::this_thread::yield();
			}
		}

		bool try_lock_shared()
		{
			if (m_readCount.fetch_add(1, std::memory_order::memory_order_acquire) >= 0)
			{
				return true;
			}
			m_readCount.fetch_sub(1, std::memory_order::memory_order_relaxed);
			return false;
		}

		void unlock_shared()
		{
			m_readCount.fetch_sub(1, std::memory_order::memory_order_release);
		}
	};
}

#undef DEBUG_LOCK_CONTENTION
#undef LOG_CALLSTACK

#endif // !__SHARED_SPIN_MUTEX_HEADER__

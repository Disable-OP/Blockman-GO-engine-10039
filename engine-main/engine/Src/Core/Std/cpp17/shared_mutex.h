#ifndef __CPP17_SHARED_MUTEX_HEADER__
#define __CPP17_SHARED_MUTEX_HEADER__
#if __cplusplus < 201402L

#include <stdexcept>
#include <mutex>
#include <condition_variable>

namespace std
{
	class shared_mutex
	{
	private:
		enum class Status
		{
			IDLE,
			READING,
			WRITING
		};
		Status m_status = Status::IDLE;
		unsigned long long m_writeWaitCount = 0;
		unsigned long long m_readCount = 0;
		mutex m_mutex;
		condition_variable m_lockRelease;

	public:
		shared_mutex() = default;
		shared_mutex(const shared_mutex&) = delete;
		~shared_mutex() = default;
		shared_mutex& operator=(const shared_mutex&) = delete;

		void lock()
		{
			unique_lock<mutex> lock(m_mutex);
			++m_writeWaitCount;
			if (m_status != Status::IDLE)
			{
				m_lockRelease.wait(lock, [this] { return m_status == Status::IDLE; });
			}
			--m_writeWaitCount;
			m_status = Status::WRITING;
		}
		bool try_lock()
		{
			unique_lock<mutex> lock(m_mutex);
			if (m_status != Status::IDLE)
			{
				return false;
			}
			m_status = Status::WRITING;
			return true;
		}
		void unlock()
		{
			unique_lock<mutex> lock(m_mutex);
			m_status = Status::IDLE;
			m_lockRelease.notify_all();
		}
		void lock_shared()
		{
			unique_lock<mutex> lock(m_mutex);
			if (m_status == Status::READING && m_writeWaitCount > 0)
			{
				m_lockRelease.wait(lock, [this] { return m_status != Status::READING; });
			}
			if (m_status == Status::WRITING)
			{
				m_lockRelease.wait(lock, [this] { return m_status != Status::WRITING; });
			}
			++m_readCount;
			m_status = Status::READING;
		}
		bool try_lock_shared()
		{
			unique_lock<mutex> lock(m_mutex);
			if (m_status == Status::READING && m_writeWaitCount > 0)
			{
				return false;
			}
			if (m_status == Status::WRITING)
			{
				return false;
			}
			++m_readCount;
			m_status = Status::READING;
			return true;
		}
		void unlock_shared()
		{
			unique_lock<mutex> lock(m_mutex);
			if (m_readCount == 0)
			{
				throw std::logic_error("unlock_shared() is called more number of shared_lock");
			}
			if (--m_readCount == 0)
			{
				m_status = Status::IDLE;
				m_lockRelease.notify_all();
			}
		}

	};

	template<typename Mutex>
	class shared_lock
	{
	public:
		using mutex_type = Mutex;

	private:
		bool m_owned = false;
		mutex_type* m_mutex = nullptr;

	public:
		shared_lock() = default;
		shared_lock(shared_lock&& other)
			: m_owned(other.m_owned)
			, m_mutex(other.m_mutex)
		{
			other.m_owned = false;
			other.m_mutex = nullptr;
		}
		explicit shared_lock(mutex_type& m)
			: m_mutex(&m)
		{
			m_mutex->lock_shared();
			m_owned = true;
		}
		shared_lock& operator=(shared_lock&& other)
		{
			if (owns_lock())
			{
				m_mutex->unlock_shared();
			}
			m_mutex = other.m_mutex;
			m_owned = other.m_owned;
			other.m_mutex = nullptr;
			other.m_owned = false;
		}
		~shared_lock()
		{
			if (owns_lock())
			{
				m_mutex->unlock_shared();
				m_owned = false;
			}
		}

		void lock()
		{
			if (m_mutex)
			{
				if (m_owned)
				{
					throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur));
				}
				else
				{
					m_mutex->lock_shared();
					m_owned = true;
				}
			}
			else
			{
				throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
			}
		}

		void unlock()
		{
			if (owns_lock())
			{
				m_mutex->unlock_shared();
				m_owned = false;
			}
			else
			{
				throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
			}
		}

		mutex_type* release() noexcept
		{
			return m_mutex = nullptr;
		}

		mutex_type* mutex() const noexcept
		{
			return m_mutex;
		}

		bool owns_lock() const noexcept
		{
			return m_mutex && m_owned;
		}

		explicit operator bool() const noexcept
		{
			return owns_lock();
		}
	};
}

#elif __cplusplus < 201703L

#include <shared_mutex>

namespace std
{
	using shared_mutex = shared_timed_mutex;
}

#endif
#endif

#ifndef __UTHREAD_BASE_HEADER__
#define __UTHREAD_BASE_HEADER__

#include "Core.h"
#include "TypeTraits/FunctionTraits.h"
#include <thread>
#include <atomic>
#include <system_error>

using namespace LORD;

enum class ThreadPriority
{
	LOW,
	NORMAL,
	HIGH
};

using StopFlag = const std::atomic_bool&;

/* this is not thread-safe */
class UThreadBase
{
public:
	using NativeHandle = BLOCKMAN::function_traits<decltype(&std::thread::native_handle)>::return_type;
	enum class Status : ui8
	{
		EMPTY,
		READY,
		RUNNING,
		JOINED,
		DETACHED
	};

	UThreadBase() = default;
	template<typename Function, typename ... Args>
	UThreadBase(const String & name, Function&& threadFunc, Args&& ... args)
		: m_name(name)
		, m_threadFunc(std::bind(std::forward<Function>(threadFunc), std::forward<Args>(args) ..., std::cref(m_shouldStop)))
		, m_status(Status::READY)
	{
	}
	UThreadBase(UThreadBase&& that) = default;
	UThreadBase& operator=(UThreadBase&& that) = default;

	~UThreadBase()
	{
		if (m_status == Status::RUNNING)
		{
			LordLogWarning("UThread %s is destructed while it's still running", getName().c_str());
		}
	}

	bool start()
	{
		switch (m_status)
		{
		case Status::EMPTY:
			LordLogError("Failed to start UThread: UThread is empty");
			return false;
		case Status::READY:
			try
			{
				m_thread = std::thread(&UThreadBase::execute, this);
				m_status = Status::RUNNING;
				setName(m_name);
				setPriority(m_priority);
				return true;
			}
			catch (const std::system_error & e)
			{
				LordLogError("%s", e.code().message().c_str());
				return false;
			}
		case Status::RUNNING:
			LordLogError("Failed to start UThread: UThread is already running");
			return false;
		case Status::JOINED:
			LordLogError("Failed to start UThread: UThread is already joined");
			return false;
		case Status::DETACHED:
			LordLogError("Failed to start UThread: UThread is already detached");
			return false;
		default:
			LordLogError("Failed to start UThread: Unknown UThread status %d", int(m_status));
			return false;
		}
	}

	bool stopSync()
	{
		if (!assertCanStop())
		{
			return false;
		}
		m_shouldStop = true;
		return tryJoin();
	}

	bool stopAsync()
	{
		if (!assertCanStop())
		{
			return false;
		}
		m_shouldStop = true;
		return tryDetach();
	}

	bool join()
	{
		if (!assertCanStop())
		{
			return false;
		}
		return tryJoin();
	}

	bool detach()
	{
		if (!assertCanStop())
		{
			return false;
		}
		return tryDetach();
	}

	bool joinable()
	{
		return m_thread.joinable();
	}

	String getName() const
	{
		return m_name;
	}

	virtual void setName(const String& name) = 0;
	virtual void setPriority(ThreadPriority priority) = 0;
	virtual int getNativeId() = 0;

private:
	virtual void execute()
	{
		m_status = Status::RUNNING;
		m_threadFunc();
	}

	bool assertCanStop()
	{
		switch (m_status)
		{
		case Status::EMPTY:
			LordLogError("Failed to stop UThread: UThread is empty");
			return false;
		case Status::READY:
			LordLogError("Failed to stop UThread: UThread is not started");
			return false;
		case Status::RUNNING:
			return true;
		case Status::JOINED:
			LordLogError("Failed to stop UThread: UThread is already joined");
			return false;
		case Status::DETACHED:
			LordLogError("Failed to stop UThread: UThread is already detached");
			return false;
		default:
			LordLogError("Failed to stop UThread: Unknown UThread status %d", int(m_status));
			return false;
		}
	}

	bool tryJoin()
	{
		try
		{
			m_thread.join();
			m_status = Status::JOINED;
			return true;
		}
		catch (const std::system_error & e)
		{
			LordLogError("%s", e.code().message().c_str());
			m_status = Status::JOINED;
			return false;
		}
	}

	bool tryDetach()
	{
		try
		{
			m_thread.detach();
			m_status = Status::DETACHED;
			return true;
		}
		catch (const std::system_error & e)
		{
			LordLogError("%s", e.code().message().c_str());
			m_status = Status::DETACHED;
			return false;
		}
	}

protected:
	std::thread m_thread;
	String m_name;
	std::function<void(void)> m_threadFunc;
	ThreadPriority m_priority = ThreadPriority::NORMAL;
	Status m_status = Status::EMPTY;
	std::atomic_bool m_shouldStop{ false };
};

#endif
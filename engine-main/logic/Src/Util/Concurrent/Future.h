#ifndef __FUTURE_HEADER__
#define __FUTURE_HEADER__

#include <mutex>
#include <condition_variable>
#include "Core.h"
#include "Std/type_traits.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum class FutureStatus : char
	{
		NOT_STARTED,
		RUNNING,
		FINISHED,
		FAILED,
		CANCELED
	};

	namespace FUTURE_DETAIL
	{
		template<typename T>
		struct FinishCallbackOf
		{
			using type = std::function<void(const T&)>;
		};

		template<>
		struct FinishCallbackOf<void>
		{
			using type = std::function<void()>;
		};
	}

	class GetCanceledFutureException : public std::exception
	{
	public:
		virtual char const* what() const noexcept
		{
			return "Invoked get() on a canceled future";
		}
	};

	class RunAgainException : public std::exception
	{
	public:
		virtual char const* what() const noexcept
		{
			return "Tried to change FutureStatus to RUNNING, but it's not NOT_STARTED";
		}
	};

	template<typename T>
	class FutureState : public LORD::ObjectAlloc
	{
#define LOCK(mut) std::lock_guard<decltype(mut)> lock(mut);
	private:
		using FinishCallback = typename FUTURE_DETAIL::FinishCallbackOf<T>::type;
		using ValueType = std::conditional_t<std::is_same<T, void>::value, int, T>;
		using IsVoidFuture = std::integral_constant<bool, std::is_same<T, void>::value>;

		FutureStatus m_status = FutureStatus::NOT_STARTED;
		ValueType m_value;
		FinishCallback m_finishCallback;
		std::function<void()> m_cancelCallback; 
		std::function<void(std::exception_ptr)> m_failCallback;
		bool m_callbackCalled = false;
		std::mutex m_mutex;
		std::condition_variable m_cond;
		std::exception_ptr m_except;

		void runFinishCallback(const FinishCallback& callback, std::true_type)
		{
			callback();
		}

		void runFinishCallback(const FinishCallback& callback, std::false_type)
		{
			callback(m_value);
		}

		bool isEnded() const
		{
			return m_status == FutureStatus::FINISHED
				|| m_status == FutureStatus::CANCELED
				|| m_status == FutureStatus::FAILED;
		}

		/* 
		* IMPORTANT: returned function must be called outside the locked area. 
		* invoking callback function inside the lock is dangerous.
		*/
		std::function<void()> createCallbackFunction()
		{
			LordAssertX(isEnded(), "Future::createCallbackFunction() must be called after future is ended");
			switch (m_status)
			{
			case FutureStatus::FINISHED:
				if (m_finishCallback && !m_callbackCalled)
				{
					m_callbackCalled = true;
					// copy m_finishCallback in case it is changed after the lock is released
					auto callback = m_finishCallback;
					// IMPORTANT: m_value cannot be changed after return
					return [this, callback] {
						runFinishCallback(callback, IsVoidFuture());
					};
				}
			case FutureStatus::FAILED:
				if (m_failCallback && !m_callbackCalled)
				{
					m_callbackCalled = true;
					// copy m_failCallback in case it is changed after the lock is released
					auto callback = m_failCallback;
					// IMPORTANT: m_except cannot be changed after this line
					return [this, callback] {
						callback(m_except);
					};
				}
			case FutureStatus::CANCELED:
				if (m_cancelCallback && !m_callbackCalled)
				{
					m_callbackCalled = true;
					// copy m_cancelCallback in case it is changed after the lock is released
					return m_cancelCallback;
				}
			}
			return [] {};
		}

	public:
		template<typename R = T>
		std::enable_if_t<std::is_same<R, T>::value> setValue(const R& value)
		{
			LOCK(m_mutex);
			if (m_status != FutureStatus::RUNNING)
			{
				throw std::logic_error("FutureState::setValue() is called but status is not RUNNING");
			}
			m_status = FutureStatus::FINISHED;
			m_value = value;
			m_cond.notify_all();
		}

		template<typename R = T>
		std::enable_if_t<std::is_same<R, T>::value && std::is_void<T>::value> setValue()
		{
			LOCK(m_mutex);
			if (m_status != FutureStatus::RUNNING)
			{
				throw std::logic_error("FutureState::setValue() is called but status is not RUNNING");
			}
			m_status = FutureStatus::FINISHED;
			m_cond.notify_all();
		}

		void setException(std::exception_ptr eptr)
		{
			LOCK(m_mutex);
			if (m_status != FutureStatus::RUNNING)
			{
				throw std::logic_error("FutureState::setValue() is called but status is not RUNNING");
			}
			m_status = FutureStatus::FAILED;
			m_except = eptr;
			m_cond.notify_all();
		}

		T getValue()
		{
			std::unique_lock<decltype(m_mutex)> lock(m_mutex);
			if (!isEnded())
			{
				m_cond.wait(lock, [this]() {
					return isEnded();
				});
			}
			auto callback = createCallbackFunction();
			/*
			* IMPORTANT: lock can be released because it's assumed that m_value, m_status and m_except
			* CANNOT be changed after this line. In the other words, all 3 variables will NOT change
			* after the future is ended.
			*/
			lock.unlock();
			// NOTE: callback must be called outside the locked area
			callback();
			switch (m_status)
			{
			case FutureStatus::FINISHED:
				return static_cast<T>(m_value);
			case FutureStatus::FAILED:
				std::rethrow_exception(m_except);
			case FutureStatus::CANCELED:
				throw GetCanceledFutureException();
			}
			throw std::logic_error("Impossible to reach this line");
		}

		bool cancel()
		{
			std::unique_lock<decltype(m_mutex)> lock(m_mutex);
			if (m_status == FutureStatus::NOT_STARTED)
			{
				m_status = FutureStatus::CANCELED;
				auto callback = createCallbackFunction();
				lock.unlock();
				callback();
				return true;
			}
			return false;
		}

		bool changeToRunning()
		{
			LOCK(m_mutex);
			switch (m_status)
			{
			case FutureStatus::NOT_STARTED:
				m_status = FutureStatus::RUNNING;
				return true;
			case FutureStatus::CANCELED:
				return false;
			case FutureStatus::RUNNING:
			case FutureStatus::FINISHED:
			case FutureStatus::FAILED:
				throw RunAgainException();
			default:
				throw std::logic_error("impossible to execute this line in changeToRunning()!");
			}
		}

		FutureStatus getStatus()
		{
			LOCK(m_mutex);
			return m_status;
		}

		void setFinishCallback(const FinishCallback& callback)
		{
			LOCK(m_mutex);
			m_finishCallback = callback;
		}

		void setFailCallback(std::function<void(std::exception_ptr)> callback)
		{
			LOCK(m_mutex);
			m_failCallback = callback;
		}

		void setCancelCallback(std::function<void()> callback)
		{
			LOCK(m_mutex);
			m_cancelCallback = callback;
		}

		// return false if task is not ended, i.e. NOT_STARTED or RUNNING
		bool runCallback()
		{
			std::function<void()> callback;
			{
				LOCK(m_mutex);
				if (!isEnded())
				{
					return false;
				}
				callback = createCallbackFunction();
			}
			// NOTE: callback must be called outside the locked area
			callback();
			return true;
		}
#undef LOCK
	};

	template<typename T>
	using FutureStatePtr = std::shared_ptr<FutureState<T>>;

	// Future must not be moved to or used by other threads!
	template<typename T>
	class Future : public LORD::ObjectAlloc
	{
	private:
		FutureStatePtr<T> m_state;

	public:
		using FinishCallback = typename FUTURE_DETAIL::FinishCallbackOf<T>::type;

		explicit Future(FutureStatePtr<T> state)
			: m_state(state) {}
		Future() = default;
		Future(Future&&) = default;
		Future(const Future&) = delete;
		Future& operator=(const Future&) = delete;
		Future& operator=(Future&&) = default;

		T get()
		{
			return m_state->getValue();
		}

		bool cancel()
		{
			return m_state->cancel();
		}

		bool isFinished()
		{
			return m_state->getStatus() == FutureStatus::FINISHED;
		}

		bool isCanceled()
		{
			return m_state->getStatus() == FutureStatus::CANCELED;
		}

		/* 
		* Set a callback which will be called in the same thread that owns the Future.
		* It is guaranteed that callback is called before the Future::get() returns
		*/
		Future& onFinish(const FinishCallback& callback)
		{
			m_state->setFinishCallback(callback);
			return *this;
		}

		/*
		* Set a callback which will be called in the same thread that owns the Future.
		* It is guaranteed that callback is called before the Future::get() returns
		*/
		Future& onFail(std::function<void(std::exception_ptr)> callback)
		{
			m_state->setFailCallback(callback);
			return *this;
		}

		/*
		* Set a callback which will be called in the same thread that owns the Future.
		* It is guaranteed that callback is called before the Future::cancel() returns
		*/
		Future& onCancel(std::function<void()> callback)
		{
			m_state->setCancelCallback(callback);
			return *this;
		}
	};
}

#endif // !__FUTURE_HEADER__

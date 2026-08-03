#ifndef __PROMISE_HEADER__
#define __PROMISE_HEADER__

#include "Std/type_traits.h"
#include "Future.h"
#include "FutureCallback.h"

namespace BLOCKMAN
{
	namespace PROMISE
	{
		using CallbackQueue = LORD::vector<FutureCallback>::type;
		extern thread_local CallbackQueue callbackQueue;
		// This function should be called periodically in every thread that has Future
		void runCallbacks();
		void clearCallbacks();
	}

	class FutureGeneratedAgainException : public std::exception
	{
	public:
		virtual char const* what() const noexcept
		{
			return "getFuture() is called more than once on the same Promise";
		}
	};

	template<typename T>
	class Promise : public LORD::ObjectAlloc
	{
	public:
		Promise() = default;
		Promise(const Promise&) = delete;
		Promise(Promise&&) = default;
		Promise& operator=(const Promise&) = delete;
		Promise& operator=(Promise&&) = default;

		~Promise()
		{
			cancel();
		}

		Future<T> getFuture()
		{
			if (m_futureGenerated)
			{
				throw FutureGeneratedAgainException();
			}
			m_futureGenerated = true;
			PROMISE::callbackQueue.emplace_back(m_state);
			return Future<T>(m_state);
		}

		bool changeToRunning()
		{
			return m_state->changeToRunning();
		}

		template<typename R = T>
		std::enable_if_t<std::is_same<R, T>::value> setValue(R&& value)
		{
			setValue(value);
		}

		template<typename R = T>
		std::enable_if_t<std::is_same<R,T>::value> setValue(const R& value)
		{
			m_state->setValue(value);
		}

		template<typename R = T>
		std::enable_if_t<std::is_same<R, T>::value && std::is_void<R>::value> setValue()
		{
			m_state->setValue();
		}

		void setException(std::exception_ptr p)
		{
			m_state->setException(p);
		}

		bool cancel()
		{
			return m_state->cancel();
		}

	private:
		FutureStatePtr<T> m_state = LORD::make_shared<FutureState<T>>();
		bool m_futureGenerated = false;
	};
}

#endif // !__PROMISE_HEADER__

#ifndef __FUTURE_CALLBACK_HEADER__
#define __FUTURE_CALLBACK_HEADER__

#include "Core.h"
#include "Future.h"

namespace BLOCKMAN
{
	class FutureCallbackRunner : public LORD::ObjectAlloc
	{
	public:
		virtual bool run() = 0;
		virtual ~FutureCallbackRunner() = default;
	};

	template<typename T>
	class FutureCallbackRunnerImpl : public FutureCallbackRunner
	{
	public:
		FutureCallbackRunnerImpl(FutureStatePtr<T> state)
			: m_state(state)
		{}

		virtual bool run() override
		{
			return m_state->runCallback();
		}

	private:
		FutureStatePtr<T> m_state;
	};

	class FutureCallback : public LORD::ObjectAlloc
	{
	public:
		template<typename T>
		explicit FutureCallback(FutureStatePtr<T> state)
			: m_runner(LORD::make_shared<FutureCallbackRunnerImpl<T>>(state))
		{}
		FutureCallback() = default;
		FutureCallback(const FutureCallback&) = default;
		FutureCallback(FutureCallback&&) = default;
		FutureCallback& operator=(const FutureCallback&) = default;
		FutureCallback& operator=(FutureCallback&&) = default;

		// return false if task is not ended, i.e. FutureStatus is NOT_STARTED or RUNNING
		bool operator()() const
		{
			return m_runner->run();
		}

		explicit operator bool() const
		{
			return static_cast<bool>(m_runner);
		}

	private:
		std::shared_ptr<FutureCallbackRunner> m_runner;
	};
}

#endif // !__FUTURE_CALLBACK_HEADER__

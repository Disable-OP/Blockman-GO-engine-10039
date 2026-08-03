#ifndef __PACKAGED_TASK_HEADER__
#define __PACKAGED_TASK_HEADER__

#include "Core.h"
#include "Promise.h"

namespace BLOCKMAN
{
	template<typename T>
	class PackagedTask;

	template<typename Ret, typename ... Args>
	class PackagedTask<Ret(Args ...)> : public LORD::ObjectAlloc
	{
	public:
		PackagedTask() = default;
		template<typename Func>
		explicit PackagedTask(Func&& func)
			: m_func(func)
		{}
		PackagedTask(const PackagedTask&) = delete;
		PackagedTask(PackagedTask&&) = default;
		PackagedTask& operator=(const PackagedTask&) = delete;
		PackagedTask& operator=(PackagedTask&&) = default;

		Future<Ret> getFuture()
		{
			return m_promise.getFuture();
		}

		// return false if task is canceled already
		bool operator()(Args... args)
		{
			if (!m_promise.changeToRunning())
			{
				return false;
			}
			try
			{
				run(std::integral_constant<bool, std::is_same<Ret, void>::value>(), std::forward<Args>(args)...);
				return true;
			}
			catch (...)
			{
				m_promise.setException(std::current_exception());
				return true;
			}
		}

		bool cancel()
		{
			return m_promise.cancel();
		}

	private:
		Promise<Ret> m_promise;
		std::function<Ret(Args ...)> m_func;

		template<typename ... _Args>
		void run(std::true_type, _Args&& ... args)
		{
			m_func(std::forward<_Args>(args) ...);
			m_promise.setValue();
		}

		template<typename ... _Args>
		void run(std::false_type, _Args&& ... args)
		{
			m_promise.setValue(m_func(std::forward<_Args>(args) ...));
		}
	};
}

#endif // !__PACKAGED_TASK_HEADER__

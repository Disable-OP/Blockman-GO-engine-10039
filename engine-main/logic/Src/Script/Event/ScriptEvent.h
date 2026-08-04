#pragma once
#include <string>
#include <functional>
#include <map>
#include "Core.h"

namespace SCRIPT_EVENT
{
	template<typename EventClass, typename ... Args>
	struct ScriptEvent
	{
		using Callback = std::function<bool(Args...)>;

		static void registerCallBack(Callback callback)
		{
			subscribers.push_back(callback);
		}

		template<typename ... T>
		static bool invoke(T&&... args)
		{
			if (subscribers.size() == 0)
				return true;

			bool ret = true;
			for (Callback subscriber : subscribers)
			{
				ret = subscriber(std::forward<T>(args)...);
			}

			return ret;
		}

	private:
		static typename LORD::list<Callback>::type subscribers;

	};

	template<typename EventClass, typename ... Args>
	typename LORD::list<typename ScriptEvent<EventClass, Args...>::Callback>::type ScriptEvent<EventClass, Args...>::subscribers = {};
}
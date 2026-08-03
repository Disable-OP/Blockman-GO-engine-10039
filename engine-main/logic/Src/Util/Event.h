#ifndef __EVENT_HEADER__
#define __EVENT_HEADER__

#include <functional>
#include "Core.h"

using namespace LORD;

#if (LORD_MODE == LORD_MODE_DEBUG)
#	define TRACK_EVENT_SUBSCRIPTION
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#		define WIN32_LEAN_AND_MEAN
#		include <Windows.h>
#		include <Windowsx.h>
#		define LordOutputCString(str) ::OutputDebugStringA(str)
#	else
#		define LordOutputCString(str) std::cerr << str
#	endif
#endif

namespace BLOCKMAN
{
	inline namespace EVENT
	{

#ifdef TRACK_EVENT_SUBSCRIPTION
		class EventManager
		{
		private:
			struct EventInfo
			{
				const char* eventName;
				std::function<size_t()> sizeFunction;
			};
			static vector<EventInfo>::type eventInfos;

		public:
			static void registerEvent(const char* eventName, std::function<size_t()> sizeFunction)
			{
				eventInfos.push_back({ eventName, sizeFunction });
			}

			static void reportLeaks()
			{
				StringStream ss;
				for (const auto& eventInfo : eventInfos)
				{
					auto size = eventInfo.sizeFunction();
					if (size > 0)
					{
						ss << "Event name: " << eventInfo.eventName
							<< ", number of unsubscribed subscription: " << size
							<< std::endl;
					}
				}
				LordOutputCString(ss.str().c_str());
			}
		};
#endif

		template<typename EventClass, typename ... Args>
		struct Event
		{
			using Callback = std::function<bool(Args...)>;
			class Subscription
			{
				using Iterator = typename list<Callback>::iterator;
				Iterator m_iter;
				bool unsubscribed = false;
			public:
				Subscription() : unsubscribed(true) {};
				explicit Subscription(const Iterator& iter) : m_iter(iter) {}
				void unsubscribe();
			};

			static Subscription subscribe(Callback callback)
			{
#ifdef TRACK_EVENT_SUBSCRIPTION
				static bool firstCall = true;
				if (firstCall)
				{
					EventManager::registerEvent(typeid(Event).name(), std::bind(&list<Callback>::type::size, &subscribers));
					firstCall = false;
				}
#endif
				subscribers.push_back(callback);
				return Subscription(--subscribers.end());
			}

			template<typename ... T>
			static void emit(T&&... args)
			{
				for (Callback subscriber : subscribers)
				{
					subscriber(std::forward<T>(args)...);
				}
			}
		private:
			static typename list<Callback>::type subscribers;
		};

		template<typename EventClass, typename ... Args>
		typename list<typename Event<EventClass, Args...>::Callback>::type Event<EventClass, Args...>::subscribers = {};

		template<typename EventClass, typename ...Args>
		void Event<EventClass, Args...>::Subscription::unsubscribe()
		{
			if (unsubscribed)
			{
				LordLogWarning("Event is unsubscribed more than once");
				return;
			}
			subscribers.erase(m_iter);
			unsubscribed = true;
		}
	}
}

#endif // !__EVENT_HEADER__


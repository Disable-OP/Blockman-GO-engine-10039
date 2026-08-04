#ifndef __SUBSCRIPTION_GUARD_HEADER__
#define  __SUBSCRIPTION_GUARD_HEADER__

#include <functional>
#include "Core.h"

namespace BLOCKMAN 
{
	class SubscriptionGuard
	{
	private:
		LORD::vector<std::function<void()>>::type m_unsubscribers;

	public:
		SubscriptionGuard() = default;
		SubscriptionGuard(SubscriptionGuard&) = delete;
		SubscriptionGuard(SubscriptionGuard&& that)
		{
			m_unsubscribers = std::move(that.m_unsubscribers);
		}
		SubscriptionGuard& operator=(SubscriptionGuard&) = delete;
		SubscriptionGuard& operator=(SubscriptionGuard&& that)
		{
			m_unsubscribers = std::move(that.m_unsubscribers);
			return *this;
		}

		template<typename Subscription>
		void add(Subscription&& subscription)
		{
			m_unsubscribers.push_back([subscription]() mutable -> void
			{
				subscription.unsubscribe();
			});
		}

		void unsubscribeAll()
		{
			for (const auto& unsubscriber : m_unsubscribers)
			{
				unsubscriber();
			}
			m_unsubscribers.clear();
		}

		~SubscriptionGuard()
		{
			unsubscribeAll();
		}
	};
}

#endif // __SUBSCRIPTION_GUARD_HEADER__

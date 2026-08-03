#ifndef __LISTENABLE_HEADER__
#define __LISTENABLE_HEADER__

#include <functional>
#include <memory>

#include "Core.h"
#include "TypeTraits/FunctionTraits.h"
#include "TypeTraits/EnableIfValid.h"
#include "TypeTraits/IsSmartPointer.h"

using namespace LORD;

namespace BLOCKMAN
{
	namespace LISTENABLE
	{
		enum class EventType
		{
			VOID_EVENT = -1,
			ZERO_ARG_EVENT,
			ONE_ARG_EVENT,
			TWO_ARGS_EVENT
		};

		template<EventType type>
		using EventTypeConstant = std::integral_constant<EventType, type>;

		template<typename EventClass>
		struct EventTypeOf : EventTypeConstant<static_cast<EventType>(function_traits<decltype(EventClass::emit)>::argument_size)> {};

		template<>
		struct EventTypeOf<void> : EventTypeConstant<EventType::VOID_EVENT> {};

		// Utility function for expression SFINAE. return the argument itself (identity function). 
		// This can force to delay the evaluation of an expression to template substitution stage.
		template<typename Param, typename T>
		auto identity(T&& variable) -> decltype(std::forward<T>(variable))
		{
			return std::forward<T>(variable);
		}
	}

	template<typename VariableType, typename Event = void>
	class Listenable
	{
	private:
		using EventType = LISTENABLE::EventType;
		template<EventType type>
		using EventTypeConstant = LISTENABLE::EventTypeConstant<type>;
		template<typename EventClass>
		using EventTypeOf = LISTENABLE::EventTypeOf<EventClass>;
		using Callback = std::function<bool(const VariableType&, const VariableType&)>;
		using SubscriberList = typename list<Callback>::type;

		VariableType m_data;
		std::shared_ptr<SubscriberList> m_subscribers;

		void emitEvent(const VariableType& old_data, const VariableType& new_data, EventTypeConstant<EventType::VOID_EVENT>)
		{
		}

		void emitEvent(const VariableType& old_data, const VariableType& new_data, EventTypeConstant<EventType::ZERO_ARG_EVENT>)
		{
			Event::emit();
		}

		void emitEvent(const VariableType& old_data, const VariableType& new_data, EventTypeConstant<EventType::ONE_ARG_EVENT>)
		{
			Event::emit(new_data);
		}

		void emitEvent(const VariableType& old_data, const VariableType& new_data, EventTypeConstant<EventType::TWO_ARGS_EVENT>)
		{
			Event::emit(old_data, new_data);
		}

		void notify(const VariableType& old_data, const VariableType& new_data)
		{
			for (const auto& subscriber : *m_subscribers)
			{
				subscriber(old_data, new_data);
			}
			emitEvent(old_data, new_data, EventTypeOf<Event>());
		}

	private:
		struct SubscriptionData
		{
			using Iterator = typename std::list<Callback>::iterator;

			Iterator iter;
			bool unsubscribed;
			std::weak_ptr<SubscriberList> subscribers;

			SubscriptionData(const Iterator& iter, std::shared_ptr<SubscriberList> subscribers)
				: iter(iter)
				, unsubscribed(false)
				, subscribers(subscribers)
			{
			}
		};

	public:
		class Subscription
		{
			using Iterator = typename list<Callback>::iterator;
			std::shared_ptr<SubscriptionData> m_data = nullptr;

		public:
			Subscription() = default;
			Subscription(const Iterator& iter, std::shared_ptr<SubscriberList> subscribers) 
				: m_data(LORD::make_shared<SubscriptionData>(iter, subscribers))
			{
			}
			void unsubscribe()
			{
				if (!m_data)
				{
					LordLogWarning("unsubscribe() is called on an empty Subscription");
					return;
				}
				if (m_data->unsubscribed)
				{
					LordLogWarning("Event is unsubscribed more than once");
					return;
				}
				auto subscribers = m_data->subscribers.lock();
				if (!subscribers)
				{
					return;
				}
				subscribers->erase(m_data->iter);
				m_data->unsubscribed = true;
			}
		};

		Listenable()
		{
			m_subscribers = LORD::make_shared<SubscriberList>();
		}
		explicit Listenable(const VariableType& data) : m_data(data) 
		{
			m_subscribers = LORD::make_shared<SubscriberList>();
		}
		Listenable(const Listenable& that) : m_data(that.m_data)
		{
			m_subscribers = LORD::make_shared<SubscriberList>();
		}
		Listenable& operator=(const Listenable& that)
		{
			auto old_data = m_data;
			m_data = that.m_data;
			notify(old_data, m_data);
			return *this;
		}
		Listenable& operator=(const VariableType& data)
		{
			auto old_data = m_data;
			m_data = data;
			notify(old_data, m_data);
			return *this;
		}

		operator VariableType() const
		{
			return m_data;
		}

		Subscription subscribe(std::function<bool(const VariableType&, const VariableType&)> callback)
		{
			m_subscribers->push_back(callback);
			return Subscription(--m_subscribers->end(), m_subscribers);
		}

		template<typename T = void>
		auto operator=(std::nullptr_t null) -> ENABLE_IF_VALID(LISTENABLE::identity<T>(m_data) = nullptr, Listenable&)
		{
			auto old_data = m_data;
			m_data = null;
			notify(old_data, m_data);
			return *this;
		}

		template<typename T>
		auto operator=(const T& data) -> ENABLE_IF_VALID(m_data = data, Listenable&)
		{
			auto old_data = m_data;
			m_data = data;
			notify(old_data, m_data);
			return *this;
		}

		template<typename T>
		auto operator+=(const T& data) -> ENABLE_IF_VALID(m_data += data, Listenable&)
		{
			auto old_data = m_data;
			m_data += data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T>
		auto operator-=(const T& data) -> ENABLE_IF_VALID(m_data -= data, Listenable&)
		{
			auto old_data = m_data;
			m_data -= data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T>
		auto operator*=(const T& data) -> ENABLE_IF_VALID(m_data *= data, Listenable&)
		{
			auto old_data = m_data;
			m_data *= data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T>
		auto operator/=(const T& data) -> ENABLE_IF_VALID(m_data /= data, Listenable&)
		{
			auto old_data = m_data;
			m_data /= data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T>
		auto operator==(const T& rhs) const -> ENABLE_IF_VALID(m_data == rhs, bool)
		{
			return m_data == rhs;
		}
		template<typename T>
		auto operator!=(const T& rhs) const->ENABLE_IF_VALID(m_data != rhs, bool)
		{
			return m_data != rhs;
		}
		template<typename T = void>
		auto operator++() -> ENABLE_IF_VALID(++LISTENABLE::identity<T>(m_data), Listenable&)
		{
			auto old_data = m_data;
			++m_data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T = void>
		auto operator--() -> ENABLE_IF_VALID(--LISTENABLE::identity<T>(m_data), Listenable&)
		{
			auto old_data = m_data;
			--m_data;
			notify(old_data, m_data);
			return *this;
		}
		template<typename T = void>
		auto operator++(int) -> ENABLE_IF_VALID(LISTENABLE::identity<T>(m_data)++, decltype(m_data))
		{
			auto old_data = m_data++;
			notify(old_data, m_data);
			return old_data;
		}
		template<typename T = void>
		auto operator--(int) -> ENABLE_IF_VALID(LISTENABLE::identity<T>(m_data)--, decltype(m_data))
		{
			auto old_data = m_data--;
			notify(old_data, m_data);
			return old_data;
		}
		template<typename T = void, typename = std::enable_if_t<std::is_void<T>::value && (std::is_pointer<VariableType>::value || is_smart_pointer<VariableType>::value)>>
		VariableType operator->() const
		{
			return m_data;
		}
		template<typename T = void>
		auto operator*() const -> ENABLE_IF_VALID(*LISTENABLE::identity<T>(m_data), decltype(*LISTENABLE::identity<T>(m_data)))
		{
			return *m_data;
		}
	};

}

#endif // !__LISTENABLE_HEADER__


#ifndef __ANY_HEADER__
#define __ANY_HEADER__

#include <algorithm>
#include <typeinfo>
#include "Core.h"
#include "Std/type_traits.h"

using namespace LORD;

namespace BLOCKMAN
{
	namespace ANY_DETAIL
	{
		struct AnyHolderBase : public ObjectAlloc 
		{
			virtual ~AnyHolderBase() = default;
			virtual AnyHolderBase* clone() = 0;
			virtual const std::type_info& type() const noexcept = 0;
		};

		template<typename T>
		struct AnyHolder : public AnyHolderBase
		{
			T data;
			AnyHolder(const T& value) : data(value) {}
			AnyHolder(T&& value) : data(std::move(value)) {}
			AnyHolder* clone() override
			{
				return LordNew AnyHolder{ data };
			}
			const std::type_info& type() const noexcept override
			{
				return typeid(T);
			}
		};
	}

	class bad_any_cast : public std::runtime_error 
	{
		using std::runtime_error::runtime_error;
	};

	class Any
	{
		template<typename T>
		friend T any_cast(const Any& anyValue);
		template<typename T>
		friend T any_cast(Any& anyValue);
		template<typename T>
		friend T any_cast(Any&& anyValue);

	private:
		ANY_DETAIL::AnyHolderBase* m_holder = nullptr;

		template<typename T>
		static void castCheck(const Any& anyValue)
		{
			if (anyValue.empty())
			{
				throw bad_any_cast("Any object is empty");
			}
			if (anyValue.type() != typeid(T))
			{
				throw bad_any_cast("Any object is cast to incorrect type");
			}
		}

	public:
		Any() = default;
		Any(const Any& value)
		{
			m_holder = value.m_holder ? value.m_holder->clone() : nullptr;
		}
		Any(Any&& value) : Any()
		{
			swap(*this, value);
		}
		template<typename T>
		Any(T& value) : m_holder(LordNew ANY_DETAIL::AnyHolder<std::remove_cv_t<T>>{value}) {}
		template<typename T>
		Any(T&& value) : m_holder(LordNew ANY_DETAIL::AnyHolder<std::remove_cv_t<T>>{std::move(value)}) {}

		Any& operator=(Any value)
		{
			swap(*this, value);
			return *this;
		}
		~Any()
		{
			LordSafeDelete(m_holder);
		}

		bool empty() const noexcept
		{
			return !m_holder;
		}

		const std::type_info& type() const
		{
			return m_holder->type();
		}

		friend void swap(Any& lhs, Any& rhs) noexcept
		{
			using std::swap;
			swap(lhs.m_holder, rhs.m_holder);
		}
		
	};

	template<typename T>
	T any_cast(const Any& anyValue)
	{
		using HoldedType = std::remove_cv_t<std::remove_reference_t<T>>;
		Any::castCheck<HoldedType>(anyValue);
		return static_cast<const ANY_DETAIL::AnyHolder<HoldedType>*>(anyValue.m_holder)->data;
	}

    template<typename T>
    T any_cast(Any& anyValue)
    {
		using HoldedType = std::remove_cv_t<std::remove_reference_t<T>>;
		Any::castCheck<HoldedType>(anyValue);
        return static_cast<ANY_DETAIL::AnyHolder<HoldedType>*>(anyValue.m_holder)->data;
    }

	template<typename T>
	T any_cast(Any&& anyValue)
	{
		return any_cast<T>(anyValue);
	}

}

#endif

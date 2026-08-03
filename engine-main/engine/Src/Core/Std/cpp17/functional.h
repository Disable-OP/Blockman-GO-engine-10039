#ifndef __CPP17_FUNCTIONAL_HEADER__
#define __CPP17_FUNCTIONAL_HEADER__
#if __cplusplus < 201703L

#include "Std/type_traits.h"

namespace std
{
	namespace detail {
		template <class T>
		struct is_reference_wrapper : false_type {};
		template <class U>
		struct is_reference_wrapper<reference_wrapper<U>> : true_type {};

		template <class Base, class T, class Derived, class... Args>
		auto MY_INVOKE(T Base::*pmf, Derived&& ref, Args&&... args)
			-> typename enable_if<is_function<T>::value &&
			is_base_of<Base, typename decay<Derived>::type>::value,
			decltype((forward<Derived>(ref).*pmf)(forward<Args>(args)...))>::type
		{
			return (forward<Derived>(ref).*pmf)(forward<Args>(args)...);
		}

		template <class Base, class T, class RefWrap, class... Args>
		auto MY_INVOKE(T Base::*pmf, RefWrap&& ref, Args&&... args)
			-> typename enable_if<is_function<T>::value &&
			is_reference_wrapper<typename decay<RefWrap>::type>::value,
			decltype((ref.get().*pmf)(forward<Args>(args)...))>::type
		{
			return (ref.get().*pmf)(forward<Args>(args)...);
		}

		template <class Base, class T, class Pointer, class... Args>
		auto MY_INVOKE(T Base::*pmf, Pointer&& ptr, Args&&... args)
			-> typename enable_if<is_function<T>::value &&
			!is_reference_wrapper<typename decay<Pointer>::type>::value &&
			!is_base_of<Base, typename decay<Pointer>::type>::value,
			decltype(((*forward<Pointer>(ptr)).*pmf)(forward<Args>(args)...))>::type
		{
			return ((*forward<Pointer>(ptr)).*pmf)(forward<Args>(args)...);
		}

		template <class Base, class T, class Derived>
		auto MY_INVOKE(T Base::*pmd, Derived&& ref)
			-> typename enable_if<!is_function<T>::value &&
			is_base_of<Base, typename decay<Derived>::type>::value,
			decltype(forward<Derived>(ref).*pmd)>::type
		{
			return forward<Derived>(ref).*pmd;
		}

		template <class Base, class T, class RefWrap>
		auto MY_INVOKE(T Base::*pmd, RefWrap&& ref)
			-> typename enable_if<!is_function<T>::value &&
			is_reference_wrapper<typename decay<RefWrap>::type>::value,
			decltype(ref.get().*pmd)>::type
		{
			return ref.get().*pmd;
		}

		template <class Base, class T, class Pointer>
		auto MY_INVOKE(T Base::*pmd, Pointer&& ptr)
			-> typename enable_if<!is_function<T>::value &&
			!is_reference_wrapper<typename decay<Pointer>::type>::value &&
			!is_base_of<Base, typename decay<Pointer>::type>::value,
			decltype((*forward<Pointer>(ptr)).*pmd)>::type
		{
			return (*forward<Pointer>(ptr)).*pmd;
		}

		template <class F, class... Args>
		auto MY_INVOKE(F&& f, Args&&... args)
			-> typename enable_if<!is_member_pointer<typename decay<F>::type>::value,
			decltype(forward<F>(f)(forward<Args>(args)...))>::type
		{
			return forward<F>(f)(forward<Args>(args)...);
		}
	} // namespace detail

	// Conforming C++14 implementation (is also a valid C++11 implementation):
	namespace detail {
		template <typename AlwaysVoid, typename, typename...>
		struct invoke_result { };
		template <typename F, typename...Args>
		struct invoke_result<decltype(void(detail::MY_INVOKE(declval<F>(), declval<Args>()...))),
			F, Args...> {
			using type = decltype(detail::MY_INVOKE(declval<F>(), declval<Args>()...));
		};
	} // namespace detail

	template <class F, class... ArgTypes>
	struct invoke_result : detail::invoke_result<void, F, ArgTypes...> {};

	template< class F, class... ArgTypes>
	using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;

	template< class F, class... Args>
	invoke_result_t<F, Args...> invoke(F&& f, Args&&... args)
	{
		return detail::MY_INVOKE(forward<F>(f), forward<Args>(args)...);
	}
}

#endif
#endif

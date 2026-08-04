#ifndef __CPP17_TUPLE_HEADER__
#define __CPP17_TUPLE_HEADER__
#if __cplusplus < 201703L

#include "Std/type_traits.h"
#include "Std/utility.h"

namespace std
{
	namespace detail {
		template <class F, class Tuple, size_t... I>
		constexpr auto apply_impl(F&& f, Tuple&& t, index_sequence<I...>) 
			-> decltype(invoke(forward<F>(f), get<I>(forward<Tuple>(t))...))
		{
			return invoke(forward<F>(f), get<I>(forward<Tuple>(t))...);
		}
	}  // namespace detail

	template <class F, class Tuple>
	constexpr auto apply(F&& f, Tuple&& t) 
		-> decltype(detail::apply_impl(forward<F>(f), forward<Tuple>(t),
		make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{}))
	{
		return detail::apply_impl(
			forward<F>(f), forward<Tuple>(t),
			make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
	}
	
	namespace detail {
		template <class T, class Tuple, size_t... I>
		constexpr T make_from_tuple_impl(Tuple&& t, index_sequence<I...>)
		{
			return T(get<I>(forward<Tuple>(t))...);
		}
	} // namespace detail

	template <class T, class Tuple>
	constexpr T make_from_tuple(Tuple&& t)
	{
		return detail::make_from_tuple_impl<T>(forward<Tuple>(t),
			make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{});
	}
}

#endif
#endif

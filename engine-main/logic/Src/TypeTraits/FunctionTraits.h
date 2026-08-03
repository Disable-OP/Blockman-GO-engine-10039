#ifndef _function_traits_impl_HEADER__
#define _function_traits_impl_HEADER__

#include <functional>
#include <tuple>
#include "Std/type_traits.h"

namespace BLOCKMAN
{
	template<typename Return, typename... Args>
	struct function_traits_impl
	{
		using return_type = Return;
		using argument_types = std::tuple<Args...>;
		template<size_t index>
		using argument_type = typename std::tuple_element<index, argument_types>::type;
		static constexpr size_t argument_size = sizeof...(Args);
	};

	template<typename Function>
	struct function_traits : function_traits<decltype(&Function::operator())> {};

	template<typename Return, typename... Args>
	struct function_traits<std::function<Return(Args...)>> : function_traits_impl<Return, Args...> {};

	template<typename Return, typename... Args>
	struct function_traits<Return(Args...)> : function_traits_impl<Return, Args...> {};

	template<typename Return, typename... Args>
	struct function_traits<Return(*)(Args...)> : function_traits_impl<Return, Args...> {};

	template<typename Class, typename Return, typename... Args>
	struct function_traits<Return(Class::*)(Args...)> : function_traits_impl<Return, Args...> {};

	template<typename Class, typename Return, typename... Args>
	struct function_traits<Return(Class::*)(Args...) const> : function_traits_impl<Return, Args...> {};

	template<typename Function>
	struct function_traits<const Function> : function_traits<Function> {};
	template<typename Function>
	struct function_traits<volatile Function> : function_traits<Function> {};
	template<typename Function>
	struct function_traits<const volatile Function> : function_traits<Function> {};

}

#endif

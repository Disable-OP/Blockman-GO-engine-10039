#ifndef __CPP14_TYPE_TRAITS_HEADER__
#define __CPP14_TYPE_TRAITS_HEADER__
#if __cplusplus < 201402L

namespace std
{
	template< class T >
	using underlying_type_t = typename underlying_type<T>::type;

	template< class T >
	using remove_reference_t = typename remove_reference<T>::type;

	template< class T >
	using remove_pointer_t = typename remove_pointer<T>::type;

	template< bool B, class T = void >
	using enable_if_t = typename enable_if<B, T>::type;

	template< bool B, class T, class F >
	using conditional_t = typename conditional<B, T, F>::type;

	template< class T >
	using remove_cv_t = typename remove_cv<T>::type;

	template< class T >
	using remove_const_t = typename remove_const<T>::type;

	template< class T >
	using remove_volatile_t = typename remove_volatile<T>::type;
}

#endif
#endif

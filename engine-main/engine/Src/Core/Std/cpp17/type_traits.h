#ifndef __CPP17_TYPE_TRAITS_HEADER__
#define __CPP17_TYPE_TRAITS_HEADER__
#if __cplusplus < 201703L

namespace std
{
	template<class...> struct conjunction : true_type { };
	template<class B1> struct conjunction<B1> : B1 { };
	template<class B1, class... Bn>
	struct conjunction<B1, Bn...>
		: conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};
}

#endif
#endif

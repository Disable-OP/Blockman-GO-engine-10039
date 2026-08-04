#ifndef __CONTAINER_TRAITS_HEADER__
#define __CONTAINER_TRAITS_HEADER__

#include <array>

namespace BLOCKMAN
{
	template<typename Containter>
	struct container_traits
	{
		using size_type = typename Containter::size_type;
		using value_type = typename Containter::value_type;
	};

	template<typename T, size_t size>
	struct container_traits<std::array<T, size>>
	{
		using size_type = size_t;
		using value_type = T;
	};

	template<typename T, size_t size>
	struct container_traits<T[size]>
	{
		using size_type = size_t;
		using value_type = T;
	};
}

#endif

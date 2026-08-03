#ifndef __CPP14_ITERATOR_HEADER__
#define __CPP14_ITERATOR_HEADER__
#if __cplusplus < 201402L

namespace std
{
	template< class C >
	constexpr auto cbegin(const C& c) noexcept(noexcept(std::begin(c))) -> decltype(std::begin(c))
	{
		return begin(c);
	}

	template< class C >
	constexpr auto cend(const C& c) noexcept(noexcept(std::end(c))) -> decltype(std::end(c))
	{
		return end(c);
	}
}

#endif
#endif

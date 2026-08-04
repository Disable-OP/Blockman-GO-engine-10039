#ifndef __CPP17_ITERATOR_HEADER__
#define __CPP17_ITERATOR_HEADER__
#if __cplusplus < 201703L

namespace std
{
	template <class C>
	constexpr auto size(const C& c) -> decltype(c.size())
	{
		return c.size();
	}
	template <class T, std::size_t N>
	constexpr std::size_t size(const T(&array)[N]) noexcept
	{
		return N;
	}

	template <class C>
	constexpr auto data(C& c) -> decltype(c.data())
	{
		return c.data();
	}
	template <class C>
	constexpr auto data(const C& c) -> decltype(c.data())
	{
		return c.data();
	}
	template <class T, std::size_t N>
	constexpr T* data(T(&array)[N]) noexcept
	{
		return array;
	}
	template <class E>
	constexpr const E* data(std::initializer_list<E> il) noexcept
	{
		return il.begin();
	}
}

#endif
#endif

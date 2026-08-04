#ifndef __CONTAINER_UTILS_HEADER__
#define __CONTAINER_UTILS_HEADER__

#include <stdexcept>
#include <Std/iterator.h>
#include "TypeTraits/ContainerTraits.h"

namespace BLOCKMAN
{
	using std::data;
	using std::size;

	using std::size_t;
	
	template<typename T>
	inline constexpr size_t max_size(const T &container)
	{
		return container.max_size();
	}

	template<typename T, size_t size>
	inline constexpr size_t max_size(T(&arr)[size])
	{
		return size;
	}

	template<typename ListType>
	inline auto element_at(ListType &container, const typename container_traits<ListType>::size_type index) -> decltype(container[index])
	{
		if (container.size() <= index)
		{
			container.resize(index + 1);
		}
		return container[index];
	}

	template<typename T, size_t size>
	inline T &element_at(std::array<T, size> &container, const size_t index)
	{
		return container.at(index);
	}

	template<typename T, size_t size>
	inline T &element_at(T(&container)[size], const size_t index)
	{
		if (size <= index)
		{
			throw std::out_of_range("accessing index is larger than the size of the native array object");
		}
		return container[index];
	}
}

#endif

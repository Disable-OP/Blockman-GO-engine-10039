#ifndef __CPP17_STRING_VIEW_HEADER__
#define __CPP17_STRING_VIEW_HEADER__
#if __cplusplus < 201703L

#include <stdexcept>
#include <algorithm>
#include "Std/iterator.h"

namespace std
{
	template<class CharT, class Traits = std::char_traits<CharT>> 
	class basic_string_view
	{
	public:
		using traits_type = Traits;
		using value_type = CharT;
		using pointer = const CharT*;
		using const_pointer = const CharT*;
		using reference = const CharT&;
		using const_reference = const CharT&;
		using const_iterator = const CharT*;
		using iterator = const_iterator;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using reverse_iterator = const_reverse_iterator;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		static constexpr size_type npos = size_type(-1);

		basic_string_view() noexcept : data(nullptr), len(0) {}
		basic_string_view(const basic_string_view& other) noexcept = default;
		basic_string_view(const CharT* s, size_type count) noexcept : data(s), len(count) {}
		basic_string_view(const CharT* s) : data(s), len(Traits::length(s)) {}
		// TODO: this function should be constexpr according to c++17
		basic_string_view& operator=(const basic_string_view& view) noexcept = default;

		constexpr const_reference operator[](size_type pos) const
		{
			return this->data[pos];
		}

		constexpr const_reference at(size_type pos) const
		{
			// TODO: fix the warning: returning reference to temporary
			return pos < this->len ? this->data[pos] : throw std::out_of_range("index out of range");
		}

		constexpr size_type size() const noexcept
		{
			return this->len;
		}

		constexpr size_type length() const noexcept
		{
			return this->len;
		}

		constexpr bool empty() const noexcept
		{
			return this->len == 0;
		}

		constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const
		{
			return pos <= this->len ? basic_string_view(this->data + pos, min(len - pos, count)) : throw std::out_of_range("index out of range");
		}

		// TODO: this function should be constexpr according to c++17
		int compare(basic_string_view v) const noexcept
		{
			int ret = traits_type::compare(this->data, v.data, min(this->len, v.len));
			if (ret == 0)
				ret = this->len - v.len;
			return ret;
		}

		int compare(size_type pos1, size_type count1, basic_string_view v) const
		{
			return substr(pos1, count1).compare(v);
		}

		constexpr int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const
		{
			return substr(pos1, count1).compare(v.substr(pos2, count2));
		}

		constexpr int compare(const CharT* s) const
		{
			return compare(basic_string_view(s));
		}

		constexpr int compare(size_type pos1, size_type count1, const CharT* s) const
		{
			return substr(pos1, count1).compare(basic_string_view(s));
		}

		constexpr int compare(size_type pos1, size_type count1, const CharT* s, size_type count2) const
		{
			return substr(pos1, count1).compare(basic_string_view(s, count2));
		}

		// TODO: this function should be constexpr according to c++17
		size_type find(basic_string_view v, size_type pos = 0) const noexcept
		{
			if (len - pos < v.len)
			{
				return npos;
			}
			auto posPtr = data + pos;
			int loopCount = len - pos - v.len + 1;
			for (int i = 0; i < loopCount; ++i)
			{
				if (basic_string_view(posPtr + i, v.len) == v)
				{
					return pos + i;
				}
			}
			return npos;
		}

		size_type find(CharT c, size_type pos = 0) const noexcept
		{
			return find(basic_string_view(&c, 1), pos);
		}

		size_type find(const CharT* s, size_type pos, size_type count) const noexcept
		{
			return find(basic_string_view(s, count), pos);
		}

		size_type find(const CharT* s, size_type pos = 0) const noexcept
		{
			return find(basic_string_view(s), pos);
		}

		// TODO: this function should be constexpr according to c++17
		size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept
		{
			for (size_type i = pos; i < len; ++i)
			{
				for (size_type j = 0; j < v.len; ++j)
				{
					if (data[i] == v[j])
					{
						return i;
					}
				}
			}
			return npos;
		}

		size_type find_first_of(CharT c, size_type pos = 0) const noexcept
		{
			return find_first_of(basic_string_view(&c, 1), pos);
		}

		size_type find_first_of(const CharT* s, size_type pos, size_type count) const
		{
			return find_first_of(basic_string_view(s, count), pos);
		}

		size_type find_first_of(const CharT* s, size_type pos = 0) const
		{
			return find_first_of(basic_string_view(s), pos);
		}

	private:
		pointer data;
		size_type len;
	};

#define DEFINE_COMPARE_OPERATOR(_op)                                           \
template< class CharT, class Traits >                                          \
constexpr bool operator _op(basic_string_view <CharT, Traits> lhs,             \
	basic_string_view <CharT, Traits> rhs) noexcept                            \
{                                                                              \
	return lhs.compare(rhs) _op 0;                                             \
}

	DEFINE_COMPARE_OPERATOR(==);
	DEFINE_COMPARE_OPERATOR(!=);
	DEFINE_COMPARE_OPERATOR(<);
	DEFINE_COMPARE_OPERATOR(<=);
	DEFINE_COMPARE_OPERATOR(>);
	DEFINE_COMPARE_OPERATOR(>=);

#undef DEFINE_COMPARE_OPERATOR

	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
}

#endif
#endif

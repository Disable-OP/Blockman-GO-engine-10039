#ifndef __IDENTITY_OP_HEADER__ 
#define __IDENTITY_OP_HEADER__

namespace BLOCKMAN
{
	struct identity_op_t
	{
		template<typename ..., typename T>
		inline constexpr auto operator()(T&& value) const noexcept -> decltype(std::forward<T>(value))
		{
			return std::forward<T>(value);
		}
	};
	constexpr identity_op_t identity_op = {};
}

#endif

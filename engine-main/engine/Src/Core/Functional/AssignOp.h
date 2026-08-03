#ifndef __ASSIGN_OP_HEADER__
#define __ASSIGN_OP_HEADER__

namespace BLOCKMAN
{
	struct assign_op_t
	{
		template<typename ..., typename T, typename V>
		inline void operator()(T&& variable, V&& value) const
		{
			std::forward<T>(variable) = std::forward<V>(value);
		}
	};
	constexpr assign_op_t assign_op = {};
}

#endif

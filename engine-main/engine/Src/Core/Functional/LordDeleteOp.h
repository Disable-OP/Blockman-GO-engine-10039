#ifndef __LORD_DELETE_OP_HEADER__
#define __LORD_DELETE_OP_HEADER__

#include "Core.h"

namespace LORD
{
	struct lord_delete_op_t
	{
		template<typename T>
		void operator()(T* ptr)
		{
			LordDelete ptr;
		}
	};

	struct lord_safe_delete_op_t
	{
		template<typename T>
		void operator()(T* ptr)
		{
			LordDelete ptr;
		}
	};

	constexpr lord_delete_op_t lord_delete_op = {};
	constexpr lord_safe_delete_op_t lord_safe_delete_op = {};
}

#endif // !__LORD_DELETE_OP_HEADER__


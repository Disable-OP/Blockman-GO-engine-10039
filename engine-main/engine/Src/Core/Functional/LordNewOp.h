#ifndef __LORD_NEW_OP_HEADER__
#define __LORD_NEW_OP_HEADER__

#include <utility>
#include "Core.h"

namespace LORD
{
	template<typename Class>
	struct lord_new_op
	{
		template<typename... Args>
		Class* operator()(Args&&... args) const
		{
			return LordNew Class(std::forward<Args>(args)...);
		}
	};
}

#endif // !__LORD_NEW_OP_HEADER__


#pragma once
#include "LuaRegister/Template/Util/Traits.h"
template<typename T>
struct ClassMeta
{
	// global name
	static const char* getName(const char* name = NULL)
	{
		static char temp[256] = "";
		if (name != NULL)
			strcpy(temp, name);
		return temp;
	}

	typedef typename std::remove_const<typename PureType<T>::type>::type type;
};

#pragma once
#include "LuaRegister/Template/Public.h"
#include <vector>
template<typename T>
struct IsContainer { static constexpr bool value = false; };

template<typename T>
struct IsContainer<std::vector<T>> { static constexpr bool value = true; };

struct ParamTranslate;

/*
{
	template<typename T>
	 T read(lua_State *L, int index);

	template<typename T>
	void write(lua_State *L, T t);
};
*/


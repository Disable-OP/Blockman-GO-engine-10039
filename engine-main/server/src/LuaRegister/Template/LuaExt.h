#pragma once
#include "LuaRegister/Template/Public.h"

class LuaExt
{
public:
	static void init(lua_State *L);

private:
	static void init_sign64(lua_State *L);
	static void init_unsign64(lua_State *L);
};
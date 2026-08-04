#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/ClassMeta.h"
#include "LuaRegister/Template/Util/Traits.h"
#include "LuaRegister/Template/FuncProxy/NativeFuncProxy.h"

struct GlobleFunctionRegister
{
	template<typename F>
	static void registerFunc(lua_State*L, const char *name, F func)
	{
		// 传入函数指针
		lua_pushlightuserdata(L, (void*)func);
		pushGlobleProxy(L, func);

		// 设置名字
		lua_setglobal(L, name);
	}
};
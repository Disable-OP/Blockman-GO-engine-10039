#pragma once
#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/ClassMeta.h"
#include "LuaRegister/Template/Util/Traits.h"
#include "LuaRegister/Template/FuncProxy/NativeFuncProxy.h"
#include "LuaRegister/Template/ParamTranslate/ParamTranslate.h"

struct ObjRegister
{
	template<typename T>
	static void registerObj(lua_State* L, const char* name, T object)
	{
		ParamTranslate::write(L, object);
		lua_setglobal(L, name);
	}
};
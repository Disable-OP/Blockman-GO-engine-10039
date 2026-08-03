#pragma once

#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/ParamTranslate/ParamTranslate.h"

// call
// C++调用lua函数
// 传入参数不能用引用!!!!!!!!

namespace LUA_CALL
{
	static inline int _pre_call(lua_State* L, int tableIndex, const char* funcName)
	{
		int top = lua_gettop(L);

		lua_rawgeti(L, LUA_REGISTRYINDEX, -2);	// traceback
		int errfunc = lua_gettop(L);

		if (tableIndex == 0)
		{
			lua_getglobal(L, funcName);
		}
		else
		{
			if (tableIndex < 0)
				tableIndex += top + 1;
			lua_getfield(L, tableIndex, funcName);
		}

		return top;
	}

	static inline bool call(lua_State* L, int tableIndex, const char* funcName)
	{
		int top = _pre_call(L, tableIndex, funcName);
		bool rtn = false;
		if (lua_pcall(L, 0, 0, top + 1))
			LuaDebug::printError(L, "%s", lua_tostring(L, -1));
		else
			rtn = true;

		lua_settop(L, top);
		return rtn;
	}

	template<typename ...Args>
	inline bool call(lua_State* L, int tableIndex, const char* funcName, Args&&... args)
	{
		
		int top = _pre_call(L, tableIndex, funcName);
		bool rtn = false;
		write_n(L, std::forward<Args>(args)...);
		
		if (lua_pcall(L, sizeof...(Args), 0, top + 1))
			LuaDebug::printError(L, "%s", lua_tostring(L, -1));
		else
			rtn = true;
		lua_settop(L, top);
		return rtn;
	}

	template<typename RVal>
	inline bool callRet(lua_State* L, int tableIndex,  const char* funcName, RVal &rval)
	{
		int top = _pre_call(L, tableIndex, funcName);
		bool rtn = false;

		if (lua_pcall(L, 0, 1, top + 1))
		{
			LuaDebug::printError(L, "%s", lua_tostring(L, -1));
		}
		else
		{
			rval = ParamTranslate::pop<RVal>(L);
			rtn = true;
		}

		lua_settop(L, top);
		return rtn;
	}

	template<typename RVal, typename ...Args>
	inline bool callRet(lua_State* L, int tableIndex, const char* funcName, RVal&rval,  Args&&... args)
	{

		int top = _pre_call(L, tableIndex, funcName);
		bool rtn = false;

		write_n(L, std::forward<Args>(args)...);


		if (lua_pcall(L, sizeof...(Args), 1, top + 1))
		{
			LuaDebug::printError(L, "%s", lua_tostring(L, -1));
		}
		else
		{
			rval = ParamTranslate::pop<RVal>(L);
			rtn = true;
		}

		lua_settop(L, top);
		return rtn;
	}

	template<typename T>
	inline bool setField(lua_State *L, int tableIndex, const char *fieldName, T t)
	{
		int top = lua_gettop(L);

		if (fieldName == NULL)
			return false;

		if (tableIndex == 0)
		{
			lua_getglobal(L, fieldName);
		}
		else
		{
			if (tableIndex < 0)
				tableIndex += top + 1;
			ParamTranslate::write<T>(L, t);
			lua_setfield(L, tableIndex, fieldName);
		}

		lua_settop(L, top);

		return true;
	}

	template<typename T>
	static inline bool getField(lua_State *L, int tableIndex, const char *fieldName, T&t)
	{
		int top = lua_gettop(L);
		bool rtn = false;

		if (fieldName == NULL)
			return false;

		if (tableIndex == 0)
		{
			lua_getglobal(L, fieldName);
		}
		else
		{
			lua_getfield(L, tableIndex, fieldName);

		}

		if (!lua_isnil(L, -1))
		{
			auto ret = ParamTranslate::read<T>(L, -1);
			t = ret;
			rtn = true;
		}

		lua_settop(L, top);
		
		return rtn;
	}

	template<typename T>
	static inline bool seti(lua_State *L, int tableIndex, int i, T t)
	{
		int top = lua_gettop(L);

		if (tableIndex < 0)
			tableIndex += top + 1;
		ParamTranslate::write<T>(L, t);
		lua_rawseti(L, tableIndex, i);

		lua_settop(L, top);

		return true;
	}

	template<typename T>
	static inline bool geti(lua_State *L, int tableIndex, int i, T&t)
	{
		int top = lua_gettop(L);
		bool rtn = false;

		lua_rawgeti(L, tableIndex, i);

		if (!lua_isnil(L, -1))
		{
			auto ret = ParamTranslate::read<T>(L, -1);
			t = ret;
			rtn = true;
		}

		lua_settop(L, top);

		return rtn;
	}
}
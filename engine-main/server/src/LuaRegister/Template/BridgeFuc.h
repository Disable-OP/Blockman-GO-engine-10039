#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/VarProxy/VarProxy.h"
#include "LuaRegister/Template/ParamTranslate/UserDataTranslate.h"
#include "LuaRegister/Template/Util/LuaDebug.h"
inline bool invoke_parent(lua_State *L)
{ 
	// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 
	const char *initName = lua_tostring(L, -2);
	int i = 0;
	while (true)
	{
		char parentName[100];
		memset(parentName, 0, 100);

		sprintf(parentName, "__parent%d", i);

		lua_pushstring(L, parentName);
		lua_rawget(L, -2);// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4,string

		// stack -1 father table name 
		if (lua_isstring(L, -1))
		{
			const char* tableName = lua_tostring(L, -1);
			lua_remove(L, -1);
			lua_getglobal(L, tableName);// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4,father table
			if (lua_istable(L, -1))
			{
				lua_pushvalue(L, 2);// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4,father table 5 变量(string)
				lua_rawget(L, -2);// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4, member
				if (!lua_isnil(L, -1))
				{              
					return true;// 找到 stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4, member userdata
				}
				else
				{
					lua_remove(L, -1);// stack: 1.类(userdata) 2.变量(string) 3.meta(table)
					if (invoke_parent(L))
					{
						// 找到 stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4, member userdata
						return true;
					}
				}
			}
			else
			{
				lua_remove(L, -1);//stack: 1.类(userdata) 2.变量(string) 3.meta(table)
			}
		}
		else
		{
			if (lua_isnil(L, -1))
			{
				lua_remove(L, -1);//stack: 1.类(userdata) 2.变量(string) 3.meta(table)
				return false;
			}
			else
			{
				lua_remove(L, -1);
			}
		}

		i = i+1; // 下一个parent列
	}
}

inline int metaGet(lua_State *L)
{
	//LuaDebug::enumStack(L);

	// 传入表 和 索引参数
	// stack: 1.类(userdata) 2.变量(string) 
	lua_getmetatable(L, 1);
	// stack: 1.类(userdata) 2.变量(string) 3.meta(table)
	lua_pushvalue(L, 2);
	// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.变量(string)
	lua_rawget(L, -2);
	// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.meta[变量]value值(userdata)

	// 如果存在userdata 存在该变量
	if (lua_isuserdata(L, -1))
	{
		UserDataTranslate::readFromPureUserData<var_base*>(L, -1)->get(L);

		// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.meta[变量]value值(userdata) 5.实际值
		lua_remove(L, -2);
		// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.实际值
	}
	else if (lua_isnil(L, -1))
	{
		// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.nil
		lua_remove(L, -1);
		// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 
		bool find = invoke_parent(L);

		if (lua_isuserdata(L, -1))
		{
			// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.父类中的变量(userdata) 
			UserDataTranslate::readFromPureUserData<var_base*>(L, -1)->get(L);
			// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.父类中的变量(userdata) 5.实际值
			lua_remove(L, -2);
			// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.实际值
		}
		else if(!find)
		{
			// stack: 1.类(userdata) 2.变量(string) 3.meta(table) 4.nil
			LuaDebug::enumStack(L);
			lua_pushfstring(L, "can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, 2));
			lua_error(L);
		}
	}

	lua_remove(L, -2);
	// stack: 1.类(userdata) 2.变量(string) 3.实际值

	return 1;
}

/*---------------------------------------------------------------------------*/
inline int metaSet(lua_State *L)
{
	// stack: 1.类(userdata) 2.变量(string) 3.要赋的值
	lua_getmetatable(L, 1);
	// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table)
	lua_pushvalue(L, 2);
	// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string)
	lua_rawget(L, -2);
	// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.meta[变量](userdata mem_var指针)

	if (lua_isuserdata(L, -1))
	{
		UserDataTranslate::readFromPureUserData<var_base*>(L, -1)->set(L);
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.meta[变量](userdata mem_var指针)
	}
	else if (lua_isnil(L, -1))
	{
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.nil
		lua_remove(L, -1);
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table)
		lua_pushvalue(L, 2);
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string)
		lua_pushvalue(L, 4);
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table)
		invoke_parent(L);
		// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table) 7.获取到父类的变量(userdata mem_var指针)
		if (lua_isuserdata(L, -1))
		{
			UserDataTranslate::readFromPureUserData<var_base*>(L, -1)->set(L);
		}
		else if (lua_isnil(L, -1))
		{
			// stack: 1.类(userdata) 2.变量(string) 3.要赋的值 4.类meta(table) 5.变量(string) 6.类meta(table) 7.nil
			lua_pushfstring(L, "can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, 2));
			lua_error(L);
		}
	}
	lua_settop(L, 3);
	// stack: 1.类(userdata) 2.变量(string) 3.要赋的值
	return 0;
}


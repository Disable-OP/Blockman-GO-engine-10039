#pragma once
#include "LuaRegister/Template/Public.h"
#include "Core.h"
#include "Log/Log.h"

using namespace LORD;
struct LuaDebug
{
	static void   enumStack(lua_State *L);
	static int    onError(lua_State *L);
	static void   printError(lua_State *L, const char* fmt, ...);
	static void   callStack(lua_State* L, int n);
};

inline int LuaDebug::onError(lua_State *L)
{
	printError(L, "%s", lua_tostring(L, -1));

	(L, 0);

	return 0;
}

inline void LuaDebug::printError(lua_State *L, const char* fmt, ...)
{
	char text[4096];

	va_list args;
	va_start(args, fmt);
	vsnprintf(text, sizeof(text), fmt, args);
	va_end(args);

	lua_getglobal(L, "_ALERT");
	if (lua_isfunction(L, -1))
	{
		lua_pushstring(L, text);
		lua_call(L, 1, 0);
	}
	else
	{
		//printf("%s\n", text);
		LordLogInfo("SCRIPT_EXCEPTION----------------%s\n", text);
		lua_pop(L, 1);
	}
}

inline void LuaDebug::callStack(lua_State* L, int n)
{
	lua_Debug ar;
	if (lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		const char* indent;
		if (n == 0)
		{
			indent = "->\t";
			printError(L, "\t<call stack>");
		}
		else
		{
			indent = "\t";
		}

		if (ar.name)
			printError(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			printError(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

		callStack(L, n + 1);
	}
}

inline void LuaDebug::enumStack(lua_State *L)
{
	//callStack(L, 0);
	int top = lua_gettop(L);
	printError(L, "%s", "----------Data stack  Begin ----------");
	printError(L, "Type:%d", top);
	for (int i = 1; i <= lua_gettop(L); ++i)
	{
		switch (lua_type(L, i))
		{
		case LUA_TNIL:
			printError(L, "\t%s", lua_typename(L, lua_type(L, i)));
			break;
		case LUA_TBOOLEAN:
			printError(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			printError(L, "\t%s    0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TNUMBER:
			printError(L, "\t%s    %f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printError(L, "\t%s    %s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
		{
			lua_getfield(L, i, "__name");
			const char * tableName = lua_tostring(L, -1);
			lua_remove(L, -1);
			printError(L, "\t%s name %s   0x%08p ", lua_typename(L, lua_type(L, i)), tableName, lua_topointer(L, i));
		}
			break;
		case LUA_TFUNCTION:
			printError(L, "\t%s()  0x%08p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TUSERDATA:
		{
			const char * tableName = "no name";
			if (lua_getmetatable(L, i))
			{
				lua_getfield(L, -1, "__name");
				tableName = lua_tostring(L, -1);
				lua_remove(L, -1);
				lua_remove(L, -1);
			}
			printError(L, "\t%s    0x%08p tableName %s", lua_typename(L, lua_type(L, i)), lua_topointer(L, i), tableName);
		}
			break;
		case LUA_TTHREAD:
			printError(L, "\t%s", lua_typename(L, lua_type(L, i)));
			break;
		}
	}
	printError(L, "%s", "-----------Data stack  end --------------");


	printError(L, "%s", "----------call stack  Begin ---------------");
	callStack(L, 0);

	printError(L, "%s", "----------call stack  End ---------------");
}

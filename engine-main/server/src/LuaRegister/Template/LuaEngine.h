#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/LuaExt.h"
#include "LuaRegister/Template/Util/LuaDebug.h"
#include "LuaRegister/3rd/lbitop.h"
class LuaEngine
{
public:
	static LuaEngine *getInstance()
	{
		static LuaEngine obj;
		return &obj;
	}

	lua_State * getLuaState() { return m_L; }

	void Load(const char *path);

	void addScriptDir(const char *path);

	bool isLoadSuc() { return m_loadSuc; }
private:
	LuaEngine() { init(); };

	void init();

	static int l_print(lua_State* L);
	static int l_traceback(lua_State* L);

private:
	lua_State *m_L;
	bool   m_loadSuc = false;
};

inline void LuaEngine::init()
{
	m_L = luaL_newstate();
	luaL_openlibs(m_L);
	luaopen_bitop(m_L);

	LuaExt::init(m_L);
	lua_getglobal(m_L, "tostring");
	lua_rawseti(m_L, LUA_REGISTRYINDEX, -1);
	lua_pushcclosure(m_L, l_traceback, 0);
	lua_pushvalue(m_L, -1);
	lua_rawseti(m_L, LUA_REGISTRYINDEX, -2);
	lua_setglobal(m_L, "traceback");
	lua_pushcclosure(m_L, l_print, 0);
	lua_setglobal(m_L, "print");

}

inline void LuaEngine::addScriptDir(const char *path)
{
	lua_getglobal(m_L, "package");       
	lua_getfield(m_L, -1, "path");     
	const char* cur_path = lua_tostring(m_L, -1);
	lua_pushfstring(m_L, "%s;%s/?.lua", cur_path, path);          
	lua_setfield(m_L, -3, "path"); 
	lua_pop(m_L, 2);
}
inline int LuaEngine::l_print(lua_State* L)
{
	int n = lua_gettop(L);
	String str = "";
	for (int i = 1; i <= n; i++)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, -1);	// tostring
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		const char * s = lua_tostring(L, -1);
		if (s)
			str += s;
		else
			str += luaL_typename(L, i);
		if (i < n)
			str += '\t';
	}
	LordLogInfo("[Lua] %s", str.c_str());
	return 0;
}

inline int LuaEngine::l_traceback(lua_State* L)
{
	int n = lua_gettop(L);
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_insert(L, 1);
	lua_pop(L, 1);
	lua_call(L, n, 1);
#ifdef _WINDOWS
	if (lua_type(L, 1) == LUA_TSTRING)
	{
		lua_getfield(L, 1, "gsub");
		lua_insert(L, 1);
		lua_pushstring(L, "(lua/[^ ]+%.lua):([0-9]+):");
		lua_pushstring(L, "../res/%1(%2):");
		lua_call(L, 3, 1);
	}
#endif
	return 1;
}
inline void LuaEngine::Load(const char *path)
{
	lua_pushcclosure(m_L, LuaDebug::onError, 0);
	int errfunc = lua_gettop(m_L);

	if (luaL_loadfile(m_L, path) == 0)
	{
		 lua_pcall(m_L, 0, 1, errfunc);
		 m_loadSuc = true;
	}
	else
	{
		LuaDebug::printError(m_L, "%s", lua_tostring(m_L, -1));
	}

	lua_remove(m_L, errfunc);
	lua_pop(m_L, 1);
}


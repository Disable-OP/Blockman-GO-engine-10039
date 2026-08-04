#pragma once
#include "LuaRegister/Template/Public.h"
class LuaRefDeleter 
{
private:
	lua_State *_state;

public:
	LuaRefDeleter(lua_State *state)
		: _state{ state } {}

	void operator()(int *ref) const 
	{
		luaL_unref(_state, LUA_REGISTRYINDEX, *ref);
		delete ref;
	}
};

class LuaRef
{
private:
	std::shared_ptr<int> _ref;

public:
	LuaRef(lua_State *state, int ref)
		: _ref(new int{ ref }, LuaRefDeleter{ state }) {}

	LuaRef(lua_State *state)
		: LuaRef(state, LUA_REFNIL)
	{}

	// 从 lua 注册表中 拿出 ref 放到 栈中
	void Push(lua_State *state) const
	{
		lua_rawgeti(state, LUA_REGISTRYINDEX, *_ref);
	}
};
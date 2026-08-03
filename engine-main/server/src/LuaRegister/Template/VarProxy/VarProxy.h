#pragma once

#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/ParamTranslate/ParamTranslate.h"

// member variable
struct var_base
{
	virtual ~var_base() {};
	virtual void get(lua_State *L) = 0;
	virtual void set(lua_State *L) = 0;
};

template<typename T, typename V>
struct mem_var : var_base
{
	V T::*_var;
	mem_var(V T::*val) : _var(val) {}
	void get(lua_State *L)
	{
		ParamTranslate::write<V>(L, ParamTranslate::read<T*>(L, 1)->*(_var));
	}

	void set(lua_State *L) 
	{ 
		ParamTranslate::read<T*>(L, 1)->*(_var) = ParamTranslate::read<V>(L, 3);
	}
};
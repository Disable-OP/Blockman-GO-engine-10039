#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/Traits.h"
#include "Core.h"
#include "CoreDef.h"
#include <string>

using namespace LORD;
template<typename T>
struct IsBaseType
{
	static constexpr bool value = false;
};

template<> struct IsBaseType<char *>             { static constexpr bool value = true; };
template<> struct IsBaseType<const char *>       { static constexpr bool value = true; };
template<> struct IsBaseType<char >              { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned char>      { static constexpr bool value = true; };
template<> struct IsBaseType<short>              { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned short>     { static constexpr bool value = true; };
template<> struct IsBaseType<long>               { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned long>      { static constexpr bool value = true; };
template<> struct IsBaseType<int>                { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned int>       { static constexpr bool value = true; };
template<> struct IsBaseType<float>              { static constexpr bool value = true; };
template<> struct IsBaseType<double>             { static constexpr bool value = true; };
template<> struct IsBaseType<bool>               { static constexpr bool value = true; };
template<> struct IsBaseType<void>               { static constexpr bool value = true; };
template<> struct IsBaseType<long long>          { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned long long> { static constexpr bool value = true; };
template<> struct IsBaseType<std::string>        { static constexpr bool value = true; };
template<> struct IsBaseType<String>             { static constexpr bool value = true; };


namespace BaseTypeTranslate
{
	// read
	template<typename T>
	inline
	T read(lua_State *L, int index)
	{
		return (T)lua_tonumber(L, index);
	}

	template<>
	inline
	char* read<char*>(lua_State *L, int index)
	{
		auto rt =  (char*)lua_tostring(L, index);
		if (rt == nullptr)
		{
			lua_error(L);
		}
		return rt;
	}

	template<>
	inline
		std::string read<std::string>(lua_State *L, int index)
	{
		auto rt = (char*)lua_tostring(L, index);
		if (rt == nullptr)
		{
			lua_error(L);
		}
		return rt;
	}

	template<>
	inline
		String read<String>(lua_State *L, int index)
	{
		auto rt = (char*)lua_tostring(L, index);
		if (rt == nullptr)
		{
			lua_error(L);
		}
		return rt;
	}

	template<>
	inline
	const char* read<const char*>(lua_State *L, int index)
	{
		auto rt = (char*)lua_tostring(L, index);
		if (rt == nullptr)
		{
			lua_error(L);
		}
		return rt;
	}

	template<>
	inline
	bool read<bool>(lua_State *L, int index)
	{
		if (lua_isboolean(L, index))
			return lua_toboolean(L, index) != 0;
		else
			return lua_tonumber(L, index) != 0;
	}

	template<>
	inline
	void read<void>(lua_State *L, int index)
	{
		(void)L;
		(void)index;
		return;
	}

	template<>
	inline
	long long read(lua_State *L, int index)
	{
		if (lua_isnumber(L, index))
			return (long long)lua_tonumber(L, index);
		else
			return *(long long*)lua_touserdata(L, index);
	}
	template<>
	inline
	unsigned long long read(lua_State *L, int index)
	{
		if (lua_isnumber(L, index))
			return (unsigned long long)lua_tonumber(L, index);
		else
			return *(unsigned long long*)lua_touserdata(L, index);
	}


	//write
	template<typename T>
	inline
	void write(lua_State *L, T t)
	{
		lua_pushnumber(L, t);
	}

	template<>
	inline
	void write<char*>(lua_State *L, char* ret)
	{
		lua_pushstring(L, ret);
	}

	template<>
	inline
	void write<std::string>(lua_State *L, std::string ret)
	{
		lua_pushlstring(L, ret.c_str(), ret.size());
	}

	template<>
	inline
	void write<String>(lua_State *L, String ret)
	{
		lua_pushlstring(L, ret.c_str(), ret.size());
	}


	template<>
	inline
	void write(lua_State *L, const char* ret)
	{
		lua_pushstring(L, ret);
	}

	template<>
	inline
	void write(lua_State *L, bool ret)
	{
		lua_pushboolean(L, ret);
	}


	template<>
	inline
	void write(lua_State *L, long long ret)
	{
		*(long long*)lua_newuserdata(L, sizeof(long long)) = ret;
		lua_getglobal(L, "__s64");
		lua_setmetatable(L, -2);
	}
	template<>
	inline
	void write(lua_State *L, unsigned long long ret)
	{
		*(unsigned long long*)lua_newuserdata(L, sizeof(unsigned long long)) = ret;
		lua_getglobal(L, "__u64");
		lua_setmetatable(L, -2);
	}
};
#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/Traits.h"
#include "LuaRegister/Template/Util/ClassMeta.h"
#include "LuaRegister/Template/Util/LuaDebug.h"
#include "LuaRegister/Template/ParamTranslate/UserDataProxy.h"


template<typename T>
struct PureLuaDataToCpp
{
	static T invoke(void* input) { return *(T*)input; }
};

template<typename T>
struct PureLuaDataToCpp<T*>
{
	static T* invoke(void* input) { return (T*)input; }
};

template<typename T>
struct PureLuaDataToCpp<T&>
{
	static T& invoke(void* input) { return *(T*)input; }
};

template<typename T>
struct ProxyDataToCpp
{
	static T invoke(void * input)
	{
		return PureLuaDataToCpp<T>::invoke(PureLuaDataToCpp<ProxyImpl*>::invoke(input)->m_p);
	}
};

// 从 lua 闭包中获取函数指针
template<typename T>
T upvalue_(lua_State *L)
{
	// 得到的 userdata 直接就是 函数指针
	// 获取函数指针
	return PureLuaDataToCpp<T>::invoke(lua_touserdata(L, lua_upvalueindex(1)));
}

namespace UserDataTranslate
{

	// 这里肯定是指针了。
	template<typename T>
	T readFromPureUserData(lua_State *L, int index)
	{
		return PureLuaDataToCpp<T>::invoke(lua_touserdata(L, index));
	}

	template<typename T>
	T readFromProxyUserData(lua_State *L, int index)
	{
		return ProxyDataToCpp<T>::invoke(lua_touserdata(L, index));
	}

	//对象 指针 引用 或者 值都可以
	template<typename T>
	void writeObject(lua_State *L, T t)
	{
		CPPDataToLua<T>::invoke(L, t);
	}



	template<typename T>
	typename std::enable_if<std::is_enum<T>::value, T>::type innerRead(lua_State *L, int index)
	{
		return (T)(int)lua_tonumber(L, index);
	}

	template<typename T>
	typename std::enable_if<!std::is_enum<T>::value, T>::type innerRead(lua_State *L, int index)
	{
		if (!lua_isuserdata(L, index))
		{
            LuaDebug::enumStack(L);
			lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
			lua_error(L);
		}

		return ProxyDataToCpp<T>::invoke(lua_touserdata(L, index));
	}

	template<typename T>
	typename std::enable_if<!std::is_enum<T>::value, void>::type innerWrite(lua_State *L, T t)
	{
		writeObject<T>(L, t);
		// set C++对象传入lua 设置metatable

		lua_getglobal(L, ClassMeta<typename ClassMeta<T>::type>::getName());
		lua_setmetatable(L, -2);
	}

	template<typename T>
	typename std::enable_if<std::is_enum<T>::value, void>::type innerWrite(lua_State *L, T t)
	{
		lua_pushnumber(L, (int)t);
	}

	template<typename T>
	T read(lua_State *L, int index)
	{
		return innerRead<T>(L, index);
	}

	template<typename T>
	void write(lua_State *L, T t)
	{
		return innerWrite(L, t);
	}

	
	// destroyer
	template<typename T>
	int destroyer(lua_State *L)
	{
		// 删除C++堆上分配的内存
		((ProxyImpl*)lua_touserdata(L, 1))->~ProxyImpl();
		return 0;
	}

};

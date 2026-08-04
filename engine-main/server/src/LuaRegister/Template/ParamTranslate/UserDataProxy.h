#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/MarcoBase.h"


#define  _DEF_DEC_ARG(A) T##A t##A
#define  _DEF_DEF_ARG(A) t##A

#define  _DEF_OBJPROXY_ITEM(Args)                                                                   \
        		template<BeginLoop(_DEF_TYPENAME, Args)>                                                \
				ValProxy(BeginLoop(_DEF_DEC_ARG, Args)) : ProxyImpl(new T(BeginLoop(_DEF_DEF_ARG, Args))) {};



//最终传到 lua的 类型。真是数据存储在 m_p 指向的 位置
struct ProxyImpl
{
	ProxyImpl(void* p) : m_p(p) {}
	virtual ~ProxyImpl() {}
	void* m_p;
};

// to lua
template<typename T>
struct ValProxy : ProxyImpl
{
	ValProxy() : ProxyImpl(new T) {}
	_DEF_OBJPROXY_ITEM(1)
		_DEF_OBJPROXY_ITEM(2)
		_DEF_OBJPROXY_ITEM(3)
		_DEF_OBJPROXY_ITEM(4)
		_DEF_OBJPROXY_ITEM(5)
		_DEF_OBJPROXY_ITEM(6)
		_DEF_OBJPROXY_ITEM(7)
		_DEF_OBJPROXY_ITEM(8)
		_DEF_OBJPROXY_ITEM(9)
		_DEF_OBJPROXY_ITEM(10)

		// 只有lua调用new在C++堆上分配内存才会被__gc
		~ValProxy() { delete ((T*)m_p); }
};

template<typename T>
struct PtrProxy : ProxyImpl
{
	PtrProxy(T* t) : ProxyImpl((void*)t) {}
};

template<typename T>
struct RefProxy : ProxyImpl
{
	RefProxy(T& t) : ProxyImpl(&t) {}
};

// to lua
// 值传入lua 
// 方法：proxy 分配在lua上，而T类型input分配在C++堆上，通过val2user<T>中的指针指向
template<typename T>
struct CPPDataToLua
{
	static void invoke(lua_State *L, T& input)
	{
		new(lua_newuserdata(L, sizeof(ValProxy<T>))) ValProxy<T>(input);
	}
};

// 指针传入lua 
// 方法：proxy 分配在lua上，而T指针input存在C++中，通过ptr2user<T>中的指针指向
template<typename T>
struct CPPDataToLua<T*>
{
	static void invoke(lua_State *L, T* input)
	{
		if (input)
			new(lua_newuserdata(L, sizeof(PtrProxy<T>))) PtrProxy<T>(input);
		else
			lua_pushnil(L);
	}
};

// 引用传入lua 
// 方法：proxy 分配在lua上，而T引用input存在C++中，通过var2user<T&>中的指针指向
template<typename T>
struct CPPDataToLua<T&>
{
	static void invoke(lua_State *L, T& input)
	{
		new(lua_newuserdata(L, sizeof(RefProxy<T>))) RefProxy<T>(input);
	}
};
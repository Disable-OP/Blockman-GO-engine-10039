#pragma once
#include "LuaRegister/Template/Util/Traits.h"
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/ParamTranslate/ParamTranslate.h"
#include "LuaRegister/Template/Util/MarcoBase.h"

// lua 栈索引 如果是 调用到 memproxy 那么 第一个索引 肯定是  class point
#define  _DEF_READ_VAR(A) decltype(ParamTranslate::read<T##A>(L, A+1)) var##A = ParamTranslate::read<T##A>(L, A+1)
#define  _DEF_SET_VAR(A)  var##A

#define  _DEF_REALINVOKE(Args)                                                              \
template<typename RVal, BeginLoop(_DEF_TYPENAME, Args)>                                     \
static int RealInvoke(TypeDispatch<RVal>, lua_State *L)                                     \
{                                                                                           \
    MBeginLoop(_DEF_READ_VAR, Args);                                                        \
	ParamTranslate::write(L,                                                                \
		(ParamTranslate::read<T*>(L, 1)                                                     \
			->*upvalue_<RVal(T::*)(BeginLoop(_DEF_T, Args)) >(L))                           \
			(BeginLoop(_DEF_SET_VAR, Args)));                                               \
	return 1;                                                                               \
}                                                                                           \
template<typename RVal = void,BeginLoop(_DEF_TYPENAME, Args)>                               \
static int RealInvoke(TypeDispatch<void>, lua_State *L)                                     \
{                                                                                           \
	MBeginLoop(_DEF_READ_VAR, Args);                                                        \
	(ParamTranslate::read<T*>(L, 1)                                                         \
		->*upvalue_<void(T::*)(BeginLoop(_DEF_T, Args)) >(L))                               \
		(BeginLoop(_DEF_SET_VAR, Args));                                                    \
                                                                                            \
		return 0;                                                                           \
}

template<typename CRVal, typename T, typename ...Args>
struct ClassMemProxy
{
	static int invoke(lua_State *L)
	{
		return RealInvoke<CRVal, Args...>(TypeDispatch<CRVal>{}, L);
	}

	_DEF_REALINVOKE(1)
		_DEF_REALINVOKE(2)
		_DEF_REALINVOKE(3)
		_DEF_REALINVOKE(4)
		_DEF_REALINVOKE(5)
		_DEF_REALINVOKE(6)
		_DEF_REALINVOKE(7)
		_DEF_REALINVOKE(8)
		_DEF_REALINVOKE(9)
		_DEF_REALINVOKE(10)
};

template<typename PRVal, typename T>
struct ClassMemProxy<PRVal, T>
{
	static int invoke(lua_State *L)
	{
		return RealInvoke(TypeDispatch<PRVal>{}, L);
	}

	template<typename RVal>
	static int RealInvoke(TypeDispatch<RVal>, lua_State *L)
	{
		ParamTranslate::write(L,
			(ParamTranslate::read<T*>(L, 1)->*upvalue_<RVal(T::*)()>(L))()
		);
		return 1;
	}

	static int RealInvoke(TypeDispatch<void>, lua_State *L)
	{
		(ParamTranslate::read<T*>(L, 1)->*upvalue_<void(T::*)()>(L))();
		return 0;
	}
};

template<typename RVal, typename T, typename ...Args>
void pushMemProxy(lua_State *L, RVal(T::*func)(Args...))
{
	lua_pushcclosure(L, ClassMemProxy<RVal, T, Args...>::invoke, 1);
}

template<typename RVal, typename T, typename ...Args>
void pushMemProxy(lua_State *L, RVal(T::*func)(Args...)const)
{
	lua_pushcclosure(L, ClassMemProxy<RVal, T, Args...>::invoke, 1);
}
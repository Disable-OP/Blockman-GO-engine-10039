#pragma once
#include "LuaRegister/Template/Util/Traits.h"
#include "LuaRegister/Template/Public.h"

#define  _DEF_READ_NATIVE(A) decltype(ParamTranslate::read<T##A>(L, A)) var##A = ParamTranslate::read<T##A>(L, A) 
#define  _DEF_SET_NATIVE_VAR(A) var##A

#define  _DEF_NATIVE_REALINVOKE(Args)													    \
template<typename RVal, BeginLoop(_DEF_TYPENAME, Args)>                                     \
static int RealInvoke(TypeDispatch<RVal>, lua_State *L)                                     \
{                                                                                           \
    MBeginLoop(_DEF_READ_NATIVE, Args);                                                     \
	ParamTranslate::write(L,                                                                \
		(upvalue_<RVal(*)(BeginLoop(_DEF_T, Args)) >(L))                                    \
			(BeginLoop(_DEF_SET_NATIVE_VAR, Args)));                                        \
	return 1;                                                                               \
}                                                                                           \
template<typename RVal= void, BeginLoop(_DEF_TYPENAME, Args)>                               \
static int RealInvoke(TypeDispatch<void>, lua_State *L)                                     \
{                                                                                           \
		MBeginLoop(_DEF_READ_NATIVE, Args);                                                 \
		(upvalue_<void(*)(BeginLoop(_DEF_T, Args)) >(L))                                    \
		(BeginLoop(_DEF_SET_NATIVE_VAR, Args));                                             \
                                                                                            \
		return 0;                                                                           \
}



template<typename PRVal, typename ...Args>
struct NativeFuncProxy
{
	static int invoke(lua_State *L) 
	{
		return RealInvoke<PRVal, Args...>(TypeDispatch<PRVal>{}, L);
	}

	_DEF_NATIVE_REALINVOKE(1)
	_DEF_NATIVE_REALINVOKE(2)
	_DEF_NATIVE_REALINVOKE(3)
	_DEF_NATIVE_REALINVOKE(4)
	_DEF_NATIVE_REALINVOKE(5)
	_DEF_NATIVE_REALINVOKE(6)
	_DEF_NATIVE_REALINVOKE(7)
	_DEF_NATIVE_REALINVOKE(8)
	_DEF_NATIVE_REALINVOKE(9)
	_DEF_NATIVE_REALINVOKE(10)
};

template<typename PRVal>
struct NativeFuncProxy<PRVal>
{
	static int invoke(lua_State *L)
	{
		return RealInvoke(TypeDispatch<PRVal>{}, L);
	}

	template<typename RVal>
	static int RealInvoke(TypeDispatch<RVal>, lua_State *L)
	{
		ParamTranslate::write(L, upvalue_<RVal(*)()>(L)());
		return 1;
	}

	static int RealInvoke(TypeDispatch<void>, lua_State *L)
	{
		upvalue_<void(*)()>(L)();
		return 0;
	}
};
template<typename RVal, typename ...Args>
void pushGlobleProxy(lua_State *L, RVal(*func)(Args...))
{
	lua_pushcclosure(L, NativeFuncProxy<RVal, Args...>::invoke, 1);
}

#pragma once
#include "LuaRegister/Template/Util/ClassMeta.h"
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/ParamTranslate/UserDataTranslate.h"


#define  _DEF_READ_CTOR(A)  decltype(ParamTranslate::read<T##A>(L, A)) var##A = ParamTranslate::read<T##A>(L, A)
#define  _DEF_SET_CTOR_VAR(A) var##A

#define  CONSTRUCTOR_PROXY(Args)                                                                              \
template<typename T, BeginLoop(_DEF_TYPENAME, Args)>                                                          \
int CtorProxy(lua_State *L)                                                                                   \
{                                                                                                             \
	MBeginLoop(_DEF_READ_CTOR, Args);																          \
	new(lua_newuserdata(L, sizeof(ValProxy<T>))) ValProxy<T>(BeginLoop(_DEF_SET_CTOR_VAR, Args));             \
	lua_getglobal(L, ClassMeta<typename ClassMeta<T>::type>::getName());                                      \
	lua_setmetatable(L, -2);                                                                                  \
	                                                                                                          \
	return 1;                                                                                                 \
};


CONSTRUCTOR_PROXY(1)
CONSTRUCTOR_PROXY(2)
CONSTRUCTOR_PROXY(3)
CONSTRUCTOR_PROXY(4)
CONSTRUCTOR_PROXY(5)
CONSTRUCTOR_PROXY(6)
CONSTRUCTOR_PROXY(7)
CONSTRUCTOR_PROXY(8)
CONSTRUCTOR_PROXY(9)
CONSTRUCTOR_PROXY(10)

template<typename T>
int CtorProxy(lua_State *L)
{
	new(lua_newuserdata(L, sizeof(ValProxy<T>))) ValProxy<T>();
	lua_getglobal(L, ClassMeta<typename ClassMeta<T>::type>::getName());
	lua_setmetatable(L, -2);

	return 1;
};



template<typename T, typename ...Args>
void pushCtorProxy(lua_State *L)
{
	lua_pushcclosure(L, CtorProxy<T, Args...>, 0);
}
#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/MarcoBase.h"
#include "LuaRegister/Template/FuncProxy/ClassMemProxy.h"
#include "LuaRegister/Template/FuncProxy/NativeFuncProxy.h"
#include "LuaRegister/Template/FuncProxy/CtorProxy.h"
#include "LuaRegister/Template/Util/ClassMeta.h"
#include "LuaRegister/Template/BridgeFuc.h"
#include <map>

template<typename T>
class ClassRegister
{
public:
	ClassRegister(std::string name, lua_State *L_)
		:m_name(name)
		, L(L_) 
	{
		registerClass();
	}


public:
	void registerClass();

	template<typename BC>      
	void addBaseClass(const char *name);

	template<typename ...Args> 
	void addCtor();

	template<typename F>       
	void addMember(const char *name, F func);

	template<typename F>
	void addVarMember(const char *name, F T::*var);

	template<typename Ret,typename ...Args>
	void addOverrideMember(const char *name, Ret(T::*func)(Args...));

	template<typename Ret, typename ...Args>
	void addOverrideMember(const char *name, Ret(T::*func)(Args...)const);

	template<typename F>
	void addStaticMember(const char *name, F func);

	template<typename Ret, typename ...Args>
	void addOverrideStaticMember(const char *name, Ret(*func)(Args...));

private:
	std::string m_name;
	lua_State *L;
	int m_ctorNum = 0;
	int m_baseClassNum = 0;
	std::map<std::string, int> m_registerFunc;
};

template<typename T>
inline void ClassRegister<T>::registerClass()
{
	// 通过类名设置类table
	// 如果该类没有注册，在lua中是获取不到类信息的
	ClassMeta<T>::getName(m_name.c_str());

	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, m_name.c_str());
	lua_rawset(L, -3);

	lua_pushstring(L, "__index");
	lua_pushcclosure(L, metaGet, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__newindex");
	lua_pushcclosure(L, metaSet, 0);
	lua_rawset(L, -3);


	lua_pushstring(L, "__gc");
	lua_pushcclosure(L, UserDataTranslate::destroyer<T>, 0);
	lua_rawset(L, -3);

	lua_setglobal(L, m_name.c_str());
}

template<typename T>
template<typename BC>
inline void ClassRegister<T>::addBaseClass(const char *name)
{
	// 获取类table
	lua_getglobal(L, ClassMeta<T>::getName());

	char parentName[100];
	memset(parentName, 0, 100);

	sprintf(parentName, "__parent%d", m_baseClassNum);

	if (lua_istable(L, -1))
	{
		// 压入父类名字
		lua_pushstring(L, parentName);
		//lua_getglobal(L, ClassMeta<BC>::getName());
		lua_pushstring(L, ClassMeta<BC>::getName(name));
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);

	m_baseClassNum++;
}

template<typename T>
template<typename ...Args> 
inline void ClassRegister<T>::addCtor()
{
	// 获取类table
	lua_getglobal(L, ClassMeta<T>::getName());

	char ctorName[100];
	memset(ctorName, 0, 100);

	if (m_ctorNum != 0)
		sprintf(ctorName,"new%d", m_ctorNum);
	else
		strcpy(ctorName, "new");

	if (lua_istable(L, -1))
	{
		lua_pushstring(L, ctorName);
		pushCtorProxy<T, Args...>(L);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);

	m_ctorNum++;
}

template<typename T>
template<typename Ret, typename ...Args>
inline void ClassRegister<T>::addOverrideMember(const char *name, Ret(T::*func)(Args...))
{
	addMember(name, func);
}

template<typename T>
template<typename Ret, typename ...Args>
inline void ClassRegister<T>::addOverrideMember(const char *name, Ret(T::*func)(Args...)const)
{
	addMember(name, func);
}

template<typename T>
template<typename F>
inline void ClassRegister<T>::addMember(const char *name, F func)
{        
	// 获取类table
	lua_getglobal(L, ClassMeta<T>::getName());
	if (lua_istable(L, -1))
	{
		// 压入函数到类table
		lua_pushstring(L, name);
		new(lua_newuserdata(L, sizeof(decltype(func)))) decltype(func)(func);
		pushMemProxy(L, func);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}

template<typename T>
template<typename F>
inline void ClassRegister<T>::addVarMember(const char *name, F T::*var)
{
	// 获取类table
	lua_getglobal(L, ClassMeta<T>::getName());
	if (lua_istable(L, -1))
	{
		// 压入类参数
		lua_pushstring(L, name);
		new(lua_newuserdata(L, sizeof(mem_var<T, F>))) mem_var<T, F>(var);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}


template<typename T>
template<typename F>
inline void ClassRegister<T>::addStaticMember(const char *name, F func)
{
	// 获取类table
	lua_getglobal(L, ClassMeta<T>::getName());
	if (lua_istable(L, -1))
	{
		// 压入函数到类table
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, (void*)func);
		pushGlobleProxy(L, func);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}

template<typename T>
template<typename Ret, typename ...Args>
inline void ClassRegister<T>::addOverrideStaticMember(const char *name, Ret(*func)(Args...))
{
	addStaticMember(name, func);
}

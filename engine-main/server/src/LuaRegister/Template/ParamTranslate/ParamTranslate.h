#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/ParamTranslate/BaseTypeTranslate.h"
#include "LuaRegister/Template/ParamTranslate/UserDataTranslate.h"
#include "LuaRegister/Template/ParamTranslate/FunctionalTranslate.h"
#include "LuaRegister/Template/ParamTranslate/ContainerTranslate.h"

class TableVisitor;

template<typename T>
struct IsUserType
{
	static constexpr bool value = (!IsBaseType<T>::value && !IsFunctional<T>::value && !IsContainer<T>::value &&  !std::is_same<typename ParamType<T>::type, TableVisitor>::value);
};


///////////////////////////////////// forward declare/////////////////////////////////////////////////////
template<typename T, typename ...Args>
static void write_n(lua_State *L, T&& a, Args&&... args);


namespace FunctionalTranslate
{
	template<typename T>
	T read(lua_State *L, int index);

	template<typename T>
	void write(lua_State *L, T t);
};

namespace ContainerTranslate
{
	template<typename T>
	T read(lua_State *L, int index);

	template<typename T>
	void write(lua_State *L, const T&t);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ParamTranslate
{

	template<typename T>
	static typename std::enable_if<!IsBaseType<typename ParamType<T>::type>::value && !IsContainer<typename ParamType<T>::type>::value, T>::type read(lua_State *L, int index)
	{
		return innerRead<T>(L, index);
	}

	// base type 不让它返回引用
	template<typename T>
	static typename std::enable_if<IsBaseType<typename ParamType<T>::type>::value, typename ParamType<T>::type>::type read(lua_State *L, int index)
	{
		return BaseTypeTranslate::read<typename ParamType<T>::type>(L, index);
	}

	// container type 不让它返回引用
	template<typename T>
	static typename std::enable_if<IsContainer<typename ParamType<T>::type>::value, typename ParamType<T>::type>::type read(lua_State *L, int index)
	{
		return ContainerTranslate::read<typename ParamType<T>::type>(L, index);
	}

	template<typename T>
	static void write(lua_State *L, T t)
	{
		innerWrite<T>(L, t);
	}

	//user data
	template<typename T>
	static typename std::enable_if<IsUserType<typename ParamType<T>::type >::value, T>::type innerRead(lua_State *L, int index)
	{
		return UserDataTranslate::read<T>(L, index);
	}

	template<typename T>
	static typename std::enable_if<IsUserType<typename ParamType<T>::type>::value, void>::type innerWrite(lua_State *L, T t)
	{
		UserDataTranslate::write<T>(L, t);
	}


	// base type


	template<typename T>
	static typename std::enable_if<IsBaseType<typename ParamType<T>::type>::value, void>::type innerWrite(lua_State *L, T t)
	{
		BaseTypeTranslate::write(L, t);
	}


	// std::function type

	template<typename T>
	static typename std::enable_if<IsFunctional<T>::value, T>::type innerRead(lua_State *L, int index)
	{
		return	FunctionalTranslate::read<T>(L, index);
	}

	template<typename T>
	static typename std::enable_if<IsFunctional<T>::value, void>::type  innerWrite(lua_State *L, T t)
	{
		FunctionalTranslate::write<T>(L, t);
	}


	// containner type
	template<typename T>
	static typename std::enable_if<IsContainer<typename ParamType<T>::type>::value, void>::type innerWrite(lua_State *L, const T& t)
	{
		ContainerTranslate::write<typename ParamType<T>::type>(L, t);
	}

	// tableVisitor
	template<typename T>
	static typename std::enable_if<std::is_same<typename ParamType<T>::type, TableVisitor>::value, T>::type innerRead(lua_State *L, int index)
	{
		return CastVar<T>::CreateVar(L, index);
	}

	template<typename T>
	static typename std::enable_if<std::is_same<typename ParamType<T>::type, TableVisitor>::value, void>::type innerWrite(lua_State *L, T t)
	{
		CastVar<T>::GetVar(t).push();
	}

	template<typename T>
	static typename std::enable_if<!std::is_void<T>::value, T>::type  pop(lua_State *L)
	{
		T t = ParamTranslate::read<T>(L, -1);
		lua_pop(L, 1);
		return t;
	}

	template<typename T>
	static typename std::enable_if<std::is_void<T>::value, void>::type pop(lua_State *L)
	{
		lua_pop(L, 1);
	};


};

namespace FunctionalTranslate
{
	struct FuncionalProxy
	{
		LuaRef _ref;
		lua_State *L;

		void invoke() const
		{
			_ref.Push(L);
		}
	};

	template<typename T>
	struct innerRead
	{
		static T invoke(lua_State*L, int index)
		{

		}
	};


	template<typename RVal, typename ...Args>
	struct innerRead<std::function<RVal(Args...)>>
	{
		static std::function<RVal(Args...)> invoke(lua_State *L, int index)
		{
			lua_pushvalue(L, index);
			FuncionalProxy ref = FuncionalProxy{ { L, luaL_ref(L, LUA_REGISTRYINDEX) }, L };

			auto rt = [ref](Args... args)
			{
				lua_rawgeti(ref.L, LUA_REGISTRYINDEX, -2);	// debug.traceback
				int errfunc = lua_gettop(ref.L);
				ref.invoke();
				write_n<Args...>(ref.L, std::forward<Args>(args)...);
				if (lua_pcall(ref.L, sizeof...(Args), 1, errfunc))
					LuaDebug::printError(ref.L, "%s", lua_tostring(ref.L, -1));

				lua_remove(ref.L, errfunc);

				return ParamTranslate::pop<RVal>(ref.L);
			};

			return rt;
		}
	};


	template<typename T>
	T read(lua_State *L, int index)
	{
		return innerRead<T>::invoke(L, index);
	}

	template<typename T>
	void write(lua_State *L, T t)
	{
		pushGlobleProxy(L, t);
	}

}

namespace ContainerTranslate
{

	template<typename T>
	struct innerRead {};
	template<typename T>
	struct innerWrite {};

	template<typename T>
	struct innerRead<std::vector<T>>
	{
		static	std::vector<T> invoke(lua_State*L, int index)
		{
			std::vector<T> ret;


			size_t len = lua_objlen(L, index);

			ret.reserve(len);

			lua_pushvalue(L, index);
			for (size_t i = 1; i <= len; ++i) {
				lua_pushinteger(L, static_cast<unsigned int>(i));
				lua_gettable(L, -2);
				ret.push_back(ParamTranslate::read<T>(L, -1));
				lua_pop(L, 1);
			}
			lua_pop(L, 1);

			return ret;
		}
	};

	template<typename T>
	struct innerWrite<std::vector<T>>
	{
		static void invoke(lua_State *L, std::vector<T> v)
		{
			unsigned int res = 1;
			lua_newtable(L);
			int tb = lua_gettop(L);
			for (const T &ele : v) {
				// 目前只支持一个值
				lua_pushinteger(L, res);
				ParamTranslate::write<T>(L, ele);
				lua_settable(L, -3);

				++res;
			}
			lua_settop(L, tb);
		}
	};

	template<typename T>
		T read(lua_State *L, int index)
	{
		return innerRead<T>::invoke(L, index);
	}

	template<typename T>
		void write(lua_State *L, const T& t)
	{
		return innerWrite<T>::invoke(L, t);
	}
};


template<typename T, typename ...Args>
static void write_n(lua_State *L, T&& a, Args&&... args)
{
	ParamTranslate::write<T>(L, std::forward<T>(a));
	write_n<Args...>(L, std::forward<Args>(args)...);
}

template<typename T>
static void write_n(lua_State *L, T &&a)
{
	ParamTranslate::write<T>(L, std::forward<T>(a));
};

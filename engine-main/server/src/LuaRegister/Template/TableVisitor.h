#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/LuaCall.h"
#include <atomic>
#include "Core.h"
class TableVisitor
{
	struct RealTableVisiter
	{
		lua_State *m_L;
		int m_ref;

		RealTableVisiter(TableVisitor *table)
		{
			m_L = table->m_L;
			m_ref = table->m_ref;
			lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
		}

		~RealTableVisiter()
		{
			lua_pop(m_L, 1);
		}
	};

public:
	TableVisitor(lua_State *L, const char *tableName = nullptr, bool create = true);
	TableVisitor(lua_State *L, int index, bool removeIndexTable = false);
	TableVisitor(const TableVisitor & table);
	TableVisitor& operator=(const TableVisitor & table);

	~TableVisitor();

public:

	template<typename RVal, typename ...Args>
	bool callFunc(RVal &rval, const char * funcName, Args&&...args)
	{
		RealTableVisiter table(this);
		return LUA_CALL::callRet(this->m_L, -1, funcName, rval, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	bool callFunc(const char *funcName, Args&&...args)
	{
		RealTableVisiter table(this);
		return LUA_CALL::call(this->m_L, -1, funcName, std::forward<Args>(args)...);
	}

	template<typename RVal>
	bool callFunc(RVal & rval, const char *funcName)
	{
		RealTableVisiter table(this);
		return LUA_CALL::callRet(m_L, -1, funcName, rval);
	}

	template<typename T>
	bool getField(const char *fieldName, T& t)
	{
		RealTableVisiter table(this);
		return LUA_CALL::getField(m_L, -1, fieldName, t);
	}

	template<typename T>
	bool setField(const char *fieldName, T& t)
	{
		RealTableVisiter table(this);
		return LUA_CALL::setField(m_L, -1, fieldName, t);
	}

	template<typename T>
	bool geti(int i, T& t)
	{
		RealTableVisiter table(this);
		return LUA_CALL::geti(m_L, -1, i, t);
	}

	template<typename T>
	bool seti(int i, T& t)
	{
		RealTableVisiter table(this);
		return LUA_CALL::seti(m_L, -1, i, t);
	}

	int len()
	{
		RealTableVisiter table(this);
		return (int)luaL_getn(m_L, -1);
	}

	lua_State * luaState()
	{
		return m_L;
	}

	std::shared_ptr<TableVisitor> getTable(const char*tableName);
	std::shared_ptr<TableVisitor> getTable(int i);

	void push();

private:
	std::shared_ptr<int> m_refCount;
	lua_State  *m_L;
	int m_ref;
};


inline
TableVisitor::TableVisitor(lua_State *L, int index, bool removeIndexTable)
{
	this->m_L = L;
	lua_pushvalue(L, index);
	m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	if (removeIndexTable)
		lua_remove(L, index);
	m_refCount = std::make_shared<int>();
}

inline
TableVisitor::TableVisitor(lua_State *L, const char *tableName, bool create)
{
	m_L = L;
	if (create)
	{
		lua_newtable(L);
		if (tableName)
		{
			lua_pushstring(L, "__name");
			lua_pushstring(L, tableName);
			lua_rawset(L, -3);
		}
		m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	else
	{
		lua_getglobal(L, tableName);
		m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	m_refCount = std::make_shared<int>();
}

inline
TableVisitor::TableVisitor(const TableVisitor& table)
{
	this->m_L = table.m_L;
	this->m_ref = table.m_ref;
	this->m_refCount = table.m_refCount;
}

inline
TableVisitor& TableVisitor::operator=(const TableVisitor & table)
{
	if (m_refCount.use_count() == 1)
	{
		luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
	}

	this->m_L = table.m_L;
	this->m_ref = table.m_ref;
	this->m_refCount = table.m_refCount;

	return *this;
}

inline
TableVisitor::~TableVisitor()
{
	if (m_refCount.use_count() == 1)
	{
		luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
	}
}

inline
std::shared_ptr<TableVisitor> TableVisitor::getTable(const char*tableName)
{
	RealTableVisiter table(this);
	lua_getfield(m_L, -1, tableName);
	if (lua_istable(m_L, -1))
	{
		return std::shared_ptr<TableVisitor>(new TableVisitor(m_L, lua_gettop(m_L), true));
	}
	else
	{
		lua_pop(m_L, 1);
		return NULL;
	}
}

inline
std::shared_ptr<TableVisitor> TableVisitor::getTable(int i)
{
	RealTableVisiter table(this);
	lua_pushinteger(m_L, i);
	lua_gettable(m_L, -2);
	if (lua_istable(m_L, -1))
	{
		return std::shared_ptr<TableVisitor>(new TableVisitor(m_L, lua_gettop(m_L), true));
	}
	else
	{
		lua_pop(m_L, 1);
		return NULL;
	}
}

inline
void TableVisitor::push()
{
	lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
}
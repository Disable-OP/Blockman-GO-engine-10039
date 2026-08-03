#include "LuaRegister/Template/LuaExt.h"

#if defined(_MSC_VER)
#define I64_FMT "I64"
#elif defined(__APPLE__) 
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif


void LuaExt::init(lua_State *L)
{
	init_sign64(L);
	init_unsign64(L);
}
/*---------------------------------------------------------------------------*/
/* __s64                                                                     */
/*---------------------------------------------------------------------------*/
static int tostring_s64(lua_State *L)
{
	char temp[64];
	sprintf(temp, "%" I64_FMT "d", *(long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int eq_s64(lua_State *L)
{
	int64_t a = *(int64_t*)lua_touserdata(L, 1);
	int64_t b = *(int64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a == b));
	return 1;
}

/*---------------------------------------------------------------------------*/
static int lt_s64(lua_State *L)
{
	int64_t a = *(int64_t*)lua_touserdata(L, 1);
	int64_t b = *(int64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a < b));
	return 1;
}

/*---------------------------------------------------------------------------*/
static int le_s64(lua_State *L)
{
	int64_t a = *(int64_t*)lua_touserdata(L, 1);
	int64_t b = *(int64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a <= b));
	return 1;
}

/*---------------------------------------------------------------------------*/
void LuaExt::init_sign64(lua_State *L)
{
	const char* name = "__s64";
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_s64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_s64, 0);
	lua_rawset(L, -3);

	lua_setglobal(L, name); //pop table
}

/*---------------------------------------------------------------------------*/
/* __u64                                                                     */
/*---------------------------------------------------------------------------*/
static int tostring_u64(lua_State *L)
{
	char temp[64];
	sprintf(temp, "%" I64_FMT "u", *(unsigned long long*)lua_topointer(L, 1));
	lua_pushstring(L, temp);
	return 1;
}

/*---------------------------------------------------------------------------*/
static int eq_u64(lua_State *L)
{
	uint64_t a = *(uint64_t*)lua_touserdata(L, 1);
	uint64_t b = *(uint64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a == b));
	return 1;
}

/*---------------------------------------------------------------------------*/
static int lt_u64(lua_State *L)
{
	uint64_t a = *(uint64_t*)lua_touserdata(L, 1);
	uint64_t b = *(uint64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a < b));
	return 1;
}

/*---------------------------------------------------------------------------*/
static int le_u64(lua_State *L)
{
	uint64_t a = *(uint64_t*)lua_touserdata(L, 1);
	uint64_t b = *(uint64_t*)lua_touserdata(L, 2);
	lua_pushboolean(L, (a <= b));
	return 1;
}

/*---------------------------------------------------------------------------*/
void LuaExt::init_unsign64(lua_State *L)
{
	const char* name = "__u64";
	lua_newtable(L);

	lua_pushstring(L, "__name");
	lua_pushstring(L, name);
	lua_rawset(L, -3);

	lua_pushstring(L, "__tostring");
	lua_pushcclosure(L, tostring_u64, 0);
	lua_rawset(L, -3);

	// 比较只会两边元表都是相同的元素才会进入比较
	lua_pushstring(L, "__eq");
	lua_pushcclosure(L, eq_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__lt");
	lua_pushcclosure(L, lt_u64, 0);
	lua_rawset(L, -3);

	lua_pushstring(L, "__le");
	lua_pushcclosure(L, le_u64, 0);
	lua_rawset(L, -3);

	// __concat, __add等只要是其中一个元素的元表属于该元表就会进入
	//lua_pushstring(L, "__concat");
	//lua_pushcclosure(L, concat_u64, 0);
	//lua_rawset(L, -3);

	lua_setglobal(L, name);
}
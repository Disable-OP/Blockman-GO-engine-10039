//
// Created by alex on 09/07/2015.
//

#include "lbitop.h"

static int b_arshift(lua_State *L) {
    luaL_error(L, "b_arshift: not implemented!");
	return 0;
}

/* bit and
 *
 * a & b
 *
 * */
static int b_and(lua_State *L) {
    int a = luaL_checkint(L, 1);
    int b = luaL_checkint(L, 2);
    lua_pushnumber(L, a & b);
	return 1;
}

/* bit not
 *
 * ~a
 *
 * */
static int b_not(lua_State *L) {
    int a = luaL_checkint(L, 1);
    lua_pushnumber(L, ~a);
	return 1;
}

/* bit or
 *
 * a | b
 *
 * */
static int b_or(lua_State *L) {
    int a = luaL_checkint(L, 1);
    int b = luaL_checkint(L, 2);
    lua_pushnumber(L, a | b);
	return 1;
}

/* bit xor
 *
 * a ^ b
 *
 * */
static int b_xor(lua_State *L) {
    int a = luaL_checkint(L, 1);
    int b = luaL_checkint(L, 2);
    lua_pushnumber(L, a ^ b);
	return 1;
}

static int b_lrot(lua_State *L) {
    luaL_error(L, "b_lrot: not implemented!");
	return 0;
}

/* bit lshift
 *
 * a << b
 *
 * */
static int b_lshift(lua_State *L) {
    int a = luaL_checkint(L, 1);
    int b = luaL_checkint(L, 2);
    lua_pushnumber(L, a << b);
	return 1;
}

static int b_rrot(lua_State *L) {
    luaL_error(L, "b_rrot: not implemented!");
	return 0;
}

/* bit rshift
 *
 * a >> b
 *
 * */
static int b_rshift(lua_State *L) {
    int a = luaL_checkint(L, 1);
    int b = luaL_checkint(L, 2);
    lua_pushnumber(L, a >> b);
	return 1;
}

static const luaL_reg bitlib[] = {
    {"barshift", b_arshift},
    {"band",     b_and},
    {"bnot",     b_not},
    {"bor",      b_or},
    {"bxor",     b_xor},
    {"blrotate", b_lrot},
    {"blshift",  b_lshift},
    {"brrotate", b_rrot},
    {"brshift",  b_rshift},
	{NULL, NULL}
};

LUALIB_API int luaopen_bitop(lua_State *L)
 {
	const char *tableName = "bitop";
	luaL_register(L, tableName, bitlib);
	return 1;
}
#pragma once
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <memory>
#include <tuple>
#include <functional>
#include <exception>
#include <vector>
#include <typeinfo>


// 注册函数 支持的参数个数。依赖 宏循环的 最大循环次数。现在最多是13
#define  MAX_ARGS 10
#pragma once
#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/LuaRef.h"
#include "LuaRegister/Template/Util/LuaDebug.h"

template<typename T>
struct IsFunctional
{
	static constexpr bool value = false;
};

template<typename RVal, typename ...Args>
struct IsFunctional<std::function<RVal(Args...)>>
{
	static constexpr bool value = true;
};
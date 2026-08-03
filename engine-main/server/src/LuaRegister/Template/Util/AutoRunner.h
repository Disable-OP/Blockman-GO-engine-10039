#pragma once
#include "LuaRegister/Template/Public.h"
#include "Core.h"
#include "Log/Log.h"

using namespace LORD;
struct AutoRunner
{
	AutoRunner(std::function<void()> fuc);

	static void doRegister();
	static typename std::vector<std::function<void()> > registers;
};



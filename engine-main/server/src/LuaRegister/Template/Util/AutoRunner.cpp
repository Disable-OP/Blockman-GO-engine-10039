#include "LuaRegister/Template/Util/AutoRunner.h"

AutoRunner::AutoRunner(std::function<void()> fuc)
{
	//registers.push_back(fuc);
	fuc();
}

void AutoRunner::doRegister()
{
	LordLogInfo("AutoRunner::doRegister");
	for (auto reg : registers)
	{
		reg();
	}
}

typename std::vector<std::function<void()>> AutoRunner::registers;
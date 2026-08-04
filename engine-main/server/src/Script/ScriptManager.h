#pragma once
#include "Object/Singleton.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "LuaRegister/Template/Util/AutoRunner.h"
#include "Script/GameServerEvents.h"
using namespace LORD;

const char *Script_Main = "ScriptMain.lua";

class ScriptManager:public Singleton<ScriptManager>
{

public:
	void loadScript(const char* path, bool isMain)
	{
	//	AutoRunner::doRegister();
		LuaEngine::getInstance()->addScriptDir(path);

		if (isMain)
		{
			std::string MainPath = path;
			MainPath += "/";
			MainPath += Script_Main;
			LuaEngine::getInstance()->Load(MainPath.c_str());
		}

		//initDefaultEvent();  open throw tnt for all game
	}

	void initDefaultEvent()
	{
		SCRIPT_EVENT::PlayerUseThrowableItemEvent::registerCallBack([](int){ return false; });
	}
};
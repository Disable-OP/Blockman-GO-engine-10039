#ifndef __SCRIPT_SETTING_HEADER__
#define __SCRIPT_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ScriptSetting
	{
	public:
		static bool	loadScriptDir(String & path);
	};
}

#endif // !__SCRIPT_SETTING_HEADER__

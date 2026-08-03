#ifndef __LOG_CONFIG_HEADER__
#define __LOG_CONFIG_HEADER__

#include "Memory/LordMemory.h"
#include "g3log/loglevels.hpp"

namespace LORD
{
	struct LogConfig
	{
		String logName = "LordLog";
		String logFilename = "lordlog.txt";
		bool bConsoleOutput = true;
		bool bVSOutput = false;
		bool bFileOutput = false;
		LEVELS logLevel = G3LOG_INFO;
		bool bTimeStamp = true;
		bool bLogAppend = false;

		static LogConfig readConfig(const String& logCfgFile);
		static LogConfig createDefaultConfig();
	};
}

#endif // !__LOG_CONFIG_HEADER__

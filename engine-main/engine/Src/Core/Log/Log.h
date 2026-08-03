/********************************************************************
filename: 	Log.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_LOG_H__
#define __LORD_LOG_H__

#include "Memory/LordMemory.h"
#include "g3log/g3log.hpp"
#include "LogConfig.h"

namespace LORD
{
	class LORD_CORE_API Log
	{
	public:
		Log() = delete;

		static bool isInitialized() { return initialized; }
		static void init(const String& logCfgFile = "LordLog.cfg");
		template<typename StringType>
		static String format(LEVELS level, const StringType& message)
		{
			return format(level, String(message.c_str()));
		}
		static String format(LEVELS level, const String& message);
		static LogConfig getConfig() { return config; }
		static String getCallStack() { return g3::internal::stackdump().c_str(); }
		static void printCallStack() { LOG(G3LOG_INFO) << "******* STACKDUMP *******\n" << getCallStack(); }

	private:
		static LogConfig config;
		static bool initialized;
	};
}

#define LordLogDebug(formats, ...)      LOGF(G3LOG_DEBUG, formats, ##__VA_ARGS__);
#define LordLogInfo(formats, ...)       LOGF(G3LOG_INFO, formats, ##__VA_ARGS__);
#define LordLogWarning(formats, ...)    LOGF(G3LOG_WARNING, formats, ##__VA_ARGS__);
#define LordLogError(formats, ...)      LOGF(G3LOG_ERROR, formats, ##__VA_ARGS__);
#define LordLogFatal(formats, ...)      LOGF(G3LOG_FATAL, formats, ##__VA_ARGS__);

#endif

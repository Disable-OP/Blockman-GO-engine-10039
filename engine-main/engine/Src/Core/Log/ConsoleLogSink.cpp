#include "Core.h"
#include "ConsoleLogSink.h"
#include "Log.h"

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#elif (LORD_PLATFORM == LORD_PLATFORM_ANDROID)
#	include <android/log.h>
#endif

void LORD::ConsoleLogSink::logMessage(g3::LogMessageMover logEntry)
{
	const String& msgStr = Log::format(logEntry.get()._level, logEntry.get().message());
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID)
	android_LogPriority priority;
	switch (logEntry.get()._level.value)
	{
	case g3::kDebugValue:		priority = ANDROID_LOG_DEBUG;	break;
	case g3::kInfoValue:		priority = ANDROID_LOG_INFO;	break;
	case g3::kWarningValue:		priority = ANDROID_LOG_WARN;	break;
	case g3::kErrorValue:		priority = ANDROID_LOG_ERROR;	break;
	case g3::kFatalValue:		priority = ANDROID_LOG_FATAL;	break;
	default:			priority = ANDROID_LOG_INFO;	break;
	}

	__android_log_write(priority, Log::getConfig().logName.c_str(), msgStr.c_str());
	//writelogtosdcard(msgStr.c_str());
#else
	printf("%s", msgStr.c_str());
#endif
}

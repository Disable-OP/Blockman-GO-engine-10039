#ifndef __VS_CONSOLE_LOG_SINK_HEADER__
#define __VS_CONSOLE_LOG_SINK_HEADER__

#include "Memory/LordMemory.h"
#include "g3log/logmessage.hpp"

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

namespace LORD
{
	class VSConsoleLogSink
	{
	public:
		void logMessage(g3::LogMessageMover logEntry);
	};
}

#endif // !__VS_CONSOLE_LOG_SINK_HEADER__

#ifndef __CONSOLE_LOG_SINK_HEADER__
#define __CONSOLE_LOG_SINK_HEADER__

#include "Memory/LordMemory.h"
#include "g3log/logmessage.hpp"

namespace LORD
{
	class ConsoleLogSink
	{
	public:
		void logMessage(g3::LogMessageMover logEntry);
	};
}

#endif // !__CONSOLE_LOG_SINK_HEADER__

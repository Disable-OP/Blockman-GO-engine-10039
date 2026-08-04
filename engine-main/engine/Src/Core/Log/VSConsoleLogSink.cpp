#include "Core.h"
#include "VSConsoleLogSink.h"
#include "Log.h"

void LORD::VSConsoleLogSink::logMessage(g3::LogMessageMover logEntry)
{
	const String& message = Log::format(logEntry.get()._level, logEntry.get().message());
#if defined(_WIN32) || defined(WIN32)
	OutputDebugString(message.c_str());
	printf(message.c_str());
#endif
}

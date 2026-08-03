#ifndef __FILE_LOG_SINK_HEADER__
#define __FILE_LOG_SINK_HEADER__

#include "Memory/LordMemory.h"
#include "g3log/logmessage.hpp"
#include "LogConfig.h"

namespace LORD
{
	class FileLogSink
	{
	public:
		FileLogSink();
		~FileLogSink()
		{
			m_logStream.close();
		}
		void logMessage(g3::LogMessageMover logEntry);

	private:
		OFStream m_logStream;

		void open();
		void writeFirstMessage();
	};
}

#endif // !__FILE_LOG_SINK_HEADER__

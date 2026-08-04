#include "Core.h"
#include "FileLogSink.h"
#include "Object/Root.h"
#include "Log.h"

namespace LORD
{
	FileLogSink::FileLogSink()
	{
		open();
		writeFirstMessage();
	}

	void FileLogSink::open()
	{
		const auto& config = Log::getConfig();
		String strLogPath = Root::Instance()->getWriteablePath() + config.logFilename;

		if (config.bLogAppend)
		{
			m_logStream.open(strLogPath.c_str(), std::ios::app);
			m_logStream << std::endl;
			m_logStream.flush();
		}
		else
		{
			m_logStream.open(strLogPath.c_str());
		}
	}

	void FileLogSink::writeFirstMessage()
	{
		String msg;
		msg += "----------------------------------------------------------------------------------------------------\n";

		struct tm *pTime;
		time_t ctTime;
		time(&ctTime);
		pTime = localtime(&ctTime);
		msg += StringUtil::Format("LOG DATE: %02d-%02d-%04d %02d:%02d:%02d \n",
			pTime->tm_mon + 1,
			pTime->tm_mday,
			pTime->tm_year + 1900,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec);

		msg += "----------------------------------------------------------------------------------------------------\n";
		m_logStream << msg;
		m_logStream.flush();
	}

	void FileLogSink::logMessage(g3::LogMessageMover logEntry)
	{
		m_logStream << Log::format(logEntry.get()._level, logEntry.get().message());
		m_logStream.flush();
	}
}
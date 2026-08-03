#include "Core.h"
#include "Util/StringUtil.h"
#include "Log.h"
#include "LogConfig.h"
#include "Object/Root.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include "g3log/loglevels.hpp"
#include "ConsoleLogSink.h"
#include "VSConsoleLogSink.h"
#include "FileLogSink.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include "windows.h"
#endif

namespace LORD
{
	LogConfig Log::config;
	bool Log::initialized = false;

	void Log::init(const String& logCfgFile)
	{
		if (initialized)
		{
			return;
		}
		String configPath;
		if (!PathUtil::IsAbsolutePath(logCfgFile))
		{
			configPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), logCfgFile);
		}
		else
		{
			configPath = logCfgFile;
		}
		static std::unique_ptr<g3::LogWorker> logWorker(g3::LogWorker::createLogWorker());
		g3::initializeLogging(logWorker.get());
		config = LogConfig::readConfig(configPath);
		config.logName = "Main";
		g3::log_levels::setHighest(config.logLevel);
		if (config.bConsoleOutput)
		{
			logWorker->addSink(std2::make_unique<ConsoleLogSink>(), &ConsoleLogSink::logMessage);
		}
		if (config.bVSOutput)
		{
			logWorker->addSink(std2::make_unique<VSConsoleLogSink>(), &VSConsoleLogSink::logMessage);
		}
		if (config.bFileOutput)
		{
			logWorker->addSink(std2::make_unique<FileLogSink>(), &FileLogSink::logMessage);
		}
		initialized = true;
	}

	String Log::format(LEVELS level, const String & message)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		switch (level.value)
		{
		case 100:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case 300:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00F | FOREGROUND_INTENSITY);
			break;
		case 500:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x006 | FOREGROUND_INTENSITY);
			break;
		case 800:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case 1000:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		default:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00F | FOREGROUND_INTENSITY);
			break;
		}
#endif
		String msgStr;

		if (config.bTimeStamp)
		{
			struct tm* pTime;
			time_t ctTime;
			time(&ctTime);
			pTime = localtime(&ctTime);
			msgStr += StringUtil::Format("%02d:%02d:%02d ", pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
		}
		msgStr += ("[" + level.text + "]: ").c_str();
		msgStr += message;
		msgStr = "(" + config.logName + ") " + msgStr + "\n";
		return msgStr;
	}
}


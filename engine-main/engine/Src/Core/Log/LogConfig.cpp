#include "Core.h"
#include "LogConfig.h"
#include "Object/Root.h"

// Keywords
#define _LOGCFG_APPENDER				"APPENDER"
#define _LOGCFG_APPENDER_FILE			"FILE"
#define _LOGCFG_APPENDER_CONSOLE		"CONSOLE"
#define _LOGCFG_APPENDER_VSCONSOLE		"VSCONSOLE"
#define _LOGCFG_LOGFILE					"LOGFILE"
#define _LOGCFG_LOGLEVEL				"LOGLEVEL"
#define _LOGCFG_LOGLEVEL_DEBUG			"DEBUG"
#define _LOGCFG_LOGLEVEL_INFO			"INFO"
#define _LOGCFG_LOGLEVEL_WARNING		"WARNING"
#define _LOGCFG_LOGLEVEL_ERROR			"ERROR"
#define _LOGCFG_LOGLEVEL_FATAL			"FATAL"
#define _LOGCFG_TIMESTAMP				"TIMESTAMP"
#define _LOGCFG_LOGAPPEND				"LOGAPPEND"

namespace LORD
{
	LogConfig LogConfig::readConfig(const String & logCfgFile)
	{
		IFStream fin;

		// Open the log config file.
		//std::locale loc = std::locale::global(std::locale(""));
#if(LORD_PLATFORM == LORD_PLATFORM_ANDROID)
#endif
		fin.open(logCfgFile.c_str());
		//std::locale::global(loc);

		// If it could not open the file then exit.
		if (fin.fail())
		{
			//LordLogInfo("Not found log config file, create default log.");
			return createDefaultConfig();
		}
		String logName;
		String logFilename;
		bool bConsoleOutput = false;
		bool bVSOutput = false;
		bool bFileOutput = false;
		LEVELS logLevel = G3LOG_INFO;
		bool bTimeStamp = true;
		bool bLogAppend = false;

		char content[256];
		String contentStr;
		String key;
		String value;

		while (fin.getline(content, 256))
		{
			contentStr = content;
			if (!contentStr.empty() && contentStr[0] == '#')		// comment
				continue;

			size_t pos = contentStr.find('=');
			if (pos != String::npos)
			{
				key = contentStr.substr(0, pos);
				StringUtil::Trim(key);
				StringUtil::UpperCase(key);

				value = contentStr.substr(pos + 1);
				StringUtil::Trim(value);
				StringUtil::UpperCase(value);

				// config appender
				if (key == _LOGCFG_APPENDER)
				{
					StringArray values = StringUtil::Split(value, ",");
					StringArray::iterator valuesIt = values.begin();
					for (; valuesIt != values.end(); ++valuesIt)
					{
						String tempValue = *valuesIt;
						StringUtil::Trim(tempValue);

						if (tempValue == _LOGCFG_APPENDER_CONSOLE)
						{
							bConsoleOutput = true;
						}
						else if (tempValue == _LOGCFG_APPENDER_VSCONSOLE)
						{
							bVSOutput = true;
						}
						else if (tempValue == _LOGCFG_APPENDER_FILE)
						{
							bFileOutput = true;

							size_t tempPos = logCfgFile.find('.');

							// set default log filename
							if (tempPos != String::npos)
							{
								logName = logCfgFile.substr(0, tempPos);
							}
							else
							{
								logName = logCfgFile;
							}
						}
					}
				}
				else if (key == _LOGCFG_LOGFILE)			// config the log file
				{
					if (!value.empty())
						logFilename = value;
				}
				else if (key == _LOGCFG_LOGLEVEL)			// config the log level
				{
					if (value == _LOGCFG_LOGLEVEL_DEBUG)
						logLevel = G3LOG_DEBUG;
					else if (value == _LOGCFG_LOGLEVEL_INFO)
						logLevel = G3LOG_INFO;
					else if (value == _LOGCFG_LOGLEVEL_WARNING)
						logLevel = G3LOG_WARNING;
					else if (value == _LOGCFG_LOGLEVEL_ERROR)
						logLevel = G3LOG_ERROR;
					else if (value == _LOGCFG_LOGLEVEL_FATAL)
						logLevel = G3LOG_FATAL;
				}
				else if (key == _LOGCFG_TIMESTAMP)			// config the time stamp
				{
					if (value == "TRUE")
						bTimeStamp = true;
					else
						bTimeStamp = false;
				}
				else if (key == _LOGCFG_LOGAPPEND)			// check if append log to the end of file 
				{
					if (value == "TRUE")
						bLogAppend = true;
					else
						bLogAppend = false;
				}
			}
		} // while

		LogConfig logConfig;
		logConfig.logName = logName;
		logConfig.logFilename = logFilename;
		logConfig.bConsoleOutput = bConsoleOutput;
		logConfig.bVSOutput = bVSOutput;
		logConfig.bFileOutput = bFileOutput;
		logConfig.logLevel = logLevel;
		logConfig.bTimeStamp = bTimeStamp;
		logConfig.bLogAppend = bLogAppend;

		return logConfig;
	}

	LogConfig LogConfig::createDefaultConfig()
	{
		LogConfig logConfig;

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)

		if (!Root::Instance()->isEditorMode())
		{
			time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);

			char logName[512] = { 0 };
			memset(logName, 0, 512);
			sprintf_s(logName, "LordLog_%d_%d_%d_%d_%d.txt", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

			logConfig.logFilename = logName;
		}
#elif (LORD_PLATFORM == LORD_PLATFORM_LINUX)
		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		char logName[512] = { 0 };
		memset(logName, 0, 512);
		sprintf(logName, "LordLog_%d_%d_%d_%d_%d.txt", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		logConfig.logFilename = logName; 
#else
		logConfig.logFilename = "server.log";
#endif

#if (LORD_COMPILER == LORD_COMPILER_MSVC)
		logConfig.bConsoleOutput = false;
		logConfig.bVSOutput = true;
#else
	#if (LORD_PLATFORM == LORD_PLATFORM_LINUX)
		logConfig.bConsoleOutput = false;
	#else
		logConfig.bConsoleOutput = true;
	#endif
		logConfig.bVSOutput = false;
#endif

		logConfig.bFileOutput = true;
		logConfig.logLevel = G3LOG_DEBUG;
		logConfig.bTimeStamp = true;
		logConfig.bLogAppend = false;

		return logConfig;
	}
}
#include <algorithm>
#include "GameVersionDefine.h"
#include "rapidjson/schema.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN 
{
	int EngineVersionSetting::m_engineVersion = 10000;
	bool EngineVersionSetting::m_useCmd = true;
	LEVELS EngineVersionSetting::m_logLevel = G3LOG_DEBUG;

	bool EngineVersionSetting::loadSetting()
	{
		String path = "engineVersion.json";  
		DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(path);
		if (pDataStream == NULL)
		{
			LordLogError("%s not exist!", path.c_str());
			return false;
		}

		size_t nDataSize = pDataStream->size();
		char* pJSONData = (char*)LordMalloc(nDataSize + 1);
		pDataStream->read(pJSONData, nDataSize);
		pJSONData[nDataSize] = 0;

		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(pJSONData);
		if (doc->HasParseError())
		{
			LordLogError("%s parse error!", path.c_str());
			delete doc;
			LordFree(pJSONData);
			LordDelete(pDataStream);
			return false;
		}

		if (doc->HasMember("engineVersion"))
		{
			m_engineVersion = doc->FindMember("engineVersion")->value.GetInt();
		}

		if (doc->HasMember("useCmd"))
		{
			std::string ret = doc->FindMember("useCmd")->value.GetString();
			m_useCmd = ret == "true";
		}

		if (doc->HasMember("logLevel"))
		{
			std::string logLevel = doc->FindMember("logLevel")->value.GetString();
			std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::tolower);
			if (logLevel == "debug") {
				m_logLevel = G3LOG_DEBUG;
			}
			else if (logLevel == "info") {
				m_logLevel = G3LOG_INFO;
			}
			else if (logLevel == "warning") {
				m_logLevel = G3LOG_WARNING;
			}
			else if (logLevel == "error") {
				m_logLevel = G3LOG_ERROR;
			}
			else if (logLevel == "fatal") {
				m_logLevel = G3LOG_FATAL;
			}
		}

		delete doc;
		LordFree(pJSONData);
		LordDelete(pDataStream);
		return true;
	}
}


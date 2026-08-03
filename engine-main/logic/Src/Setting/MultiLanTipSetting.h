#pragma once
#include <map>
#include <string>
#include "Core.h"
#include "LanguageDef.h"
#include "Util/CSVReader.h"

using namespace LORD;

class MultiLanTipSetting
{
public:
	typedef std::map<int, MultiLanTipSetting*> MultiLanTipSettingMap;
	typedef std::map<std::string, std::string> TipMap;

	MultiLanTipSetting();
	~MultiLanTipSetting();

	static bool					loadSetting(bool isClient);
	static bool					loadMapSetting(bool isClient);
	static bool					unloadSetting();
	static std::string			getMessage(String& lanType, int nTipType, const char* args);

private:
	int				m_nTipId;
	TipMap			m_TipMap;

private:
	static bool loadFileToSetting(std::string& path, CsvReader* pReader);
	static std::string getMessage(String& lanType, int nTipType);
	static std::string formatMessage(String& lanType, std::string& msg, const char* args);
	static std::string getArgString(String& lanType, std::string& arg);

	static	MultiLanTipSettingMap ms_multiLanTipSettingMap;
	static  std::set<std::string> ms_languageSet;
};

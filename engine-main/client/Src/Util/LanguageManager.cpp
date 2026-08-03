#include "LanguageManager.h"
#include "Core.h"
#include "Util/PathUtil.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "LanguageDef.h"


using namespace LORD;

namespace BLOCKMAN
{
	LanguageManager::LanguageManager()
	{
	}

	LanguageManager::~LanguageManager()
	{
	}

	LanguageManager::LanguageManager(String rootPath) :
		m_rootPath(rootPath),
		m_currentLanguage("en_US")
	{
		loadLocaleData();
	}

	LanguageManager::LanguageManager(String rootPath, String lang) :
		m_rootPath(rootPath),
		m_currentLanguage(lang)
	{
		loadLocaleData();
	}

	void LanguageManager::loadLocaleData(String lang)
	{
		if (m_defaultLanguage == lang) { return; }
		this->m_currentLanguage = lang;
		loadLocaleData();
	}

	void LanguageManager::loadMapData(String mapPath, String worldName)
	{
		String dirPath = "";
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		dirPath = PathUtil::ConcatPath(mapPath, worldName, "lang");
#else
		dirPath = PathUtil::ConcatPath(worldName, "lang");
#endif
		ResourceGroupManager::Instance()->addResourceLocation(dirPath.c_str(), "FileSystem", "General");

		String path = PathUtil::ConcatPath(mapPath, worldName, "lang");
		if (!PathUtil::IsDirExist(path)) { return; }

		String oldLang = m_currentLanguage;
		m_currentLanguage = "en_US";
		String ext = "_map.lang";
		loadLangData(path, ext, m_langText);
		if (oldLang == m_currentLanguage) { return; }
		m_currentLanguage = oldLang;
		loadLangData(path, ext, m_langText);
	}

	void LanguageManager::loadLocaleData()
	{
		String ext = ".lang";
		String path = PathUtil::ConcatPath(m_rootPath, "Media/Lang/Text");
		loadLangData( path, ext, m_langText);
	}

	void LanguageManager::loadLangData(String& path, String& ext, map<String, String>::type& dataSet)
	{
		StringArray strs = StringUtil::Split(m_currentLanguage, "_");
		String currLanguageFamily = "";
		if (int(strs.size()) >= 1) {
			currLanguageFamily = strs[0];
		}

		/*if (currLanguageFamily == "zh") {
			currLanguageFamily = m_currentLanguage;
		}*/

		String langPath = PathUtil::ConcatPath(path , currLanguageFamily + ext);
		if (!PathUtil::IsFileExist(langPath))
		{
			currLanguageFamily = "en";
			LordLogError("langPath path [ %s ] does not exist, use default english", langPath.c_str());
		}
		
		DataStream* stream = ResourceGroupManager::Instance()->openResource((currLanguageFamily + ext).c_str());
		if (stream == NULL)
		{
			LordLogError("file not found [%s].", (currLanguageFamily + ext).c_str());
			return;
		}
		String line;
		while (!stream->eof())
		{
			line = stream->getLine();
			if (line.length() > 0 && line.at(0) != '#' && line.at(0) != '@')
			{
				StringArray array = StringUtil::Split(line, "=", 1);
				if (array.size() == 2)
				{
					String ori = array[1];
					String temp = StringUtil::Replace(ori, "\\\\n", "\n");
					dataSet[array[0]] = temp;
				}
			}
		}
		stream->close();
		LordDelete(stream);
	}



	String LanguageManager::getString(String key)
	{
		auto lang = m_langText.find(key);
		if (lang != m_langText.end())
		{
			return lang->second.length() > 0 ? lang->second : key;
		}
		return key;
	}

	String LanguageManager::getItemName(String key)
	{
		String keyName = key + ".name";
		auto lang = m_langText.find(keyName);
		if (lang != m_langText.end())
		{
			return lang->second.length() > 0 ? lang->second : key;
		}
		return key;
	}

	String LanguageManager::getCurrentLanguage()
	{
		return m_currentLanguage;
	}
}
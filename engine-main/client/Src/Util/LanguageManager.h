#pragma once

#ifndef __LANGUAGE_MANAGER_HEADER__
#define __LANGUAGE_MANAGER_HEADER__
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN {

	class LanguageManager : public Singleton<LanguageManager>, public ObjectAlloc
	{
	public:
		LanguageManager();
		~LanguageManager();
		LanguageManager(String  rootPath);
		LanguageManager(String  rootPath, String lang);
		void setRootPath(String rootPath) { this->m_rootPath = rootPath; }
		void setLanguage(String lang) { this->m_currentLanguage = lang; }
		void loadLocaleData(String lang);
		void loadMapData(String mapPath, String worldName);
		String getString(String key);
		String getItemName(String key);
		String getCurrentLanguage();

	private:
		void loadLocaleData();
		void loadLangData(String& path, String& ext, map<String, String>::type& dataSet);
	private:
		String m_rootPath;
		String m_currentLanguage;
		map<String, String>::type  m_langText;
		//map<String, String>::type  m_langImage;
		const String m_defaultLanguage = "en_us";
	};

}
#endif
/********************************************************************
filename: 	ConfigFile.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_CONFIGFILE_H__
#define __LORD_CONFIGFILE_H__

#include "Core.h"
#include "Util/StringUtil.h"

namespace LORD
{
	class LORD_CORE_API ConfigFile : public ObjectAlloc
	{
	public:

		ConfigFile();
		virtual ~ConfigFile();
		/// load from a filename (not using resource group locations)
		void load(const String& filename, const String& separators = "\t:=", bool trimWhitespace = true);
		/// load from a filename (using resource group locations)
		void load(const String& filename, const String& resourceGroup, const String& separators = "\t:=", bool trimWhitespace = true);
		/// load from a data stream
		void load(DataStream* stream, const String& separators = "\t:=", bool trimWhitespace = true);
		/// load from a filename (not using resource group locations)
		void loadDirect(const String& filename, const String& separators = "\t:=", bool trimWhitespace = true);
		/// load from a filename (using resource group locations)
		void loadFromResourceSystem(const String& filename, const String& resourceGroup, const String& separators = "\t:=", bool trimWhitespace = true);

		/** Gets the first setting from the file with the named key. 
		@param key The name of the setting
		@param section The name of the section it must be in (if any)
		@param defaultValue The value to return if the setting is not found
		*/
		String getSetting(const String& key, const String& section = StringUtil::BLANK, const String& defaultValue = StringUtil::BLANK) const;
		/** Gets all settings from the file with the named key. */
		StringArray getMultiSetting(const String& key, const String& section = StringUtil::BLANK) const;

		typedef multimap<String, String>::type SettingsMultiMap;
		//typedef map<String, SettingsMultiMap*>::type SettingsBySection;
		struct Settings
		{
			String name;
			SettingsMultiMap* setting;
		};

		/** Get an iterator over all the available settings in a section */
		const Settings* getAllSetting(size_t& num) const;

		/** Clear the settings */
		void clear(void);

	protected:
		Settings* findSettings(const String& name) const;

	protected:
		Settings* mSettings;
		size_t	  mSettingNum;
		size_t	  mCapacity;
	};
}

#endif

#include "Core.h"
#include "ConfigFile.h"
#include "Resource/ResourceGroupManager.h"

#include "Exception.h"

#include <iostream>

namespace LORD
{

	//-----------------------------------------------------------------------
	ConfigFile::ConfigFile()
		: mSettings(NULL)
		, mSettingNum(0)
		, mCapacity(16)
	{
		mSettings = LordNewArrayT(Settings, mCapacity);
	}
	//-----------------------------------------------------------------------
	ConfigFile::~ConfigFile()
	{
		for (size_t i = 0; i < mSettingNum; ++i)
		{
			LordDeleteT(mSettings[i].setting, SettingsMultiMap);
		}
		
		LordSafeDeleteArrayT(mSettings, Settings, mCapacity);
		mCapacity = 0;
		mSettingNum = 0;
	}
	//-----------------------------------------------------------------------
	void ConfigFile::clear(void)
	{
		for (size_t i = 0; i < mSettingNum; ++i)
		{
			LordDeleteT(mSettings[i].setting, SettingsMultiMap);
		}
		mSettingNum = 0;
	}
	//-----------------------------------------------------------------------
	void ConfigFile::load(const String& filename, const String& separators, bool trimWhitespace)
	{
		loadDirect(filename, separators, trimWhitespace);
	}
	//-----------------------------------------------------------------------
	void ConfigFile::load(const String& filename, const String& resourceGroup, 
		const String& separators, bool trimWhitespace)
	{
		loadFromResourceSystem(filename, resourceGroup, separators, trimWhitespace);
	}
	//-----------------------------------------------------------------------
	void ConfigFile::loadDirect(const String& filename, const String& separators, 
		bool trimWhitespace)
	{
		/* Open the configuration file */
		std::ifstream fp;
		// Always open in binary mode
		fp.open(filename.c_str(), std::ios::in | std::ios::binary);
		if(!fp)
		{
			LordException("Error: %s file not found!", filename.c_str());
		}

		// Wrap as a stream
		DataStream* stream = LordNew FileStreamDataStream(filename, &fp, false);

		load(stream, separators, trimWhitespace);
		LordSafeDelete(stream);
	}
	//-----------------------------------------------------------------------
	void ConfigFile::loadFromResourceSystem(const String& filename, 
		const String& resourceGroup, const String& separators, bool trimWhitespace)
	{
		DataStream* stream = 
			ResourceGroupManager::Instance()->openResource(filename, resourceGroup);
		load(stream, separators, trimWhitespace);
	}
	//-----------------------------------------------------------------------
	void ConfigFile::load(DataStream* stream, const String& separators, 
		bool trimWhitespace)
	{
		/* Clear current settings map */
		clear();

		String currentSection = StringUtil::BLANK;
		SettingsMultiMap* currentSettings = LordNewT(SettingsMultiMap);
		mSettings[mSettingNum].setting = currentSettings;
		mSettings[mSettingNum].name = currentSection;
		mSettingNum++;


		/* Process the file line for line */
		String line, optName, optVal;
		while (!stream->eof())
		{
			line = stream->getLine();
			/* Ignore comments & blanks */
			if (line.length() > 0 && line.at(0) != '#' && line.at(0) != '@')
			{
				if (line.at(0) == '[' && line.at(line.length()-1) == ']')
				{
					// Section
					currentSection = line.substr(1, line.length() - 2);
					Settings* seci = findSettings(currentSection);
					if (seci == NULL)
					{
						currentSettings = LordNewT(SettingsMultiMap);

						mSettings[mSettingNum].name = currentSection;
						mSettings[mSettingNum].setting = currentSettings;
						mSettingNum++;
						// need copy
						if (mSettingNum == mCapacity)
						{
							Settings* newSettings = LordNewArrayT(Settings, mCapacity * 2);
							for (size_t i = 0; i < mSettingNum; ++i)
							{
								newSettings[i].name = mSettings[i].name;
								newSettings[i].setting = mSettings[i].setting;
							}
							LordSafeDeleteArrayT(mSettings, Settings, mCapacity);
							mCapacity = mCapacity * 2;
							mSettings = newSettings;
						}
					}
					else
					{
						currentSettings = seci->setting;
					}
				}
				else
				{
					/* Find the first seperator character and split the string there */
					String::size_type separator_pos = line.find_first_of(separators, 0);
					if (separator_pos != String::npos)
					{
						optName = line.substr(0, separator_pos);
						/* Find the first non-seperator character following the name */
						String::size_type nonseparator_pos = line.find_first_not_of(separators, separator_pos);
						/* ... and extract the value */
						/* Make sure we don't crash on an empty setting (it might be a valid value) */
						optVal = (nonseparator_pos == String::npos) ? "" : line.substr(nonseparator_pos);
						if (trimWhitespace)
						{
							StringUtil::Trim(optVal);
							StringUtil::Trim(optName);
						}
						currentSettings->insert(SettingsMultiMap::value_type(optName, optVal));
					}
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	String ConfigFile::getSetting(const String& key, const String& section, const String& defaultValue) const
	{

		Settings* seci = findSettings(section);
		if (seci == NULL)
		{
			return defaultValue;
		}
		else
		{
			SettingsMultiMap::const_iterator i = seci->setting->find(key);
			if (i == seci->setting->end())
			{
				return defaultValue;
			}
			else
			{
				return i->second;
			}
		}
	}
	//-----------------------------------------------------------------------
	StringArray ConfigFile::getMultiSetting(const String& key, const String& section) const
	{
		StringArray ret;


		Settings* seci = findSettings(section);
		if (seci != NULL)
		{
			SettingsMultiMap::const_iterator i;

			i = seci->setting->find(key);
			// Iterate over matches
			while (i != seci->setting->end() && i->first == key)
			{
				ret.push_back(i->second);
				++i;
			}
		}
		return ret;


	}
	//-----------------------------------------------------------------------
	ConfigFile::Settings* ConfigFile::findSettings(const String& name) const
	{
		for (size_t i = 0; i < mSettingNum; ++i)
		{
			if (mSettings[i].name == name)
				return mSettings + i;
		}
		return NULL;
	}
	//-----------------------------------------------------------------------
	const ConfigFile::Settings* ConfigFile::getAllSetting(size_t& num) const
	{
		num = mSettingNum;
		return mSettings;
	}

}

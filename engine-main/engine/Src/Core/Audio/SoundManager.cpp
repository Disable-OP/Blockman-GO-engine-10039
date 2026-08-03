#include "SoundManager.h"
#include "Object/Root.h"
#include "Util/CSVReader.h"
#include "SoundDef.h"

namespace LORD
{
	 const char SoundManager::SOUND_ROOT_PATH[] = "Media/Sound";
	 const char SoundManager::SOUND_FILE_EXT[] = ".mp3";

	void SoundManager::scanSounds()
	{
		const static String soundRoot = PathUtil::ConcatPath(Root::Instance()->getRootPath(), SOUND_ROOT_PATH);
		StringArray soundFiles;
		PathUtil::EnumFilesInDir(soundFiles, soundRoot, false, true, true);
		for (const auto& file : soundFiles)
		{
			if (!StringUtil::EndWith(file, SOUND_FILE_EXT))
			{
				LordLogWarning("found unsupported sound file: %s", file.c_str());
				continue;
			}
			auto index = file.find(SOUND_ROOT_PATH);
			if (index == String::npos)
			{
				LordLogWarning("file is not under sound directory: %s", file.c_str());
				continue;
			}
			int startIndex = index + strlen(SOUND_ROOT_PATH) + 1;
			int endIndex = file.size() - strlen(SOUND_FILE_EXT);
			const auto& relativePathWithoutExt = file.substr(startIndex, endIndex - startIndex);
			auto soundName = StringUtil::Replace(relativePathWithoutExt, '/', '.');
			soundName = StringUtil::Replace(soundName, '\\', '.');
			if (std::isdigit(soundName[soundName.size() - 1]))
			{
				StringUtil::Trim(soundName, false, true, "0123456789");
			}
			m_nameToSoundSetMap[soundName].sounds.push_back(file);
		}
	}

	String SoundManager::getRandomSoundFile(const String & soundName)
	{
		auto iter = m_nameToSoundSetMap.find(soundName);
		if (iter == m_nameToSoundSetMap.end())
		{
			LordLogWarning("sound %s is not found", soundName.c_str());
			return {};
		}
		const auto& soundSet = iter->second;
		auto randomIndex = std::uniform_int_distribution<>(0, soundSet.sounds.size() - 1)(m_rand);
		return soundSet.sounds[randomIndex];
	}

	SoundSetting::SoundSettingMap SoundSetting::ms_soundSettingMap = {};

	SoundSetting::SoundSetting()
	{
	}

	SoundSetting::~SoundSetting()
	{
	}

	bool SoundSetting::loadSetting()
	{
		SoundSetting* pSetting = NULL;
		ms_soundSettingMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/sound.csv")).c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open SoundSetting file %s", path.c_str());
			return false;
		}

		while (pReader->Read())
		{
			pSetting = new SoundSetting();
			pReader->GetInt("SoundId", pSetting->nSoundId);

			if (pSetting->nSoundId > (int)ST_Invalid)
			{
				pReader->GetBool("IsRepeat", pSetting->bIsRepeat);
				pReader->GetFloat("RepeatInterval_min", pSetting->fMinInterval);
				pReader->GetFloat("RepeatInterval_max", pSetting->fMaxInterval);
				std::string str;
				pReader->GetString("SoundFile", str);
				pSetting->sSoundPath = StringUtil::RemoveCharactors(str.c_str(), ' '); // remove empty space in path		
				pSetting->InitSuffix();

				ms_soundSettingMap[pSetting->nSoundId] = pSetting;
			}
			else
			{
				delete pSetting;
			}
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	bool SoundSetting::unloadSetting()
	{
		for (auto iter = ms_soundSettingMap.begin(); iter != ms_soundSettingMap.end(); iter++)
		{
			SoundSetting* pSetting = iter->second;
			delete pSetting;
		}

		ms_soundSettingMap.clear();
		return true;
	}

	SoundSetting* SoundSetting::getSoundSetting(int soundId)
	{
		auto iter = ms_soundSettingMap.find(soundId);
		if (iter != ms_soundSettingMap.end())
		{
			return iter->second;
		}

		return NULL;
	}

	void SoundSetting::testSetting()
	{
		for (int i = 1; i <= 10; i++)
		{
			SoundSetting* pSetting = getSoundSetting(i);
			if (pSetting)
				pSetting->printSetting();
		}
	}

	void SoundSetting::printSetting()
	{
		LordLogInfo("SoundSetting: Id[%d]; IsRepeat[%d]; minInterval[%f]; maxInterval[%f] SoundPath[%s]", nSoundId, bIsRepeat, fMinInterval, fMaxInterval, sSoundPath.c_str());
	}

	bool SoundSetting::InitSuffix()
	{
		String	sFirst;
		bool	bResult			= false;
		int	    nStartIndex		= 1;

		m_suffixes.clear();
		if (PathUtil::IsFileExist(PathUtil::ConcatPath(Root::Instance()->getRootPath(), sSoundPath + m_SoundSuffix)))
		{
			bResult = true;
			m_suffixes.push_back("");
		}

		while (PathUtil::IsFileExist(PathUtil::ConcatPath(Root::Instance()->getRootPath(), sSoundPath + StringUtil::ToString(nStartIndex) + m_SoundSuffix)))
		{
			bResult = true;
			m_suffixes.push_back(StringUtil::ToString(nStartIndex));
			nStartIndex++;
		}

		return bResult;
	}

	String SoundSetting::GetRandomPath()
	{
		if (m_suffixes.size() < 1)
		{
			LordLogWarning("soundPath %s is not found", PathUtil::ConcatPath(Root::Instance()->getRootPath(), sSoundPath).c_str());
			return {};
		}

		int randomIndex = std::uniform_int_distribution<>(0, m_suffixes.size() - 1)(m_rand);
		String suffix = m_suffixes[randomIndex];
		return PathUtil::ConcatPath(Root::Instance()->getRootPath(), sSoundPath + suffix + m_SoundSuffix);
	}
}

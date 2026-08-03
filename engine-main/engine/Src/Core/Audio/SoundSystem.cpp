#include "SoundSystem.h"
#include "SoundEngine.h"
#include "Core.h"
#include "Object/Root.h"

namespace LORD
{
	SoundSystem::SoundSystem()
		: m_pSoundEngine(NULL)
		, m_soundSwitch(true)
	{
		m_soundManager.scanSounds();
	}

	SoundSystem::~SoundSystem()
	{
	}

	/*SoundSystem* SoundSystem::Instance()
	{
		return ms_pSingleton;
	}*/

	String SoundSystem::getFilePath(const String & name)
	{
		return m_soundManager.getRandomSoundFile(name);
	}

	void SoundSystem::preloadBackgroundMusic(const String& soundName)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (PathUtil::IsFileExist(filePath))
			{
				m_pSoundEngine->preloadBackgroundMusic(filePath);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
			}
		}
	}

	void SoundSystem::playBackgroundMusic(const String& soundName, bool bLoop)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (PathUtil::IsFileExist(filePath))
			{
				m_pSoundEngine->playBackgroundMusic(filePath, bLoop, 0, 0);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
			}
		}

		if (m_soundSwitch == false || m_backgroundMusicSwitch == false)
		{
			pauseBackgroundMusic();
		}
	}

	void SoundSystem::stopBackgroundMusic( bool bReleaseData)
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopBackgroundMusic();
			m_pSoundEngine->stopBackgroundMusic(bReleaseData);
		}
	}

	void SoundSystem::pauseBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseBackgroundMusic();
		}
	}

	void SoundSystem::resumeBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeBackgroundMusic();
		}
	}

	void SoundSystem::rewindBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->rewindBackgroundMusic();
		}
	}

	bool SoundSystem::willPlayBackgroundMusic()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->willPlayBackgroundMusic();
		}

		return false;
	}

	bool SoundSystem::isBackgroundMusicPlaying()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->isBackgroundMusicPlaying();
		}

		return false;
	}

	float SoundSystem::getBackgroundMusicVolume()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->getBackgroundMusicVolume();
		}

		return 0.0f;
	}

	void SoundSystem::setBackgroundMusicVolume( float volume )
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->setBackgroundMusicVolume(volume);
		}
	}

	float SoundSystem::getEffectsVolume()
	{
		if (m_pSoundEngine)
		{
			return m_pSoundEngine->getEffectsVolume();
		}

		return 0.0f;
	}

	void SoundSystem::setEffectsVolume( float volume )
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->setEffectsVolume(volume);
		}
	}

	unsigned int SoundSystem::playEffectByType(SoundType soundType)
	{
		if (m_soundPause)
			return 0;

		if (soundType <= ST_Invalid || soundType >= ST_Total)
			return 0;

		SoundSetting* pSetting = SoundSetting::getSoundSetting((int)soundType);
		if (!pSetting)
		{
			LordLogWarning("SoundType [%d] does not exist!!!", (int)soundType);
			return 0;
		}

		String filePath = pSetting->GetRandomPath();
		if (PathUtil::IsFileExist(filePath))
		{
			if (soundType >= ST_BgmStart)
			{
				m_pSoundEngine->playBackgroundMusic(filePath, pSetting->bIsRepeat, pSetting->fMinInterval, pSetting->fMaxInterval);
			}
			else
			{
				return m_pSoundEngine->playEffect(filePath, pSetting->bIsRepeat, pSetting->fMinInterval, pSetting->fMaxInterval);
			}
		}
		else
		{
			LordLogWarning("sound file of sound type %d is not found", (int)soundType);
		}

		return 0;
	}

	void SoundSystem::pauseEffect( unsigned int nSoundId )
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseEffect(nSoundId);
		}
	}

	void SoundSystem::pauseAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->pauseAllEffects();
		}
	}

	void SoundSystem::resumeEffect( unsigned int nSoundId )
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeEffect(nSoundId);
		}
	}

	void SoundSystem::resumeAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->resumeAllEffects();
		}
	}

	void SoundSystem::stopEffect( unsigned int nSoundId )
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopEffect(nSoundId);
		}
	}

	void SoundSystem::stopAllEffects()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->stopAllEffects();
		}
	}

	void SoundSystem::preloadEffect(const String& soundName)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (PathUtil::IsFileExist(filePath))
			{
				m_pSoundEngine->preloadEffect(filePath);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
			}
		}
	}

	void SoundSystem::unloadEffect(const String& soundName)
	{
		if (m_pSoundEngine)
		{
			auto filePath = getFilePath(soundName);
			if (PathUtil::IsFileExist(filePath))
			{
				m_pSoundEngine->unloadEffect(filePath);
			}
			else
			{
				LordLogWarning("sound file is not found: %s", filePath.c_str());
			}
		}
	}

	void SoundSystem::EndSoundEngine()
	{
		if (m_pSoundEngine)
		{
			m_pSoundEngine->end();
		}
	}

}//LORD namespace end.

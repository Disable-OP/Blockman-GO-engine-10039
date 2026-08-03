/********************************************************************
filename: 	SoundSystem.h
file path:	dev\engine\Src\Core\Audio\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef  _SOUNDSYSTEM_H_
#define  _SOUNDSYSTEM_H_

#include "Core.h"
#include "Object/Singleton.h"
#include "Resource/ResourceGroupManager.h"
#include "SoundManager.h"
#include "SoundDef.h"

namespace LORD
{
	class SoundEngine;
	class LORD_CORE_API SoundSystem : public Singleton<SoundSystem>, public ObjectAlloc
	{
	public:
		SoundSystem();
		~SoundSystem();

		inline static SoundSystem* Instance()
		{
			return ms_pSingleton;
		}

		void SetSoundEngine(SoundEngine* pSoundEngine) { m_pSoundEngine = pSoundEngine; }

		void EndSoundEngine();

		void setSoundOn(bool setting) {

				if (setting)
				{
					resumeAllEffects();
					resumeBackgroundMusic();
				} 
				else
				{
					pauseBackgroundMusic();
					pauseAllEffects();
				}

				m_soundSwitch = setting;  
			}

		void setSoundPause(bool bPause) { m_soundPause = bPause; }

		bool isSoundOn() const { return m_soundSwitch; }

		void setBackgroundMusicSwitch(bool isOn) {
			if (isOn)
			{
				resumeBackgroundMusic();
			}
			else
			{
				pauseBackgroundMusic();
			}
			m_backgroundMusicSwitch = isOn;
		}

		bool isSwitchOnBackgroundMusic() {
			return m_backgroundMusicSwitch;
		}

	protected:
		SoundEngine*		m_pSoundEngine;
		SoundManager		m_soundManager;

		bool				m_soundSwitch;
		bool                m_soundPause = false;
		bool				m_backgroundMusicSwitch = true;
		String getFilePath(const String& name);

	public:
		/**
		 @brief Preload background music
		 @param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
		 */
		void preloadBackgroundMusic(const String& soundName);
    
		/**
		@brief Play background music
		@param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
		@param bLoop Whether the background music loop or not
		*/
		void playBackgroundMusic(const String& soundName, bool bLoop);
		void playBackgroundMusic(const String& soundName) {
    		this->playBackgroundMusic(soundName, false);
		}

		/**
		@brief Stop playing background music
		@param bReleaseData If release the background music data or not.As default value is false
		*/
		void stopBackgroundMusic(bool bReleaseData);
		void stopBackgroundMusic() {
    		this->stopBackgroundMusic(false);
		}

		/**
		@brief Pause playing background music
		*/
		void pauseBackgroundMusic();

		/**
		@brief Resume playing background music
		*/
		void resumeBackgroundMusic();

		/**
		@brief Rewind playing background music
		*/
		void rewindBackgroundMusic();

		bool willPlayBackgroundMusic();

		/**
		@brief Whether the background music is playing
		@return If is playing return true,or return false
		*/
		bool isBackgroundMusicPlaying();

		// properties
		/**
		@brief The volume of the background music max value is 1.0,the min value is 0.0
		*/
		float getBackgroundMusicVolume();

		/**
		@brief set the volume of background music
		@param volume must be in 0.0~1.0
		*/
		void setBackgroundMusicVolume(float volume);

		/**
		@brief The volume of the effects max value is 1.0,the min value is 0.0
		*/
		float getEffectsVolume();

		/**
		@brief set the volume of sound effecs
		@param volume must be in 0.0~1.0
		*/
		void setEffectsVolume(float volume);

		// for sound effects
		// 直接通过音效配置ID来播放声音
		unsigned int playEffectByType(SoundType soundType);

		/**
		@brief Pause playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void pauseEffect(unsigned int nSoundId);

		/**
		@brief Pause all playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void pauseAllEffects();

		/**
		@brief Resume playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void resumeEffect(unsigned int nSoundId);

		/**
		@brief Resume all playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void resumeAllEffects();

		/**
		@brief Stop playing sound effect
		@param nSoundId The return value of function playEffect
		*/
		void stopEffect(unsigned int nSoundId);

		/**
		@brief Stop all playing sound effects
		*/
		void stopAllEffects();

		/**
		@brief          preload a compressed audio file
		@details        the compressed audio will be decode to wave, then write into an 
		internal buffer in SimpleaudioEngine
		*/
		void preloadEffect(const String& soundName);

		/**
		@brief          unload the preloaded effect from internal buffer
		@param[in]        pszFilePath        The path of the effect file,or the FileName of T_SoundResInfo
		*/
		void unloadEffect(const String& soundName);
	};
}

#endif

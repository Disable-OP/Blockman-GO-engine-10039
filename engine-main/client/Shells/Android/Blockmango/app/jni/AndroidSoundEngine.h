#ifndef _ANDROID_SOUND_ENGINE_H_
#define _ANDROID_SOUND_ENGINE_H_

#include "Core.h"
#include "Audio/SoundEngine.h"

namespace LORD
{
	class AndroidSoundEngine:public SoundEngine
	{
	public:
		AndroidSoundEngine();
		virtual ~AndroidSoundEngine();

		virtual void end();
		/**
		 @brief Preload background music
		 @param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
		 */
		void preloadBackgroundMusic(String strFilePath);
		/**
		@brief Play background music
		@param pszFilePath The path of the background music file,or the FileName of T_SoundResInfo
		@param bLoop Whether the background music loop or not
		*/
		void playBackgroundMusic(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval);
		/**
		@brief Stop playing background music
		@param bReleaseData If release the background music data or not.As default value is false
		*/
		void stopBackgroundMusic();
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
		/**
		@brief Play sound effect
		@param pszFilePath The path of the effect file,or the FileName of T_SoundResInfo
		@bLoop Whether to loop the effect playing, default value is false
		*/
		unsigned int playEffect(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval);

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
		void preloadEffect(String strFilePath);

		/**
		@brief          unload the preloaded effect from internal buffer
		@param[in]        pszFilePath        The path of the effect file,or the FileName of T_SoundResInfo
		*/
		void unloadEffect(String strFilePath);
	};
}
#endif /* SOUND_H_ */

#include "Win32SoundEngine.h"
#include "SimpleAudioEngine.h"

namespace LORD
{


	void Win32SoundEngine::preloadBackgroundMusic( String strFilePath )
	{
		Lord::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(strFilePath.c_str());
	}

	void Win32SoundEngine::playBackgroundMusic(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval)
	{
		Lord::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(strFilePath.c_str(), bLoop, fMinInterval, fMaxInterval);
	}

	void Win32SoundEngine::stopBackgroundMusic( bool bReleaseData )
	{
		Lord::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(bReleaseData);
	}

	void Win32SoundEngine::pauseBackgroundMusic()
	{
		Lord::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}

	void Win32SoundEngine::resumeBackgroundMusic()
	{
		Lord::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	}

	void Win32SoundEngine::rewindBackgroundMusic()
	{
		Lord::SimpleAudioEngine::sharedEngine()->rewindBackgroundMusic();
	}

	bool Win32SoundEngine::willPlayBackgroundMusic()
	{
		return Lord::SimpleAudioEngine::sharedEngine()->willPlayBackgroundMusic();
	}

	bool Win32SoundEngine::isBackgroundMusicPlaying()
	{
		return Lord::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
	}

	float Win32SoundEngine::getBackgroundMusicVolume()
	{
		return Lord::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	}

	void Win32SoundEngine::setBackgroundMusicVolume( float volume )
	{
		Lord::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume);
	}

	float Win32SoundEngine::getEffectsVolume()
	{
		return Lord::SimpleAudioEngine::sharedEngine()->getEffectsVolume();
	}

	void Win32SoundEngine::setEffectsVolume( float volume )
	{
		Lord::SimpleAudioEngine::sharedEngine()->setEffectsVolume(volume);
	}

	unsigned int Win32SoundEngine::playEffect(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval)
	{
		return Lord::SimpleAudioEngine::sharedEngine()->playEffect(strFilePath.c_str(), bLoop, fMinInterval, fMaxInterval);
	}

	void Win32SoundEngine::pauseEffect( unsigned int nSoundId )
	{
		Lord::SimpleAudioEngine::sharedEngine()->pauseEffect(nSoundId);
	}

	void Win32SoundEngine::pauseAllEffects()
	{
		Lord::SimpleAudioEngine::sharedEngine()->pauseAllEffects();
	}

	void Win32SoundEngine::resumeEffect( unsigned int nSoundId )
	{
		Lord::SimpleAudioEngine::sharedEngine()->resumeEffect(nSoundId);
	}

	void Win32SoundEngine::resumeAllEffects()
	{
		Lord::SimpleAudioEngine::sharedEngine()->resumeAllEffects();
	}

	void Win32SoundEngine::stopEffect( unsigned int nSoundId )
	{
		Lord::SimpleAudioEngine::sharedEngine()->stopEffect(nSoundId);
	}

	void Win32SoundEngine::stopAllEffects()
	{
		Lord::SimpleAudioEngine::sharedEngine()->stopAllEffects();
	}

	void Win32SoundEngine::preloadEffect( String strFilePath )
	{
		Lord::SimpleAudioEngine::sharedEngine()->preloadEffect(strFilePath.c_str());
	}

	void Win32SoundEngine::unloadEffect( String strFilePath )
	{
		Lord::SimpleAudioEngine::sharedEngine()->unloadEffect(strFilePath.c_str());
	}

	Win32SoundEngine::Win32SoundEngine()
	{

	}

	Win32SoundEngine::~Win32SoundEngine()
	{

	}

	void Win32SoundEngine::end()
	{
		Lord::SimpleAudioEngine::end();
	}

}//LORD namespace end.

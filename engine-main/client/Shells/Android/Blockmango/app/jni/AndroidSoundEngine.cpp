/*
 * Sound.cpp
 *
 *  Created on: 2013-11-13
 *      Author: DevUser
 */


#include "AndroidSoundEngine.h"
#include "JniHelper.h"
#include "SimpleAudioEngineJni.h"
#include <android/log.h>

#define  LOG_TAG     "Device Model"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define  CLASS_NAME   "com/sandboxol/blockmango/EchoesHelper"
#define  METHOD_NAME  "getDeviceModel"

namespace LORD
{
	AndroidSoundEngine::AndroidSoundEngine()
	{
	    JniMethodInfo methodInfo;
	    jstring jstr;
	    if (JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, METHOD_NAME, "()Ljava/lang/String;"))
	    {
	        jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
	    }
	    methodInfo.env->DeleteLocalRef(methodInfo.classID);

	    const char* deviceModel = methodInfo.env->GetStringUTFChars(jstr, NULL);

	    LOGD("SimpleAudioEngine() - deviceModel = %s", deviceModel);

	    methodInfo.env->ReleaseStringUTFChars(jstr, deviceModel);
	    methodInfo.env->DeleteLocalRef(jstr);
	}
	AndroidSoundEngine::~AndroidSoundEngine()
	{

	}

	void AndroidSoundEngine::end()
	{
	    endJNI();
	}

	void AndroidSoundEngine::preloadBackgroundMusic(String strFilePath)
	{
	    preloadBackgroundMusicJNI(strFilePath.c_str());
	}

	void AndroidSoundEngine::playBackgroundMusic(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval)
	{
	    playBackgroundMusicJNI(strFilePath.c_str(), bLoop, fMinInterval, fMaxInterval);
	}

	void AndroidSoundEngine::stopBackgroundMusic()
	{
	    stopBackgroundMusicJNI();
	}

	void AndroidSoundEngine::pauseBackgroundMusic()
	{
	    pauseBackgroundMusicJNI();
	}

	void AndroidSoundEngine::resumeBackgroundMusic()
	{
	    resumeBackgroundMusicJNI();
	}

	void AndroidSoundEngine::rewindBackgroundMusic()
	{
	    rewindBackgroundMusicJNI();
	}

	bool AndroidSoundEngine::willPlayBackgroundMusic()
	{
	    return true;
	}

	bool AndroidSoundEngine::isBackgroundMusicPlaying()
	{
	    return isBackgroundMusicPlayingJNI();
	}

	float AndroidSoundEngine::getBackgroundMusicVolume()
	{
	    return getBackgroundMusicVolumeJNI();
	}

	void AndroidSoundEngine::setBackgroundMusicVolume(float volume)
	{
	    setBackgroundMusicVolumeJNI(volume);
	}

	float AndroidSoundEngine::getEffectsVolume()
	{
	    return getEffectsVolumeJNI();
	}

	void AndroidSoundEngine::setEffectsVolume(float volume)
	{
	    setEffectsVolumeJNI(volume);
	}

	unsigned int AndroidSoundEngine::playEffect(String strFilePath, bool bLoop, float fMinInterval, float fMaxInterval)
	{
	    return playEffectJNI(strFilePath.c_str(), bLoop, fMinInterval, fMaxInterval);
	}

	void AndroidSoundEngine::stopEffect(unsigned int nSoundId)
	{
	    stopEffectJNI(nSoundId);
	}

	void AndroidSoundEngine::preloadEffect(String strFilePath)
	{
	    preloadEffectJNI(strFilePath.c_str());
	}

	void AndroidSoundEngine::unloadEffect(String strFilePath)
	{
	    unloadEffectJNI(strFilePath.c_str());
	}

	void AndroidSoundEngine::pauseEffect(unsigned int nSoundId)
	{
	    pauseEffectJNI(nSoundId);
	}

	void AndroidSoundEngine::pauseAllEffects()
	{
	    pauseAllEffectsJNI();
	}

	void AndroidSoundEngine::resumeEffect(unsigned int nSoundId)
	{
	    resumeEffectJNI(nSoundId);
	}

	void AndroidSoundEngine::resumeAllEffects()
	{
	    resumeAllEffectsJNI();
	}

	void AndroidSoundEngine::stopAllEffects()
	{
	    stopAllEffectsJNI();
	}
}

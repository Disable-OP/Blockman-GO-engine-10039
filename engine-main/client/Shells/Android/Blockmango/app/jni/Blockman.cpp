#include <jni.h>
#include <android/log.h>
#include "Blockman.h"
#include "game.h"
#include "Core.h"
#include "AndroidSoundEngine.h"
#include "ShellInterfaceAndroid.h"
#include "Audio/SoundEngine.h"
#include "Audio/SoundSystem.h"

using namespace GameClient;

CGame g_Game;
bool g_isInit = false;

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeTouchesBegin
		(JNIEnv* env, jclass c, jint nIndex, jfloat fX, jfloat fY)
{

	if(!g_isInit)
	{
		return;
	}
	intptr_t id = 0;
	id = nIndex;
	g_Game.handleTouchesBegin(1, &id, &fX, &fY);
}

/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeTouchesEnd
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeTouchesEnd
		(JNIEnv* env, jclass c, jint nIndex, jfloat fX, jfloat fY)
{
	if(!g_isInit)
	{
		return;
	}

	intptr_t id = 0;
	id = nIndex;
	g_Game.handleTouchesEnd(1, &id, &fX, &fY);
}

/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeTouchesMove
 * Signature: ([I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeTouchesMove
		(JNIEnv* env, jclass c, jintArray nIndexArray, jfloatArray fXArray, jfloatArray fYArray)
{
	if(!g_isInit)
	{
		return;
	}

	int size = env->GetArrayLength(nIndexArray);
	jint id[size];
	jfloat x[size];
	jfloat y[size];

	env->GetIntArrayRegion(nIndexArray, 0, size, id);
	env->GetFloatArrayRegion(fXArray, 0, size, x);
	env->GetFloatArrayRegion(fYArray, 0, size, y);


    intptr_t *ids = new intptr_t[size]{0};
    for (int i = 0; i < size; i++)
    {
        ids[i] = id[i];
    }
    g_Game.handleTouchesMove(size, ids, x, y);
    delete[] ids;
}


/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeTouchesCancel
 * Signature: ([I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeTouchesCancel
		(JNIEnv* env, jclass c, jintArray nIndexArray, jfloatArray fXArray, jfloatArray fYArray)
{
	if(!g_isInit)
	{
		return;
	}

	int size = env->GetArrayLength(nIndexArray);
	jint id[size];
	jfloat x[size];
	jfloat y[size];

	env->GetIntArrayRegion(nIndexArray, 0, size, id);
	env->GetFloatArrayRegion(fXArray, 0, size, x);
	env->GetFloatArrayRegion(fYArray, 0, size, y);

    intptr_t *ids = new intptr_t[size]{0};
	for(int i=0; i<size; i++)
	{
		ids[i]=id[i];
	}
	g_Game.handleTouchesCancel(size, ids, x, y);
    delete[] ids;
}

/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeKeyDown
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeKeyDown
		(JNIEnv *, jclass, jint key)
{
    if(!g_isInit)
        return false;
    g_Game.handleKeyDown(key);
	return true;
}


/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeKeyUp
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeKeyUp
        (JNIEnv *, jclass, jint key)
{

    if(!g_isInit)
        return false;
    g_Game.handleKeyUp(key);
    return true;
}

/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeRender
		(JNIEnv *, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.MainTick();
}


/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeInit
 * Signature: (II)V
 */

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_netInit
	(jstring ip, jshort port)
{

}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeInit
		(JNIEnv *env, jclass c, jfloat displayDensity, jstring s, jstring config, jstring mapPath, jint w, jint h)
{
	if (!g_isInit)
	{
		const char* strRootPath = (char *)env->GetStringUTFChars(s, 0);
		const char* strConfigPath = (char*)env->GetStringUTFChars(config, 0);
		const char* strMapPath = (char*)env->GetStringUTFChars(mapPath, 0);

		LORD::SoundEngine* pSoundEngine = new LORD::AndroidSoundEngine;
		GameClient::ShellInterfaceAndroid* pShellInterface = new GameClient::ShellInterfaceAndroid;

		// init game
		g_Game.setShellInterface(pShellInterface);

		g_Game.setPlatformInfo("test");
		// init game
		g_Game.InitGame(strRootPath, strConfigPath, w, h, 0, pSoundEngine, strMapPath);
        g_Game.setDisplayDensity(displayDensity);
		env->ReleaseStringUTFChars(s, strRootPath);
		env->ReleaseStringUTFChars(config, strConfigPath);
		g_isInit = true;
	}
}


JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeInitGame
		(JNIEnv *env, jclass c, jfloat displayDensity, jstring nickName, jlong userId, jstring token, jstring ip, jint port, jlong gameTimestamp, jstring lang, jstring gType,jstring mapName, jstring mapUrl,jstring s, jstring config, jstring mapPath, jint w, jint h, jint isMultiPlayer)
{
	if (!g_isInit)
	{
		const char* userNickName = (char*)env->GetStringUTFChars(nickName, 0);
		const char* userToke = (char*)env->GetStringUTFChars(token, 0);
		const char* gameIp = (char*)env->GetStringUTFChars(ip, 0);
		const char* platformLang = (char*)env->GetStringUTFChars(lang, 0);
        const char* gameMapName = (char*)env->GetStringUTFChars(mapName, 0);
        const char* gameMapUrl = (char*)env->GetStringUTFChars(mapUrl, 0);
        const char* gameType = (char*)env->GetStringUTFChars(gType, 0);

		const char* strRootPath = (char *)env->GetStringUTFChars(s, 0);
		const char* strConfigPath = (char*)env->GetStringUTFChars(config, 0);
		const char* strMapPath = (char*)env->GetStringUTFChars(mapPath, 0);

		LORD::SoundEngine* pSoundEngine = new LORD::AndroidSoundEngine;
		GameClient::ShellInterfaceAndroid* pShellInterface = new GameClient::ShellInterfaceAndroid;
		// init game
		g_Game.setShellInterface(pShellInterface);

		g_Game.setPlatformInfo("test");
		// init game
        g_Game.InitGame(userNickName, (ui64) userId, userToke, gameIp, port, (ui64) gameTimestamp, platformLang, gameType,
                        gameMapName, gameMapUrl, strRootPath, strConfigPath, w, h, 0, isMultiPlayer, pSoundEngine, strMapPath);
        g_Game.setDisplayDensity(displayDensity);
		env->ReleaseStringUTFChars(s, strRootPath);
		env->ReleaseStringUTFChars(config, strConfigPath);
		g_isInit = true;
	}
}

/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeOnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnSurfaceChanged
		(JNIEnv *env, jclass c, jint width, jint height)
{
	if(!g_isInit)
	{
		return;
	}
	g_Game.OnSize(width, height);
}


/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeOnPause
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnPause
		(JNIEnv *, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.OnPause();
}


/*
 * Class:     com_sandboxol_blockmango_EchoesRenderer
 * Method:    nativeOnResume
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnResume
		(JNIEnv *, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.OnResume();
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeCheckVersion
		(JNIEnv *env, jclass, jstring s)
{
	const char * strRootPath = (char *)env->GetStringUTFChars(s, 0);
	int result = g_Game.CheckVersion(strRootPath);
	env->ReleaseStringUTFChars(s, strRootPath);
	return result;
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeUpdateFiles
		(JNIEnv *env, jclass)
{
	return g_Game.UpdateFiles();
}

JNIEXPORT jfloat JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetDownloadPercent
		(JNIEnv *env, jclass)
{
	return g_Game.GetUpdatePercent();
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetDownloadState
		(JNIEnv *env, jclass)
{
	return g_Game.GetUpdateState();
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnGetPhoneType
		(JNIEnv *env, jclass, jstring s)
{
	const char * strType = (char *)env->GetStringUTFChars(s, 0);
	g_Game.GetPhoneType(strType);
	env->ReleaseStringUTFChars(s, strType);
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetTotalDownloadSize
		(JNIEnv *env, jclass)
{
	return g_Game.GetTotalDownloadSize();
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetCurrentDownloadSize
		(JNIEnv *env, jclass)
{
	return g_Game.GetCurrentDownloadSize();
}

JNIEXPORT jstring JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetLocalVersion
		(JNIEnv *env, jclass)
{
	return env->NewStringUTF(g_Game.getLocalVersion().c_str());
}

JNIEXPORT jstring JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeGetServerVersion
		(JNIEnv *env, jclass)
{
	return env->NewStringUTF(g_Game.getServerVersion().c_str());
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeUseProp(JNIEnv *env, jclass type, jstring propId_) {
	const char *propId = env->GetStringUTFChars(propId_, 0);
	env->ReleaseStringUTFChars(propId_, propId);
	if (g_isInit) {
		g_Game.useProp(propId);
	}
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnDestroy(JNIEnv *env, jclass type)
{
	g_isInit = false;
	g_Game.DestroyGame();
	g_Game.setShellInterface(NULL);
}

JNIEXPORT jint JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_getPing(JNIEnv *env, jclass type) {
	return g_isInit ? g_Game.getPing() : 999;
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnFriendOperationForAppHttpResult
		(JNIEnv *, jclass , jint type, jlong userId) {
	if(!g_isInit)
	{
		return;
	}
	g_Game.onFriendOperationForAppHttpResult(type, (ui64) userId);
};

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnResetGameResult
		(JNIEnv *, jclass , jint result) {
	if(!g_isInit)
	{
		return;
	}
	g_Game.resetGameResult(result);
};

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeSetUserInfo
		(JNIEnv *env, jclass , jstring url, jstring token, jlong inviter) {
	const char* baseUrl = (char*)env->GetStringUTFChars(url, 0);
    const char* userToken = (char*)env->GetStringUTFChars(token, 0);
	g_Game.setUserToken(userToken);
	g_Game.setBaseUrl(baseUrl);
	g_Game.setInviter(inviter);
};

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeOnRechargeResult
        (JNIEnv *, jclass, jint type, jint result) {
	if(!g_isInit)
    {
    	return;
    }
    g_Game.onRechargeResult(type, result);
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeHideRechargeBtn
        (JNIEnv *, jclass) {
    if(!g_isInit)
    {
        return;
    }
    g_Game.hideRechargeBtn();
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesRenderer_nativeExitGame(JNIEnv *env, jclass type) {
	if(!g_isInit)
    {
        return;
    }
    g_Game.exitGame();
}

/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include <stdlib.h>
#include <android/log.h>
#include <string>
#include <cstring>
#include "JniHelper.h"
#include "android/asset_manager_jni.h"
#include "EchoHelper.h"
#include "TypeDef.h"

#define  LOG_TAG    "EchoesHelper.cpp"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define  CLASS_NAME "com/sandboxol/blockmango/EchoesHelper"

EditTextCallback s_pfEditTextCallback = NULL;
void* s_ctx = NULL;

using namespace std;

string g_apkPath;

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetApkPath
  (JNIEnv *, jclass, jstring apkPath)
{
	g_apkPath = JniHelper::jstring2string(apkPath);
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetContext
  (JNIEnv* env, jclass thiz, jobject context)
{
    JavaVM* vm;
    env->GetJavaVM(&vm);
    JniHelper::setJavaVM(vm);
	JniHelper::setClassLoaderFrom(context);
}

JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetEditTextDialogResult
  (JNIEnv* env, jclass thiz, jbyteArray text)
{
	jsize  size = env->GetArrayLength(text);

	if (size > 0) {
		jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
		char* pBuf = (char*)malloc(size+1);
		if (pBuf != NULL) {
			memcpy(pBuf, data, size);
			pBuf[size] = '\0';
			// pass data to edittext's delegate
			if (s_pfEditTextCallback) s_pfEditTextCallback(pBuf, s_ctx);
			free(pBuf);
		}
		env->ReleaseByteArrayElements(text, data, 0);
	} else {
		if (s_pfEditTextCallback) s_pfEditTextCallback("", s_ctx);
	}
}


const char * getApkPath() {
    return g_apkPath.c_str();
}

void showDialogJNI(const char * pszMsg, const char * pszTitle) {
    if (!pszMsg) {
        return;
    }

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showDialog", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;

        if (!pszTitle) {
            stringArg1 = t.env->NewStringUTF("");
        } else {
            stringArg1 = t.env->NewStringUTF(pszTitle);
        }

        jstring stringArg2 = t.env->NewStringUTF(pszMsg);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void showEditTextDialogJNI(const char* pszTitle, const char* pszMessage, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength, EditTextCallback pfEditTextCallback, void* ctx) {
    if (pszMessage == NULL) {
        return;
    }

    s_pfEditTextCallback = pfEditTextCallback;
    s_ctx = ctx;

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIII)V")) {
        jstring stringArg1;

        if (!pszTitle) {
            stringArg1 = t.env->NewStringUTF("");
        } else {
            stringArg1 = t.env->NewStringUTF(pszTitle);
        }

        jstring stringArg2 = t.env->NewStringUTF(pszMessage);

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, nInputMode, nInputFlag, nReturnType, nMaxLength);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void terminateProcessJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "terminateProcess", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

std::string getPackageNameJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getEchoesPackageName", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    return ret;
}

void onCallPayJNI(const char * payInfo)
{
	 JniMethodInfo t;
		if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onCallPay", "(Ljava/lang/String;)V")) {
			jstring stringArg1 = t.env->NewStringUTF(payInfo);
			t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1);

			t.env->DeleteLocalRef(stringArg1);
			t.env->DeleteLocalRef(t.classID);
		}
}

std::string getFileDirectoryJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getEchoesWritablePath", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    
    return ret;
}

std::string getCurrentLanguageJNI() {
    JniMethodInfo t;
    std::string ret("");
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCurrentLanguage", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }

    return ret;
}

void enableAccelerometerJni() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "enableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void setAccelerometerIntervalJni(float interval) {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setAccelerometerInterval", "(F)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
        t.env->DeleteLocalRef(t.classID);
    }
}

void disableAccelerometerJni() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "disableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

// functions for UserDefault
bool getBoolForKeyJNI(const char* pKey, bool defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getBoolForKey", "(Ljava/lang/String;Z)Z")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

int getIntegerForKeyJNI(const char* pKey, int defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getIntegerForKey", "(Ljava/lang/String;I)I")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

float getFloatForKeyJNI(const char* pKey, float defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getFloatForKey", "(Ljava/lang/String;F)F")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jfloat ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

double getDoubleForKeyJNI(const char* pKey, double defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getDoubleForKey", "(Ljava/lang/String;D)D")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jdouble ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

std::string getStringForKeyJNI(const char* pKey, const char* defaultValue)
{
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getStringForKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(defaultValue);
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1, stringArg2);
        ret = JniHelper::jstring2string(str);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(str);
        
        return ret;
    }
    
    return defaultValue;
}

void setBoolForKeyJNI(const char* pKey, bool value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setBoolForKey", "(Ljava/lang/String;Z)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setIntegerForKeyJNI(const char* pKey, int value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setIntegerForKey", "(Ljava/lang/String;I)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setFloatForKeyJNI(const char* pKey, float value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setFloatForKey", "(Ljava/lang/String;F)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setDoubleForKeyJNI(const char* pKey, double value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setDoubleForKey", "(Ljava/lang/String;D)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setStringForKeyJNI(const char* pKey, const char* value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setStringForKey", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(value);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
    }
}

void gameExitJNI()
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onGameExit", "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void nextGameJNI()
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onNextGame", "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void onErrorJNI(int errorCode)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onError", "(I)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, errorCode);
        t.env->DeleteLocalRef(t.classID);
    }
}

void usePropResultsJNI(const char* propId, int results)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onUsePropResults", "(Ljava/lang/String;I)V"))
    {
        jstring stringArg1 = t.env->NewStringUTF(propId);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, results);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
    }
}

void userInJNI(LORD::ui64 userId, const char* userName)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onUserIn", "(Ljava/lang/String;J)V"))
    {
        jstring stringArg1 = t.env->NewStringUTF(userName);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, userId);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
    }
}

void userOutJNI(LORD::ui64 userId)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onUserOut", "(J)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, userId);
        t.env->DeleteLocalRef(t.classID);
    }
}

void loadMapCompleteJNI()
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onLoadMapComplete", "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void gameSettlementJNI(const char* gameType,const char* gameResult)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onGameSettlement", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring type = t.env->NewStringUTF(gameType);
        jstring result = t.env->NewStringUTF(gameResult);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, type, result);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(type);
        t.env->DeleteLocalRef(result);
    }
}

void dataReportJNI(const char* mainEvent,const char* childEvent)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onDataReport", "(Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring main = t.env->NewStringUTF(mainEvent);
        jstring child = t.env->NewStringUTF(childEvent);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, main, child);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(main);
        t.env->DeleteLocalRef(child);
    }
}

void userChangeJNI(const char* userName, const char* teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onUserChange", "(Ljava/lang/String;Ljava/lang/String;JIZ)V"))
    {
        jstring user = t.env->NewStringUTF(userName);
        jstring team = t.env->NewStringUTF(teamName);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, user, team, userId, teamId, isUserIn);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(user);
        t.env->DeleteLocalRef(team);
    }
}

void onFriendOperationJNI(int operationType, LORD::ui64 userId)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onFriendOperation", "(IJ)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, operationType, userId);
        t.env->DeleteLocalRef(t.classID);
    }
}


void resetGameJNI(LORD::ui64 targetUserId, const char* gameType, const char* mapId)
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onResetGame", "(Ljava/lang/String;Ljava/lang/String;J)V"))
    {
        jstring type = t.env->NewStringUTF(gameType);
        jstring mapString = t.env->NewStringUTF(mapId);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, type, mapString, targetUserId);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(type);
        t.env->DeleteLocalRef(mapString);
    }
}

void closeGameJNI(int code)
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onCloseGame", "(I)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, code);
        t.env->DeleteLocalRef(t.classID);
    }
}

void onRechargeJNI(int type)
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onRecharge", "(I)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, type);
        t.env->DeleteLocalRef(t.classID);
    }
}

void playVideoJNI(int type, const char* path)
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "playVideo", "(ILjava/lang/String;)V"))
    {
        jstring videoPath = t.env->NewStringUTF(path);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, type, videoPath);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(videoPath);
    }
}

void playAdVideoJNI()
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "playAdVideo", "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void reportAdVideoShowJNI()
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "reportAdVideoShow", "()V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

void onExpResultJNI(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onExpResult", "(IIIIIII)V"))
    {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, curLv, toLv, addExp, curExp, toExp, upExp, status);
        t.env->DeleteLocalRef(t.classID);
    }
}
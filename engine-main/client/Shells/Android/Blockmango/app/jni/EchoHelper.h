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
#ifndef __ECHO_HELPER_H__
#define __ECHO_HELPER_H__

#include <jni.h>
#include <string>
#include "TypeDef.h"

typedef void (*EditTextCallback)(const char* pText, void* ctx);

extern const char * getApkPath();
extern void showDialogJNI(const char * pszMsg, const char * pszTitle);
extern void showEditTextDialogJNI(const char* pszTitle, const char* pszContent, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength, EditTextCallback pfEditTextCallback, void* ctx);
extern void terminateProcessJNI();
extern void onCallPayJNI(const char* payInfo);
extern std::string getCurrentLanguageJNI();
extern std::string getPackageNameJNI();
extern std::string getFileDirectoryJNI();
extern void enableAccelerometerJni();
extern void disableAccelerometerJni();
extern void setAccelerometerIntervalJni(float interval);
// functions for UserDefault
extern bool getBoolForKeyJNI(const char* pKey, bool defaultValue);
extern int getIntegerForKeyJNI(const char* pKey, int defaultValue);
extern float getFloatForKeyJNI(const char* pKey, float defaultValue);
extern double getDoubleForKeyJNI(const char* pKey, double defaultValue);
extern std::string getStringForKeyJNI(const char* pKey, const char* defaultValue);
extern void setBoolForKeyJNI(const char* pKey, bool value);
extern void setIntegerForKeyJNI(const char* pKey, int value);
extern void setFloatForKeyJNI(const char* pKey, float value);
extern void setDoubleForKeyJNI(const char* pKey, double value);
extern void setStringForKeyJNI(const char* pKey, const char* value);
extern void gameExitJNI();
extern void nextGameJNI();
extern void onErrorJNI(int errorCode);
extern void usePropResultsJNI(const char* propId, int results);
extern void userInJNI(LORD::ui64 userId, const char* userName);
extern void userOutJNI(LORD::ui64 userId);
extern void loadMapCompleteJNI();
extern void gameSettlementJNI(const char* gameType,const char* gameResult);
extern void dataReportJNI(const char* mainEvent,const char* childEvent);
extern void userChangeJNI(const char* userName, const char* teamName, LORD::ui64 userId, LORD::ui32 teamId, bool isUserIn);
extern void onFriendOperationJNI(int operationType, LORD::ui64 userId);
extern void resetGameJNI(LORD::ui64 targetUserId, const char* gameType, const char* mapId);
extern void closeGameJNI(int code);
extern void onRechargeJNI(int type);
extern void playVideoJNI(int type, const char* path);
extern void playAdVideoJNI();
extern void reportAdVideoShowJNI();
extern void onExpResultJNI(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status);

#ifdef __cplusplus
extern "C" {
#endif
#undef com_sandboxol_blockmango_EchoesHelper_RUNNABLES_PER_FRAME
#define com_sandboxol_blockmango_EchoesHelper_RUNNABLES_PER_FRAME 5L
/*
 * Class:     com_sandboxol_blockmango_EchoesHelper
 * Method:    nativeSetApkPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetApkPath
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_sandboxol_blockmango_EchoesHelper
 * Method:    nativeSetEditTextDialogResult
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetEditTextDialogResult
  (JNIEnv *, jclass, jbyteArray);

/*
 * Class:     com_sandboxol_blockmango_EchoesHelper
 * Method:    nativeSetContext
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_blockmango_EchoesHelper_nativeSetContext
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif

#endif

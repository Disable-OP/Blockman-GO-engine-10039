#include <jni.h>
#include <android/log.h>
#include "ClothesPlug.h"
#include "game.h"
#include "Core.h"
#include "ShellInterfaceAndroid.h"

using namespace ClothesPlug;

CGame g_Game;
bool g_isInit = false;

JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeTouchesBegin
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
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeTouchesEnd
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeTouchesEnd
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
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeTouchesMove
 * Signature: ([I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeTouchesMove
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
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeTouchesCancel
 * Signature: ([I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeTouchesCancel
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
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeKeyDown
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeKeyDown
  (JNIEnv *, jclass, jint)
{
	return false;
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeRender
  (JNIEnv *, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.MainTick();
}


/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeInit
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeInit
 (JNIEnv *env, jclass c, jstring s, jstring config, jint w, jint h , jint sex)
{
	if (!g_isInit)
	{
		const char * strRootPath = (char *)env->GetStringUTFChars(s, 0);
		const char* strConfigPath = (char*)env->GetStringUTFChars(config, 0);

		ClothesPlug::ShellInterfaceAndroid* pShellInterface = new ClothesPlug::ShellInterfaceAndroid;

		// init game
		g_Game.setShellInterface(pShellInterface);

		// init game
		g_Game.initEngine(strRootPath, strConfigPath, w, h, 0 , sex);

		env->ReleaseStringUTFChars(s, strRootPath);
		env->ReleaseStringUTFChars(config, strConfigPath);

		g_isInit = true;
	}
}


/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeOnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeOnSurfaceChanged
  (JNIEnv *env, jclass c, jint width, jint height)
{
	if(!g_isInit)
	{
		return;
	}
	g_Game.OnSize(width, height);
}


/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeOnPause
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeOnPause
  (JNIEnv *, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.OnPause();
}


/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeOnResume
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeOnResume
  (JNIEnv *env, jclass)
{
	if(!g_isInit)
	{
		return;
	}

	g_Game.OnResume();
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeOnDestroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeOnDestroy
  (JNIEnv *env, jclass)
{
    if(!g_isInit)
    {
        return;
    }
	g_isInit = false;
	g_Game.DestroyGame();
	g_Game.setShellInterface(NULL);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeParts
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeParts
  (JNIEnv *env, jclass, jstring parts, jstring partsId)
{
    if(!g_isInit)
    {
  		return;
    }
    const char * p = (char *)env->GetStringUTFChars(parts, 0);
  	const char* pId = (char*)env->GetStringUTFChars(partsId, 0);
    g_Game.changeParts(p,pId);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeSex
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeSex
  (JNIEnv *env, jclass, jint sex)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.changeSex(sex);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeAction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeAction
  (JNIEnv *env, jclass, jint id)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.setAction(id);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeSkinColor
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeSkinColor
  (JNIEnv *env, jclass, jfloat r, jfloat g, jfloat b, jfloat a)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.changeSkinColor( r, g, b, a);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeActorSize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeActorSize
  (JNIEnv *env, jclass, jfloat w, jfloat h)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.setActorSize(w,h);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeBackgroundImage
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeBackgroundImage
  (JNIEnv *env, jclass, jstring str)
{
  if(!g_isInit)
  {
  		return;
  }
  const char* str2 = (char *)env->GetStringUTFChars(str, 0);
  g_Game.changeBackgroundImage(str2);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangePosition
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangePosition
  (JNIEnv *env, jclass, jfloat x, jfloat y, jfloat z)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.setPosition(x, y, z);
}

/*
 * Class:    com_sandboxol_clothes_EchoesRenderer
 * Method:    nativeChangeDefaultIdle
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sandboxol_clothes_EchoesRenderer_nativeChangeDefaultIdle
  (JNIEnv *, jclass , jint defaultIdle)
{
  if(!g_isInit)
  {
  		return;
  }
  g_Game.changeDefaultIdle(defaultIdle);
};
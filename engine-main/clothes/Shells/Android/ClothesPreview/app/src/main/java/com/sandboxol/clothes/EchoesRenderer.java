package com.sandboxol.clothes;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.Log;

import java.io.File;

public class EchoesRenderer implements GLSurfaceView.Renderer {
    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("ClothesPreview");
    }

    private static final String LOGTAG = "EchoesRenderer";
    private int mScreenWidth;
    private int mScreenHeight;
    private EchoesHandler mMainHandler;

    boolean mIsActive = false;
    private boolean m_bInitOK;
    private boolean m_bIsUpdating;

    private static final float OBJECT_SCALE_FLOAT = 50.0f;

    public EchoesRenderer(Context context) {
//        try {
//            PackageInfo info = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
//            info.applicationInfo.nativeLibraryDir = info.applicationInfo.nativeLibraryDir.replace("arm64","armeabi-v7a");
//            System.load(new File(info.applicationInfo.nativeLibraryDir, "libClothesPreview.so").getPath());
//        } catch (PackageManager.NameNotFoundException e) {
//            e.printStackTrace();
//        }
//        try {
//            if (BuildConfig.FLAVOR.toLowerCase().contains("x86")) {
//                String path = context.getDir("resources", Context.MODE_WORLD_WRITEABLE).getPath() + "/libs/x86/libClothesPreview.so";
//                System.load(path);
//            } else {
//                String path = context.getDir("resources", Context.MODE_WORLD_WRITEABLE).getPath() + "/libs/armeabi-v7a/libClothesPreview.so";
//                if (new File(path).exists()) {
//                    Log.e("EchoesRenderer", "libClothesPreview so exists");
//                } else {
//                    Log.e("EchoesRenderer", "libClothesPreview not so exists");
//                }
//
//                System.load(path);
//            }
//        }  catch (Exception | UnsatisfiedLinkError error) {
//            error.printStackTrace();
//        }
        //Log.e("EchoesRenderer" , "==============================================");
        //System.loadLibrary("ClothesPreview");
    }

    public void setScreenWidthAndHeight(final int pSurfaceWidth, final int pSurfaceHeight) {
        this.mScreenWidth = pSurfaceWidth;
        this.mScreenHeight = pSurfaceHeight;
    }

    public int getScreenWidth() {
        return mScreenWidth;
    }

    public int getScreenHeight() {
        return mScreenHeight;
    }

    public void setInitOK(boolean flag) {
        m_bInitOK = flag;
    }

    public void setUpdatingFlag(boolean flag) {
        m_bIsUpdating = flag;
    }

    public void SetMainHandler(EchoesHandler handler) {
        mMainHandler = handler;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================
    @Override
    public void onSurfaceCreated(final GL10 pGL10, final EGLConfig pEGLConfig) {
        m_bInitOK = false;
        m_bIsUpdating = false;
        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_GL_INIT_OK;
        mMainHandler.sendMessage(msg);
    }

    public void SizeChanged(int _width, int _height) {
        EchoesRenderer.nativeOnSurfaceChanged(_width, _height);
    }

    @Override
    public void onSurfaceChanged(final GL10 pGL10, final int pWidth, final int pHeight) {
        SizeChanged(pWidth, pHeight);
    }

    static int nCount = 0;
    static boolean init = false;

    @Override
    public void onDrawFrame(final GL10 gl) {
        try {
            if (m_bIsUpdating) {
                Message msg = new Message();
                msg.what = EchoesHandler.HANDLER_UPDATE_DOWNLOAD;
                float percent = EchoesRenderer.nativeGetDownloadPercent();
                int state = EchoesRenderer.nativeGetDownloadState();
                msg.obj = new EchoesHandler.UpdateDownloadMessage((int) (percent * 100), 100, state);

                mMainHandler.sendMessage(msg);
            }

            // Call our function to render content
            if (m_bInitOK) {
                EchoesRenderer.nativeRender();
            }
        } catch (Exception e) {
            System.out.println("gl thread exception");
        }
    }

    // ===========================================================
    // Methods
    // ===========================================================
    private static native void nativeTouchesBegin(final int pID, final float pX, final float pY);

    private static native void nativeTouchesEnd(final int pID, final float pX, final float pY);

    private static native void nativeTouchesMove(final int[] pIDs, final float[] pXs, final float[] pYs);

    private static native void nativeTouchesCancel(final int[] pIDs, final float[] pXs, final float[] pYs);

    private static native boolean nativeKeyDown(final int pKeyCode);

    private static native void nativeRender();

    public static native void nativeInit(String strRootPath, String strConfigPath, final int pWidth, final int pHeight ,int sex);

    private static native void nativeOnSurfaceChanged(final int pWidth, final int pHeight);

    private static native void nativeOnPause();

    private static native void nativeOnResume();

    private static native void nativeOnDestroy();

    public static native void nativeChangeParts(String masterName, String slaveName);

    public static native void nativeChangeSex(int sex);

    public static native void nativeChangeAction(int id);

    public static native void nativeChangeSkinColor(float r, float g, float b, float a);

    public static native void nativeChangeActorSize(float w, float h);

    private static native void nativeOnGetPhoneType(String strType);

    // java call game interface define
    public static native int nativeCheckVersion(String strRootPath);

    public static native int nativeUpdateFiles();

    public static native float nativeGetDownloadPercent();

    public static native int nativeGetDownloadState();

    public static native int nativeGetTotalDownloadSize();

    public static native int nativeGetCurrentDownloadSize();

    public static native String nativeGetLocalVersion();

    public static native String nativeGetServerVersion();

    public void handleActionDown(final int pID, final float pX, final float pY) {
        EchoesRenderer.nativeTouchesBegin(pID, pX, pY);
    }

    public void handleActionUp(final int pID, final float pX, final float pY) {
        EchoesRenderer.nativeTouchesEnd(pID, pX, pY);
    }

    public void handleActionCancel(final int[] pIDs, final float[] pXs, final float[] pYs) {
        EchoesRenderer.nativeTouchesCancel(pIDs, pXs, pYs);
    }

    public void handleActionMove(final int[] pIDs, final float[] pXs, final float[] pYs) {
        EchoesRenderer.nativeTouchesMove(pIDs, pXs, pYs);
    }

    public void handleKeyDown(final int pKeyCode) {
        EchoesRenderer.nativeKeyDown(pKeyCode);
    }

    public void handleOnPause() {
        EchoesRenderer.nativeOnPause();
    }

    public void handleOnResume() {
        EchoesRenderer.nativeOnResume();
    }

    public void handleOnDestroy() {
        EchoesRenderer.nativeOnDestroy();
    }

    public void handleInitGame(final String strRootPath, final String strConfigPath, final int nWidth, final int nHeight, int sex) {
        EchoesRenderer.nativeInit(strRootPath, strConfigPath, nWidth, nHeight , 2);
    }

    private static native int nativeChangePosition(float x, float y, float z);
    private static native int nativeChangeBackgroundImage(String backgroundImage);

    public void handleChangePosition(float x, float y, float z){
        EchoesRenderer.nativeChangePosition(x,y,z);
    }

    public void handleChangeBackgroundImage(String backgroundImage){
        EchoesRenderer.nativeChangeBackgroundImage(backgroundImage);
    }

}

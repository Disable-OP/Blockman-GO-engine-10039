package com.sandboxol.blockmango;

import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class EchoesRenderer implements GLSurfaceView.Renderer {
    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("BlockMan");
    }

    public static final int FRIEND_OPERATION_TYPE_NO_FRIEND = 1;
    public static final int FRIEND_OPERATION_TYPE_IS_FRIEND = 2;
    public static final int FRIEND_OPERATION_TYPE_AGREE_ADD_FRIEND = 3;
    public static final int FRIEND_OPERATION_TYPE_REQUEST_ADD_FRIEND = 4;
    public static final int FRIEND_OPERATION_TYPE_AGREE_ADD_FRIEND_FAILURE = 10000;
    public static final int FRIEND_OPERATION_TYPE_REQUEST_ADD_FRIEND_FAILURE = 10001;

    private static final String LOGTAG = "EchoesRenderer";
    private int mScreenWidth;
    private int mScreenHeight;
    private EchoesHandler mMainHandler;

    private boolean m_bInitOK;
    private boolean m_bIsUpdating;
    private GL10 mGL10;

    private static final float OBJECT_SCALE_FLOAT = 50.0f;

    public EchoesRenderer(EchoesHandler handler){
        mMainHandler = handler;
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

    public boolean isInitOK() {
        return m_bInitOK;
    }

    public void setUpdatingFlag(boolean flag) {
        m_bIsUpdating = flag;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================
    @Override
    public void onSurfaceCreated(final GL10 pGL10, final EGLConfig pEGLConfig) {
        //EchoesRenderer.nativeInit(this.getSDPath(), this.mScreenWidth, this.mScreenHeight);
        m_bInitOK = false;
        m_bIsUpdating = false;

        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_GL_INIT_OK;
        mMainHandler.sendMessage(msg);
        mGL10 = pGL10;
    }

    public GL10 getGL10() {
        return mGL10;
    }

    @Override
    public void onSurfaceChanged(final GL10 pGL10, final int pWidth, final int pHeight) {
//        if (pWidth != mWidth && mHeight != pHeight) {
//            EchoesRenderer.nativeOnSurfaceChanged(pWidth, pHeight);
//        }
//        mWidth = pWidth;
//        mHeight = pHeight;
        String string = pGL10.glGetString(GL10.GL_VERSION);
        Log.e("OpenGL","onSurfaceChanged-version:"+ string);
    }

    @Override
    public void onDrawFrame(final GL10 gl) {
        try {
            mGL10 = gl;
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
    // Methodsz
    // ===========================================================
    private static native void nativeTouchesBegin(final int pID, final float pX, final float pY);

    private static native void nativeTouchesEnd(final int pID, final float pX, final float pY);

    private static native void nativeTouchesMove(final int[] pIDs, final float[] pXs, final float[] pYs);

    private static native void nativeTouchesCancel(final int[] pIDs, final float[] pXs, final float[] pYs);

    private static native boolean nativeKeyDown(final int pKeyCode);

    private static native boolean nativeKeyUp(final int pKeyCode);

    private static native void nativeRender();

    public static native void nativeInit(float displayDensity, String strRootPath, String strConfigPath, String mapPath, int pWidth, int pHeight);

    public static native void nativeInitGame(float displayDensity, String nickName, long userId, String token, String ip, int port, long gameTimestamp, String lang, String gameType,
                                             String mapName, String mapUrl, String strRootPath, String strConfigPath, String mapPath, int nWidth, int nHeight, int isMultiPlayer);

    private static native void nativeOnSurfaceChanged(final int pWidth, final int pHeight);

    private static native void nativeOnPause();

    private static native void nativeOnResume();

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

    public static native void nativeOnDestroy();

    public static native void nativeUseProp(String propId);

    public static native int getPing();

    public static native void nativeOnRechargeResult(int type, int result);

    public static native void nativeHideRechargeBtn();

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

    public void handleKeyUp(final int pKeyCode) {
        EchoesRenderer.nativeKeyUp(pKeyCode);
    }

    public void handleOnPause() {
        if (m_bInitOK)
            EchoesRenderer.nativeOnPause();
    }

    public void handleOnResume() {
        if (m_bInitOK)
            EchoesRenderer.nativeOnResume();
    }

    public void handleInitGame(float displayDensity, final String strRootPath, final String strConfigPath, final String mapPath, final int nWidth, final int nHeight) {
        EchoesRenderer.nativeInit(displayDensity, strRootPath, strConfigPath, mapPath, nWidth, nHeight);
        EchoesRenderer.nativeOnGetPhoneType(EchoesActivity.s_mainActivity.m_PhoneType);
    }

    public void handleInitGame(float displayDensity, final String nickName, final long userId, final String token, final String gameAddr, long gameTimestamp, String lang, String gameType,
                               String mapName, String mapUrl, final String strRootPath, final String strConfigPath, final String mapPath, final int nWidth, final int nHeight) {
        try {
            String[] arry = gameAddr.split(":");
            String ip = arry[0];
            int port = Integer.valueOf(arry[1]);
            EchoesRenderer.nativeInitGame(displayDensity, nickName, userId, token, ip, port, gameTimestamp, lang, gameType, mapName, mapUrl, strRootPath, strConfigPath, mapPath, nWidth, nHeight, 0);
            EchoesRenderer.nativeOnGetPhoneType(EchoesActivity.s_mainActivity.m_PhoneType);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void handleUseProp(String propId) {
        EchoesRenderer.nativeUseProp(propId);
    }

    public void handleOnDestroy() {
        EchoesRenderer.nativeOnDestroy();
    }

    private int mWidth = 0, mHeight = 0;

    public void handleSurfaceChanged(int width, int height) {
        if (width != mWidth && mHeight != height) {
            EchoesRenderer.nativeOnSurfaceChanged(width, height);
        }
        mWidth = width;
        mHeight = height;
    }


    private static native void nativeOnFriendOperationForAppHttpResult(int operationType, long userId);

    public void handleOnFriendOperationForAppHttpResult(int operationType, long userId) {
        EchoesRenderer.nativeOnFriendOperationForAppHttpResult(operationType, userId);
    }

    private static native void nativeOnResetGameResult(int result);


    /**
     * @param result 1 成功 2 拜访他人领地失败
     */
    public void handleNativeOnResetGameResult(int result) {
        EchoesRenderer.nativeOnResetGameResult(result);
    }

    private static native void nativeSetUserInfo(String baseUrl, String userToken, long inviter);

    public void handleNativeSetUserInfo(String baseUrl, String userToken, long inviter) {
        EchoesRenderer.nativeSetUserInfo(baseUrl, userToken, inviter);
	}
	
    public void handleRechargeResult(int type, int result) {
        EchoesRenderer.nativeOnRechargeResult(type, result);
    }

    public void handleHideRechargeBtn() {
        EchoesRenderer.nativeHideRechargeBtn();
    }
}

package com.sandboxol.blockmango;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;
import java.util.Locale;

public class EchoesHelper {
    // ===========================================================
    // Constants
    // ===========================================================
    private static final String PREFS_NAME = "Cocos2dxPrefsFile";
    private static final int RUNNABLES_PER_FRAME = 5;

    // ===========================================================
    // Fields
    // ===========================================================
    public static EchoesMusic sEchoesMusic;
    public static EchoesSound sEchoesSound;
    //private static AssetManager sAssetManager;
    private static boolean sAccelerometerEnabled;
    private static String sPackageName;
    private static String sFileDirectory;
    private static Activity sActivity = null;
    private static EchoesHelperListener sEchoesHelperListener;

    /**
     * Optional meta-that can be in the manifest for this component, specifying
     * the name of the native shared library to load.  If not specified,
     * "main" is used.
     */
    private static final String META_DATA_LIB_NAME = "android.app.lib_name";
    private static final String DEFAULT_LIB_NAME = "main";

    // ===========================================================
    // Constructors
    // ===========================================================

    public static void runOnGLThread(final Runnable r) {
        ((EchoesActivity) sActivity).runOnGLThread(r);
    }

    private static boolean sInited = false;

    public static void init(final Activity activity) {
        if (!sInited) {
            final ApplicationInfo applicationInfo = activity.getApplicationInfo();

            EchoesHelper.sEchoesHelperListener = (EchoesHelperListener) activity;
            
            /*
            try 
            {
            	// Get the lib_name from AndroidManifest.xml metadata
            	ActivityInfo ai = activity.getPackageManager().getActivityInfo(activity.getIntent().getComponent(), PackageManager.GET_META_DATA);
            	if (null != ai.metaData) 
            	{
            		String lib_name = ai.metaData.getString(META_DATA_LIB_NAME);
            		if (null != lib_name) 
            		{
            			System.loadLibrary(lib_name);
            		}
            		else 
            		{
            			System.loadLibrary(DEFAULT_LIB_NAME);
            		}
            	}
            }
            catch (PackageManager.NameNotFoundException e) 
            {
               throw new RuntimeException("Error getting activity info", e);
            }
			*/

            EchoesHelper.sPackageName = applicationInfo.packageName;
            EchoesHelper.sFileDirectory = activity.getFilesDir().getAbsolutePath();

            //EchoesHelper.sCocos2dxAccelerometer = new Cocos2dxAccelerometer(activity);
            EchoesHelper.sEchoesMusic = new EchoesMusic(activity);
            EchoesHelper.sEchoesSound = new EchoesSound(activity);
            //EchoesHelper.sAssetManager = activity.getAssets();
            // this native method must be called first
            EchoesHelper.nativeSetContext((Object) activity);
            EchoesHelper.nativeSetApkPath(applicationInfo.sourceDir);

            //Cocos2dxBitmap.setContext(activity);
            //Cocos2dxETCLoader.setContext(activity);
            sActivity = activity;

            sInited = true;
        }
    }

    public static Activity getActivity() {
        return sActivity;
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeSetApkPath(final String pApkPath);

    private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

    private static native void nativeSetContext(final Object pContext);

    public static String getCocos2dxPackageName() {
        return EchoesHelper.sPackageName;
    }

    public static String getCocos2dxWritablePath() {
        return EchoesHelper.sFileDirectory;
    }

    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }

    public static String getDeviceModel() {
        return Build.MODEL;
    }

//	public static AssetManager getAssetManager() {
//		return EchoesHelper.sAssetManager;
//	}

    public static void enableAccelerometer() {
        EchoesHelper.sAccelerometerEnabled = true;
        //EchoesHelper.sCocos2dxAccelerometer.enable();
    }

    private static void onCallPay(final String payInfo) throws JSONException {
        try {
            Log.i("tag", payInfo);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void setAccelerometerInterval(float interval) {
        //EchoesHelper.sCocos2dxAccelerometer.setInterval(interval);
    }

    public static void disableAccelerometer() {
        EchoesHelper.sAccelerometerEnabled = false;
        //EchoesHelper.sCocos2dxAccelerometer.disable();
    }

    public static void preloadBackgroundMusic(final String pPath) {
        Log.e("initGame", "preloadBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.preloadBackgroundMusic(pPath);
    }

    public static void playBackgroundMusic(final String pPath, final boolean isLoop, final float minInterval, final float maxInterval) {
        Log.e("initGame", "playBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.playBackgroundMusic(pPath, isLoop, minInterval, maxInterval);
    }

    public static void resumeBackgroundMusic() {
        Log.e("initGame", "resumeBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.resumeBackgroundMusic();
    }

    public static void pauseBackgroundMusic() {
        Log.e("initGame", "pauseBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.pauseBackgroundMusic();
    }

    public static void stopBackgroundMusic() {
        Log.e("initGame", "stopBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.stopBackgroundMusic();
    }

    public static void rewindBackgroundMusic() {
        Log.e("initGame", "rewindBackgroundMusic ..................");
        EchoesHelper.sEchoesMusic.rewindBackgroundMusic();
    }

    public static boolean isBackgroundMusicPlaying() {
        Log.e("initGame", "isBackgroundMusicPlaying ..................");
        return EchoesHelper.sEchoesMusic.isBackgroundMusicPlaying();
    }

    public static float getBackgroundMusicVolume() {
        Log.e("initGame", "getBackgroundMusicVolume ..................");
        return EchoesHelper.sEchoesMusic.getBackgroundVolume();
    }

    public static void setBackgroundMusicVolume(final float volume) {
        Log.e("initGame", "setBackgroundMusicVolume ..................");
        EchoesHelper.sEchoesMusic.setBackgroundVolume(volume);
    }

    public static void preloadEffect(final String path) {
        Log.e("initGame", "preloadEffect ..................");
        EchoesHelper.sEchoesSound.preloadEffect(path);
    }

    public static int playEffect(final String path, final boolean isLoop, final float pitch, final float pan, final float gain) {
        Log.e("initGame", "playEffect ..................");
        return EchoesHelper.sEchoesSound.playEffect(path, isLoop, pitch, pan, gain);
    }

    public static void resumeEffect(final int soundId) {
        Log.e("initGame", "resumeEffect ..................");
        EchoesHelper.sEchoesSound.resumeEffect(soundId);
    }

    public static void pauseEffect(final int soundId) {
        Log.e("initGame", "pauseEffect ..................");
        EchoesHelper.sEchoesSound.pauseEffect(soundId);
    }

    public static void stopEffect(final int soundId) {
        Log.e("initGame", "stopEffect ..................");
        EchoesHelper.sEchoesSound.stopEffect(soundId);
    }

    public static float getEffectsVolume() {
        Log.e("initGame", "getEffectsVolume ..................");
        return EchoesHelper.sEchoesSound.getEffectsVolume();
    }

    public static void setEffectsVolume(final float volume) {
        Log.e("initGame", "setEffectsVolume ..................");
        EchoesHelper.sEchoesSound.setEffectsVolume(volume);
    }

    public static void unloadEffect(final String path) {
        Log.e("initGame", "unloadEffect ..................");
        EchoesHelper.sEchoesSound.unloadEffect(path);
    }

    public static void pauseAllEffects() {
        Log.e("initGame", "pauseAllEffects ..................");
        EchoesHelper.sEchoesSound.pauseAllEffects();
    }

    public static void resumeAllEffects() {
        Log.e("initGame", "resumeAllEffects ..................");
        EchoesHelper.sEchoesSound.resumeAllEffects();
    }

    public static void stopAllEffects() {
        Log.e("initGame", "stopAllEffects ..................");
        EchoesHelper.sEchoesSound.stopAllEffects();
    }

    public static void end() {
        Log.e("initGame", "end ..................");
        EchoesHelper.sEchoesMusic.end();
        EchoesHelper.sEchoesSound.end();
    }

    public static void onResume() {
        if (EchoesHelper.sAccelerometerEnabled) {
            //EchoesHelper.sCocos2dxAccelerometer.enable();
        }
    }

    public static void onPause() {
        if (EchoesHelper.sAccelerometerEnabled) {
            //EchoesHelper.sCocos2dxAccelerometer.disable();
        }
    }

    public static void terminateProcess() {
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    private static void showDialog(final String pTitle, final String pMessage) {
        EchoesHelper.sEchoesHelperListener.showDialog(pTitle, pMessage);
    }

    private static void showEditTextDialog(final String pTitle, final String pMessage, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength) {
        EchoesHelper.sEchoesHelperListener.showEditTextDialog(pTitle, pMessage, pInputMode, pInputFlag, pReturnType, pMaxLength);
    }

    public static void setEditTextDialogResult(final String pResult) {
        try {
            final byte[] bytesUTF8 = pResult.getBytes("UTF8");

            EchoesHelper.sEchoesHelperListener.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    EchoesHelper.nativeSetEditTextDialogResult(bytesUTF8);
                }
            });
        } catch (UnsupportedEncodingException pUnsupportedEncodingException) {
            /* Nothing. */
        }
    }

    public static int getDPI() {
        if (sActivity != null) {
            DisplayMetrics metrics = new DisplayMetrics();
            WindowManager wm = sActivity.getWindowManager();
            if (wm != null) {
                Display d = wm.getDefaultDisplay();
                if (d != null) {
                    d.getMetrics(metrics);
                    return (int) (metrics.density * 160.0f);
                }
            }
        }
        return -1;
    }

    // ===========================================================
    // Functions for CCUserDefault
    // ===========================================================

    public static boolean getBoolForKey(String key, boolean defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        return settings.getBoolean(key, defaultValue);
    }

    public static int getIntegerForKey(String key, int defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        return settings.getInt(key, defaultValue);
    }

    public static float getFloatForKey(String key, float defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        return settings.getFloat(key, defaultValue);
    }

    public static double getDoubleForKey(String key, double defaultValue) {
        // SharedPreferences doesn't support saving double value
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        return settings.getFloat(key, (float) defaultValue);
    }

    public static String getStringForKey(String key, String defaultValue) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        return settings.getString(key, defaultValue);
    }

    public static void setBoolForKey(String key, boolean value) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(key, value);
        editor.commit();
    }

    public static void setIntegerForKey(String key, int value) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt(key, value);
        editor.commit();
    }

    public static void setFloatForKey(String key, float value) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, value);
        editor.commit();
    }

    public static void setDoubleForKey(String key, double value) {
        // SharedPreferences doesn't support recording double value
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, (float) value);
        editor.commit();
    }

    public static void setStringForKey(String key, String value) {
        SharedPreferences settings = sActivity.getSharedPreferences(EchoesHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(key, value);
        editor.commit();
    }

    public static void onGameExit() {
        Log.e("GameExit", "================================");
        if (sEchoesHelperListener != null) {
            sEchoesHelperListener.onGameExit();
        }
    }

    public static void onNextGame(){
        if (sEchoesHelperListener != null) {
            //sEchoesHelperListener.onGameExit();
        }
    }

    public static void onError(int errorCode) {
        String a = "kickOut";
    }

    public static void onUsePropResults(String propId, int results) {
        Log.e("jni_onUsePropResults", "=================UsePropResults[" + results + "]===============" + propId);
    }

    public static void onUserIn(String nickName, long userId) {
        Log.e("jni_onUserIn", "nickName_" + nickName + " userId_" + userId);
    }

    public static void onUserOut(long userId) {
        Log.e("jni_onUserOut", "userId_" + userId);
    }

    public static void onLoadMapComplete() {
        sEchoesHelperListener.onLoadMapComplete();
    }

    public static void onGameSettlement(String gameType, String gameResult) {
        Log.e("jni_onGameSettlement", "gameType_" + gameType + " gameResult_" + gameResult);
    }

    public static void onDataReport(String mainEvent, String childEvent) {
        if (TextUtils.isEmpty(childEvent)) {

        } else {

        }
    }

    public static void onUserChange(String userName, String teamName, long userId, int teamId, boolean isUserIn) {

    }

    /**
     *
     * @param operationType 1 agree add friend, 2 request add friend
     * @param userId
     */
    public static void onFriendOperation(int operationType, long userId) {
        Log.e("jni_onFriendOperation", "operationType = " + operationType +" userId = " + userId);
    }

    public static void onResetGame(final String gameType, final String mapId, final long targetUserId) {
        Log.e("jni_onResetGame", "gameType = " + gameType +" mapId = " + mapId +" targetUserId = " + targetUserId);
        if (sActivity != null && EchoesGLSurfaceView.getInstance() != null) {
            sActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    UserInfo.newInstance().gameType = gameType;
                    UserInfo.newInstance().targetId = targetUserId;
                    UserInfo.newInstance().mapId = mapId;
                    EchoesGLSurfaceView.getInstance().resetGame();
                }
            });
        }
    }

    public static void onCloseGame(int code) {
        if (sActivity != null) {
            sActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {

                }
            });
        }
    }

    public static void onRecharge(int type) {

    }

    public static void playVideo(int type, String path) {

    }

    public static void playAdVideo() {

    }

    public static void reportAdVideoShow() {

    }

    public static void onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status) {

    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static interface EchoesHelperListener {

        void showDialog(final String pTitle, final String pMessage);

        void showEditTextDialog(final String pTitle, final String pMessage, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength);

        void onLoadMapComplete();

        void onGameExit();

        void runOnGLThread(final Runnable pRunnable);
    }
}

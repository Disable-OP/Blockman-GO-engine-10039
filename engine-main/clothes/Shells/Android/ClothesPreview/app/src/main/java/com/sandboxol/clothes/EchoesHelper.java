package com.sandboxol.clothes;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
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

    // ===========================================================
    // Fields
    // ===========================================================
    //private static AssetManager sAssetManager;
    private static boolean sAccelerometerEnabled;
    private static String sPackageName;
    private static String sFileDirectory;
    private static Activity sActivity = null;
    private static EchoesHelperListener sEchoesHelperListener;

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
            EchoesHelper.sPackageName = applicationInfo.packageName;
            EchoesHelper.sFileDirectory = activity.getFilesDir().getAbsolutePath();
            EchoesHelper.nativeSetContext(activity);
            EchoesHelper.nativeSetApkPath(applicationInfo.sourceDir);
            sActivity = activity;
            sInited = true;
        }
    }

    public static Activity getActivity() {
        return sActivity;
    }

    private static native void nativeSetApkPath(final String pApkPath);

    private static native void nativeSetEditTextDialogResult(final byte[] pBytes);

    private static native void nativeSetContext(final Object pContext);

    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }


    public static void enableAccelerometer() {
        EchoesHelper.sAccelerometerEnabled = true;
        //EchoesHelper.sCocos2dxAccelerometer.enable();
    }

    private static void onCallPay(final String payInfo) throws JSONException {
        try {
            Log.i("tag", payInfo);

			/*
			Intent intent = new Intent(); 
			intent.setClassName("com.sandboxol.MineCraft", "com.sandboxol.MineCraft.UniBrowserActivity");
			String urlRequest = "http://www.baidu.com";
			DisplayMetrics  dm = new DisplayMetrics();
			sActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);
			
			int Intent_FLAG_ACTIVITY_NO_ANIMATION = 0x10000;
			intent.addFlags(Intent_FLAG_ACTIVITY_NO_ANIMATION);
			intent.addFlags(intent.FLAG_ACTIVITY_NEW_TASK);
			intent.putExtra("url", urlRequest);
			intent.putExtra("left", 0);
			intent.putExtra("top", 0);
			intent.putExtra("right", 0);
			intent.putExtra("bottom", 0);
			
			sActivity.startActivity(intent);
			*/
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

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public interface EchoesHelperListener {
        public void showDialog(final String pTitle, final String pMessage);

        public void showEditTextDialog(final String pTitle, final String pMessage, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength);

        public void runOnGLThread(final Runnable pRunnable);
    }
}

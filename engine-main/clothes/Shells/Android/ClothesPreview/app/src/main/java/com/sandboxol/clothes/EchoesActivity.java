package com.sandboxol.clothes;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileNotFoundException;

import org.apache.http.util.EncodingUtils;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import com.sandboxol.clothes.EchoesHelper.EchoesHelperListener;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

public class EchoesActivity extends Activity implements EchoesHelperListener {
    private static final String LOGTAG = "EchoesActivity";

    /**
     * inner version file. used to update resource online.
     */
    public final static String VERSION_FILE_NAME = "res.version";
    /**
     * the ASSERT file name in APK/assert/ folder.
     */
    public final static String ASSETS_NAME = "resources";
    /**
     * singleton reference.
     */
    public static EchoesActivity s_mainActivity;

    private EchoesGLSurfaceView mGLSurfaceView;
    private EchoesHandler mHandler;
    public Handler mChildHandler;

    private TextView mTextView;
    private TextView mVersionTextView;
    private ImageView mImageView;
    private FrameLayout mFrameLayout;

    public int m_nTotalFileCount;
    public int m_nCopyedFileCount;
   // private String m_strRootPath;

    private String mResPath;
    public String m_PhoneType;

    EchoesRenderer mRenderer;

    // ===========================================================
    // Constructors
    // ===========================================================
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // singleton reference initialize.
        s_mainActivity = this;


        this.mHandler = new EchoesHandler(this);

        this.init();

        EchoesHelper.init(this);

        TelephonyManager mTm = (TelephonyManager) this.getSystemService(Context.TELEPHONY_SERVICE);
        String deviceModel = android.os.Build.MODEL;
        String deviceBrand = android.os.Build.BRAND;
        m_PhoneType = deviceModel + "|" + deviceBrand;
    }

    @Override
    protected void onResume() {
        super.onResume();
        this.mGLSurfaceView.onResume();
    }

    @Override
    protected void onDestroy() {
        Log.d(LOGTAG, "onDestroy");
        super.onDestroy();
        mChildHandler.getLooper().quit();
        System.gc();
    }

    @Override
    protected void onPause() {
        super.onPause();
        this.mGLSurfaceView.onPause();
    }

    public String getRootPath() {
        return mResPath;
    }

    public void init() {
        // FrameLayout
        ViewGroup.LayoutParams frameLayoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,  ViewGroup.LayoutParams.MATCH_PARENT);
        mFrameLayout = new FrameLayout(this);
        mFrameLayout.setLayoutParams(frameLayoutParams);
        mRenderer = new EchoesRenderer(this);
        onCreateView();
        mFrameLayout.addView(this.mGLSurfaceView);
        mImageView = new ImageView(this);
        mImageView.setImageResource(R.drawable.loading);
        mImageView.setScaleType(ImageView.ScaleType.FIT_XY);
        mFrameLayout.addView(mImageView);
        mTextView = new TextView(this);
        mVersionTextView = new TextView(this);
        String tips = String.format("%s:0%%", this.getString(R.string.prepare_text));
        mTextView.setText(tips);
        mTextView.setGravity(Gravity.RIGHT | Gravity.BOTTOM);
        mFrameLayout.addView(mTextView);
        mVersionTextView.setText("");
        mFrameLayout.addView(mVersionTextView);
        setContentView(mFrameLayout);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mResPath = getResPath();
        Button btn = new Button(this);
        ViewGroup.LayoutParams btnParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,  ViewGroup.LayoutParams.WRAP_CONTENT);
        btn.setLayoutParams(btnParams);
        btn.setText("切换性别");
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isBox = !isBox;
                EchoesGLSurfaceView.getInstance().nativeChangeSex(isBox ? 1 :2);
            }
        });
        mFrameLayout.addView(btn);
    }

    private boolean isBox = true;

    public void doMainInit() {
        String configPath = getConfigPath();
        File configDir = new File(configPath);
        if (!configDir.exists()) {
            configDir.mkdirs();
        }
        new ChildThread().start();
    }

    public void removeLoadingView() {
        mFrameLayout.removeView(mTextView);
        mFrameLayout.removeView(mImageView);
        mFrameLayout.removeView(mVersionTextView);
    }

    public void onCreateView() {
        this.mGLSurfaceView = new EchoesGLSurfaceView(this);
        this.mGLSurfaceView.setEchoesRenderer(mRenderer);
        this.mGLSurfaceView.SetMainHandler(mHandler);
    }

    public void setUpdateTips(String strText) {
        mTextView.setText(strText);
    }

    public void setVersionText(String strLocalVersion, String strServerVersion) {
        String tips = String.format("%s %s \n%s %s",  getString(R.string.local_version_text), strLocalVersion, getString(R.string.server_version_text), strServerVersion);
        mVersionTextView.setText(tips);
    }

    /**
     * if the APK is upgraded, and the sd-card assets is older version.
     * then need to copy all the APK/assert to sd-card.
     */
    public boolean checkNeedCopy() {
        File file = new File(mResPath, VERSION_FILE_NAME);

        if (file.exists()) {
            String[] oldVersionArray = new String[10];
            String[] newVersionArray = new String[10];

            // fist get the version from local file.
            try {
                FileInputStream fis = new FileInputStream(file);
                byte[] buffer = new byte[fis.available()];
                fis.read(buffer);
                fis.close();

                InputStream in = null;
                in = getAssets().open(ASSETS_NAME + "/" + VERSION_FILE_NAME);
                int n = in.available();
                byte[] newBuffer = new byte[n];
                in.read(newBuffer);
                in.close();

                String strOldJsonVersion = EncodingUtils.getString(buffer, "UTF-8");
                String strNewJsonVersion = EncodingUtils.getString(newBuffer, "UTF-8");

                JSONTokener jsonParser = new JSONTokener(strOldJsonVersion);
                JSONTokener newJsonParser = new JSONTokener(strNewJsonVersion);

                try {
                    JSONObject versionObj = (JSONObject) jsonParser.nextValue();

                    String strVersion = versionObj.getString("version");

                    oldVersionArray = strVersion.split("\\.");

                    JSONObject newVersionObj = (JSONObject) newJsonParser.nextValue();

                    strVersion = newVersionObj.getString("version");

                    newVersionArray = strVersion.split("\\.");

                    int nOldAppVersion0 = Integer.parseInt(oldVersionArray[0]);
                    int nNewAppVersion0 = Integer.parseInt(newVersionArray[0]);

                    int nOldAppVersion1 = Integer.parseInt(oldVersionArray[1]);
                    int nNewAppVersion1 = Integer.parseInt(newVersionArray[1]);


                    if (nNewAppVersion0 > nOldAppVersion0) {
                        return true;
                    } else if (nNewAppVersion0 == nOldAppVersion0) {
                        if (nNewAppVersion1 > nOldAppVersion1) {
                            return true;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } catch (JSONException ex) {
                    return true;
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                return true;
            } catch (IOException e) {
                e.printStackTrace();
                return true;
            }
        } else {
            // there is no version file in sd-card. need copy first.
            return true;
        }
    }

    public String getResPath() {
        return getDir(ASSETS_NAME, MODE_WORLD_WRITEABLE).getPath() + "/";
    }

    private String getSDCardPath() {
        boolean sdCardExist = Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED);
        return (sdCardExist ? new File(Environment.getExternalStorageDirectory(), "SandBoxOL/BlockMan").getPath() : getDir("BlockMan", MODE_WORLD_WRITEABLE).getPath());
    }

    public String getMapPath() {
        return getSDCardPath() + "/map/";
    }

    public String getConfigPath() {
        return getSDCardPath() + "/config/";
    }

    private void copySingleFile(String strAssetDir, String strRootPath, String strFileName) {
        try {
            File outFile = new File(strRootPath, strFileName);
            InputStream in = null;
            in = getAssets().open(strAssetDir + "/" + strFileName);
            OutputStream out = new FileOutputStream(outFile, true);

            // Transfer bytes from in to out
            byte[] buf = new byte[1024 * 5];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }

            in.close();
            out.close();

            ++m_nCopyedFileCount;
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_UPDATE_COPY;
        msg.obj = new EchoesHandler.CopyMessage(m_nCopyedFileCount, m_nTotalFileCount);

        mHandler.sendMessage(msg);
    }

    private int getTotalFileCount(String strAssetDir) {
        int nFileCount = 0;

        String[] files;
        try {
            files = this.getResources().getAssets().list(strAssetDir);
        } catch (IOException e1) {
            return 0;
        }

        int nCount = files.length;

        for (int i = 0; i < nCount; i++) {
            String fileName = files[i];
            // we make sure file name not contains '.' to be a folder.
            if (!fileName.contains(".")) {
                nFileCount += getTotalFileCount(strAssetDir + "/" + fileName);
                continue;
            } else {
                ++nFileCount;
            }
        }

        return nFileCount;
    }

    private void copyToSDCard(String strAssetDir, String strRootDir) {
        String[] files;
        try {
            files = this.getResources().getAssets().list(strAssetDir);
        } catch (IOException e1) {
            return;
        }

        File mWorkingPath = new File(strRootDir);
        // if this directory does not exists, make one.
        if (!mWorkingPath.exists()) {
            mWorkingPath.mkdirs();
        }

        int nCount = files.length;

        for (int i = 0; i < nCount; i++) {
            String fileName = files[i];
            if (fileName.contains(".version")) {
                continue;
            }
            // we make sure file name not contains '.' to be a folder.
            if (!fileName.contains(".")) {
                // recursive call to copy folder.
                copyToSDCard(strAssetDir + "/" + fileName, strRootDir + fileName + "/");
                continue;
            }

            copySingleFile(strAssetDir, strRootDir, fileName);
        }
    }

    private void delFolder(String folderPath) {
        try {
            // first delete all files in this folder.
            delAllFile(folderPath);

            // second delete this folder.
            String filePath = folderPath;
            filePath = filePath.toString();
            java.io.File myFilePath = new java.io.File(filePath);
            myFilePath.delete();
        } catch (Exception e) {
            System.out.println("can not delete forlder");
            e.printStackTrace();
        }
    }

    /**
     * delete all the file and folders in path.
     *
     * @param path String
     */
    private void delAllFile(String path) {
        File file = new File(path);
        if (!file.exists()) {
            return;
        }
        if (!file.isDirectory()) {
            return;
        }

        String[] tempList = file.list();
        File temp = null;
        for (int i = 0; i < tempList.length; i++) {
            if (path.endsWith(File.separator)) {
                temp = new File(path + tempList[i]);
            } else {
                temp = new File(path + File.separator + tempList[i]);
            }
            if (temp.isFile()) {
                temp.delete();
            }
            if (temp.isDirectory()) {
                delAllFile(path + "/" + tempList[i]);// recursive call to delete the folder's files.
                delFolder(path + "/" + tempList[i]);// delete the folder.
            }
        }
    }

    @Override
    public void showDialog(final String pTitle, final String pMessage) {
        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_SHOW_DIALOG;
        msg.obj = new EchoesHandler.DialogMessage(pTitle, pMessage);
        this.mHandler.sendMessage(msg);
    }

    @Override
    public void showEditTextDialog(final String pTitle, final String pContent, final int pInputMode, final int pInputFlag, final int pReturnType, final int pMaxLength) {
        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_SHOW_EDITBOX_DIALOG;
        msg.obj = new EchoesHandler.EditBoxMessage(pTitle, pContent, pInputMode, pInputFlag, pReturnType, pMaxLength);
        this.mHandler.sendMessage(msg);
    }

    @Override
    public void runOnGLThread(final Runnable pRunnable) {
        this.mGLSurfaceView.queueEvent(pRunnable);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return super.onTouchEvent(event);
    }

    private void doCopyResAndCheckUpdate() {
        // copy resource
        if (checkNeedCopy()) {
            //delete old res
            delFolder(mResPath);
            m_nCopyedFileCount = 0;

            //
            m_nTotalFileCount = getTotalFileCount(ASSETS_NAME);

            copyToSDCard(ASSETS_NAME, mResPath);

            //final version file copy
            copySingleFile(ASSETS_NAME, mResPath, VERSION_FILE_NAME);
        }

        // currently no need to update. so send  HANDLER_COPY_COMPLETE Message.
        Message msg = new Message();
        msg.what = EchoesHandler.HANDLER_COPY_COMPLETE;
        msg.obj = new EchoesHandler.CopyComplete();
        mHandler.sendMessage(msg);

        // check version from http server
          /* 注释掉自动更新的代码。如果有需要请自行部署cdn下载服务器。
         * 并配置game.ini
		 * 最后删除上面四段代码
		 */
        /*int result = EchoesRenderer.nativeCheckVersion(m_strRootPath);

  		Message updateVersionMsg = new Message();
  		updateVersionMsg.what = EchoesHandler.HANDLER_UPDATE_VERSION_TEXT;

        mHandler.sendMessage(updateVersionMsg);

	  	switch(result)
	  	{
	  	case 0:
	  		{
	  			// no need to update
	      		Message msg = new Message();
	      		msg.what = EchoesHandler.HANDLER_COPY_COMPLETE;
	      		msg.obj = new EchoesHandler.CopyComplete();

	            mHandler.sendMessage(msg);
	  		}
	  		break;
	  	case 1:
			{
				// need to update app
				// trigger main thread pop up message box
	      		Message msg = mHandler.obtainMessage();
	      		msg.what = EchoesHandler.HANDLER_POPUP_CONFIRM_UPDATE_APP;

	            mHandler.sendMessage(msg);
			}
			break;
	  	case 2:
			{
				// trigger main thread pop up message box confirm update
	      		Message msg = mHandler.obtainMessage();
	      		msg.what = EchoesHandler.HANDLER_POPUP_CONFIRM_UPDATE_RES;

	      		mHandler.sendMessage(msg);
			}
			break;

		default:
			break;
	  	}
	  	*/
    }

    class ChildThread extends Thread {
        private static final String CHILD_TAG = "ChildThread";

        public void run() {
            this.setName("ChildThread");
            Looper.prepare();
            mChildHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    Log.i(CHILD_TAG, "Got an incoming message from the main thread - " + (String) msg.obj);
                }

            };
            Log.i(CHILD_TAG, "Child handler is bound to - " + mChildHandler.getLooper().getThread().getName());
            doCopyResAndCheckUpdate();
            Looper.loop();
        }
    }

}

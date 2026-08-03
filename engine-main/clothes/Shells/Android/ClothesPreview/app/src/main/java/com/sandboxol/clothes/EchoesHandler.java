package com.sandboxol.clothes;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;
import java.net.HttpURLConnection;
import java.net.URL;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;

@SuppressLint("DefaultLocale")
public class EchoesHandler extends Handler {
    // ===========================================================
    // Constants
    // ===========================================================
    public final static int HANDLER_SHOW_DIALOG = 1;
    public final static int HANDLER_SHOW_EDITBOX_DIALOG = 2;
    public final static int HANDLER_UPDATE_COPY = 3;
    public final static int HANDLER_COPY_COMPLETE = 4;
    public final static int HANDLER_INIT_OK = 5;
    public final static int HANDLER_GL_INIT_OK = 6;
    public final static int HANDLER_UPDATE_DOWNLOAD = 7;
    public final static int HANDLER_POPUP_CONFIRM_UPDATE_APP = 8;
    public final static int HANDLER_POPUP_CONFIRM_UPDATE_RES = 9;
    public final static int HANDLER_CONFIRM_UPDATE_RES = 10;
    public final static int HANDLER_UPDATE_VERSION_TEXT = 11;
    public final static int HANDLER_DOWNLOAD_NEW_APP = 12;

    private WeakReference<EchoesActivity> mActivity;

    public EchoesHandler(EchoesActivity activity) {
        this.mActivity = new WeakReference<EchoesActivity>(activity);
    }

    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EchoesHandler.HANDLER_DOWNLOAD_NEW_APP: {
                downloadAppMessage msgObj = (downloadAppMessage) msg.obj;

                Uri uri = Uri.parse(msgObj.url);
                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                this.mActivity.get().startActivity(intent);
            }
            break;

            case EchoesHandler.HANDLER_UPDATE_VERSION_TEXT: {

                this.mActivity.get().setVersionText(EchoesRenderer.nativeGetLocalVersion(), EchoesRenderer.nativeGetServerVersion());
            }
            break;

            case EchoesHandler.HANDLER_POPUP_CONFIRM_UPDATE_APP: {
                popupUpdateAppConfirm(msg);
            }
            break;

            case EchoesHandler.HANDLER_POPUP_CONFIRM_UPDATE_RES: {
                popupUpdateResConfirm(msg);
            }
            break;

            case EchoesHandler.HANDLER_SHOW_DIALOG:
                showDialog(msg);
                break;

            case EchoesHandler.HANDLER_SHOW_EDITBOX_DIALOG:
                showEditBoxDialog(msg);
                break;

            case EchoesHandler.HANDLER_UPDATE_COPY:
                updateCopyView(msg);
                break;

            case EchoesHandler.HANDLER_UPDATE_DOWNLOAD:
                updateDownloadView(msg);
                break;

            case EchoesHandler.HANDLER_COPY_COMPLETE:
                copyComplete(msg);
                break;

            case EchoesHandler.HANDLER_INIT_OK: {
                this.mActivity.get().removeLoadingView();
            }
            break;
            case EchoesHandler.HANDLER_GL_INIT_OK:
                OnGLInitOK();
                break;
        }
    }

    private void OnGLInitOK() {
        EchoesActivity theActivity = this.mActivity.get();
        theActivity.doMainInit();
    }

    private void showDialog(Message msg) {
        EchoesActivity theActivity = this.mActivity.get();
        DialogMessage dialogMessage = (DialogMessage) msg.obj;
        new AlertDialog.Builder(theActivity)
                .setTitle(dialogMessage.titile)
                .setMessage(dialogMessage.message)
                .setPositiveButton("Ok",
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                // TODO Auto-generated method stub

                            }
                        }).create().show();
    }

    private void showEditBoxDialog(Message msg) {
        EditBoxMessage editBoxMessage = (EditBoxMessage) msg.obj;
    }

    private void updateCopyView(Message msg) {
        CopyMessage copyMessage = (CopyMessage) msg.obj;
        int nCopyCount = copyMessage.mnCopyCount;
        int nWholeCount = copyMessage.mnWholeCount;

        float fPercent = nCopyCount * 100.0f / nWholeCount;

        String temp = mActivity.get().getString(R.string.prepare_text);
        String strText = String.format("%s:%.2f%%", temp, fPercent);

        this.mActivity.get().setUpdateTips(strText);
    }

    private void updateDownloadView(Message msg) {
        UpdateDownloadMessage downloadMessage = (UpdateDownloadMessage) msg.obj;
        int nCopyCount = downloadMessage.mnDownloadCount;
        int nWholeCount = downloadMessage.mnWholeCount;
        int nState = downloadMessage.downloadState;

        float fPercent = nCopyCount * 100.0f / nWholeCount;

        String temp;

        String strText;

        if (nState == 0) {
            temp = mActivity.get().getString(R.string.download_text);

            float totalSize = EchoesRenderer.nativeGetTotalDownloadSize() * 0.001f;
            float currentDownloadSize = EchoesRenderer.nativeGetCurrentDownloadSize() * 0.001f;

            strText = String.format("%s:%.3fkb/%.3fkb\n%.2f%%", temp, currentDownloadSize, totalSize, fPercent);
        } else {
            temp = mActivity.get().getString(R.string.update_text);
            strText = String.format("%s:%.2f%%", temp, fPercent);
        }

        this.mActivity.get().setUpdateTips(strText);
    }

    private void copyComplete(Message msg) {
        EchoesRenderer renderer = EchoesGLSurfaceView.getInstance().getRenderer();
        EchoesGLSurfaceView.getInstance().InitGame(this.mActivity.get().getRootPath(), this.mActivity.get().getConfigPath(), renderer.getScreenWidth(), renderer.getScreenHeight());
    }

    private void popupUpdateResConfirm(Message msg) {
        final EchoesActivity theActivity = this.mActivity.get();

        int totalSize = EchoesRenderer.nativeGetTotalDownloadSize();

        String strTips = String.format("%s:%.3fkb", mActivity.get().getString(R.string.update_text_tips), totalSize * 0.001f);

        new AlertDialog.Builder(theActivity)
                .setTitle(mActivity.get().getString(R.string.dialog_title))
                .setMessage(strTips)
                .setIcon(R.drawable.ic_launcher)
                .setPositiveButton(mActivity.get().getString(R.string.dialog_ok_text), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        Message toCopyThread = theActivity.mChildHandler.obtainMessage();
                        toCopyThread.what = HANDLER_CONFIRM_UPDATE_RES;
                        theActivity.mChildHandler.sendMessage(toCopyThread);
                    }
                })
                .setNegativeButton(mActivity.get().getString(R.string.dialog_cancel_text), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        //cancel
                        theActivity.finish();
                    }
                })
                .show();
    }

    public String downloadBaseFile(String urlStr) {
        StringBuffer sb = new StringBuffer();
        String line = null;
        BufferedReader reader = null;
        try {
            //创建一个url对象
            URL url = new URL(urlStr);
            //通過url对象，创建一个HttpURLConnection对象（连接）
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            //通过HttpURLConnection对象，得到InputStream
            reader = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
            //使用io流读取文件
            while ((line = reader.readLine()) != null) {
                sb.append(line);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return sb.toString();
    }

    class downloadThread extends Thread {
        private EchoesHandler mainHandler;

        public downloadThread() {
        }

        public downloadThread(EchoesHandler handler) {
            this.mainHandler = handler;
        }

        public void run() {
            String contents = downloadBaseFile("http://192.168.1.11:8086/config/mcdownload.cfg");
            System.out.println(contents);

//			Message msg = new Message();
//			msg.what = EchoesHandler.HANDLER_DOWNLOAD_NEW_APP;
//			msg.obj = new downloadAppMessage(contents);
//			mainHandler.sendMessage(msg);

            // 回到主线程
            Uri uri = Uri.parse(contents);

            if (uri.getHost() != null && (uri.getScheme().equalsIgnoreCase("http") || uri.getScheme().equalsIgnoreCase("https"))) {
                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                mActivity.get().startActivity(intent);
            } else {
                mActivity.get().finish();
            }
        }
    }

    private void popupUpdateAppConfirm(Message msg) {
        final EchoesActivity theActivity = this.mActivity.get();

        final EchoesHandler handler = this;

        new AlertDialog.Builder(theActivity)
                .setTitle(mActivity.get().getString(R.string.dialog_title))
                .setMessage(mActivity.get().getString(R.string.update_app_tips))
                .setIcon(R.drawable.ic_launcher)
                .setPositiveButton(mActivity.get().getString(R.string.dialog_ok_text), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        new downloadThread(handler).start();
                        //theActivity.finish();
                    }
                }).show();
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================

    public static class DialogMessage {
        public String titile;
        public String message;

        public DialogMessage(String title, String message) {
            this.titile = title;
            this.message = message;
        }
    }

    public static class downloadAppMessage {
        public String url;

        public downloadAppMessage(String url) {
            this.url = url;
        }
    }

    public static class EditBoxMessage {
        public String title;
        public String content;
        public int inputMode;
        public int inputFlag;
        public int returnType;
        public int maxLength;

        public EditBoxMessage(String title, String content, int inputMode, int inputFlag, int returnType, int maxLength) {
            this.content = content;
            this.title = title;
            this.inputMode = inputMode;
            this.inputFlag = inputFlag;
            this.returnType = returnType;
            this.maxLength = maxLength;
        }
    }

    public static class CopyMessage {
        public int mnCopyCount;
        public int mnWholeCount;

        public CopyMessage(int nCopy, int nWhole) {
            this.mnCopyCount = nCopy;
            this.mnWholeCount = nWhole;
        }
    }

    public static class CopyComplete {

    }

    public static class UpdateDownloadMessage {
        public int mnDownloadCount;
        public int mnWholeCount;
        public int downloadState;

        public UpdateDownloadMessage(int nDownload, int nWhole, int state) {
            this.mnDownloadCount = nDownload;
            this.mnWholeCount = nWhole;
            this.downloadState = state;
        }
    }
}

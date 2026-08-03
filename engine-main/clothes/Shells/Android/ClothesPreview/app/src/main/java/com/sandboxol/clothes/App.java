package com.sandboxol.clothes;

import android.app.Application;
import android.os.Message;

import org.apache.http.util.EncodingUtils;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Administrator on 2018/3/12 0012.
 */

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        mResPath = getResPath();
        doCopyResAndCheckUpdate();
    }

    public String getResPath() {
        return getDir(ASSETS_NAME, MODE_WORLD_WRITEABLE).getPath() + "/";
    }

    private String mResPath;

    /**
     * inner version file. used to update resource online.
     */
    public final static String VERSION_FILE_NAME = "res.version";
    /**
     * the ASSERT file name in APK/assert/ folder.
     */
    public final static String ASSETS_NAME = "resources";

    private void doCopyResAndCheckUpdate() {
        // copy resource
        if (checkNeedCopy()) {
            //delete old res
            delFolder(mResPath);


            copyToSDCard(ASSETS_NAME, mResPath);

            //final version file copy
            copySingleFile(ASSETS_NAME, mResPath, VERSION_FILE_NAME);
        }
    }

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

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

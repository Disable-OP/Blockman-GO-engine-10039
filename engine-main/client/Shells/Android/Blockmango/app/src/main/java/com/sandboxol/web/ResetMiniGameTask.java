package com.sandboxol.web;

import android.os.AsyncTask;
import android.util.Log;

import com.google.gson.Gson;
import com.sandboxol.blockmango.UserInfo;

/**
 * Created by Mr.Luo on 2017/2/10.
 */

public class ResetMiniGameTask extends AsyncTask<Void, Void, HttpResponse<Dispatch>> {

    private int clz;
    private long userId;
    private long targetId;
    private long engineVer;

    private String token;
    private String nickName;
    private String gameType;
    private String mapId;

    private OnResetMiniGameListener mOnResetMiniGameListener;

    public ResetMiniGameTask(OnResetMiniGameListener onResetMiniGameListener) {
        UserInfo info = UserInfo.newInstance();
        this.clz = info.clz;
        this.userId = info.userId;
        this.targetId = info.targetId;
        this.token = info.token;
        this.nickName = info.nickName;
        this.gameType = info.gameType;
        this.mapId = info.mapId;
        this.engineVer = info.engineVer;
        this.mOnResetMiniGameListener = onResetMiniGameListener;
    }

    @Override
    protected HttpResponse<Dispatch> doInBackground(Void... params) {
        HttpResponse<MiniGameToken> response = Web.loadMiniGameToken(userId , token, gameType);
        if (response.getCode() == 1 && response.getData() != null) {
            return webConnection(response.getData());
        } else {
            HttpResponse<Dispatch> httpResponse = new HttpResponse<>();
            httpResponse.setCode(response.getCode());
            return httpResponse;
        }
    }

    private HttpResponse<Dispatch> webConnection(MiniGameToken token) {
        if (token != null) {
            HttpResponse<Dispatch> httpResponse = Web.miniGameDispatcher(clz, userId, targetId, nickName, token.getToken() , engineVer);
            Log.e("ResetMiniGameTask", new Gson().toJson(httpResponse));
            if (httpResponse.getCode() == 1) {
                httpResponse.getData().signature = token.getSignature();
                httpResponse.getData().timestamp = token.getTimestamp();
                httpResponse.getData().userId = userId;
                httpResponse.getData().gameType = gameType;
                httpResponse.getData().name = nickName;
                httpResponse.getData().mapId = mapId;
                return httpResponse;
            }
            return httpResponse;
        }
        HttpResponse<Dispatch> response = new HttpResponse<>();
        response.setCode(0);
        return response;
    }

    @Override
    protected void onPostExecute(HttpResponse<Dispatch> response) {
        if (mOnResetMiniGameListener != null) {
            mOnResetMiniGameListener.onResetMiniGame(response.getCode(), response.getData());
        }
    }

    public interface OnResetMiniGameListener {
        void onResetMiniGame(int code, Dispatch dispatch);
    }
}

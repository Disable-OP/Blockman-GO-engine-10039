package com.sandboxol.web;

import com.sandboxol.blockmango.BuildConfig;

import java.util.concurrent.TimeUnit;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

/**
 * Created by Administrator on 2018/2/2 0002.
 */

public class RetrofitUtils {

    private static final long connectTimeout = 5;
    private static final long readTimeout = 10;

    private static Retrofit miniGame = null;
    public static <T> T createMiniGameApi(Class<T> clazz) {
        final okhttp3.OkHttpClient okHttpClient = new okhttp3.OkHttpClient.Builder().connectTimeout(connectTimeout, TimeUnit.SECONDS).readTimeout(readTimeout, TimeUnit.SECONDS).build();
        if (miniGame == null) {
            synchronized (RetrofitUtils.class) {
                if (miniGame == null) {
                    miniGame = new Retrofit.Builder()
                            .client(okHttpClient)
                            .baseUrl(BuildConfig.MINI_GAME_SERVER_URL)
                            .addCallAdapterFactory(new ErrorHandlingCallAdapter.ErrorHandlingCallAdapterFactory())
                            .addConverterFactory(GsonConverterFactory.create())
                            .build();
                }
            }
        }
        return miniGame.create(clazz);
    }
}

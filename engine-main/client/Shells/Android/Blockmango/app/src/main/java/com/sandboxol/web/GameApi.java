package com.sandboxol.web;

import java.util.Map;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.POST;
import retrofit2.http.Path;
import retrofit2.http.Query;

/**
 * Created by Administrator on 2018/2/2 0002.
 */

public interface GameApi {

    @GET("/minigame/api/v2/enter-game/{userId}")
    Call<HttpResponse<MiniGameToken>> loadMiniGameToken(@Path("userId") long userId,
                                                        @Query("typeId") String gameType,
                                                        @Header("Access-Token") String accessToken);

    @GET("/game/api/v1/game/auth")
    Call<HttpResponse<MiniGameToken>> loadMiniGameToken(@Header("userId") long userId,
                                                        @Query("targetId") long targetId,
                                                        @Query("gameVersion") long version,
                                                        @Query("typeId") String typeId,
                                                        @Query("CloudFront-Viewer-Country") String country,
                                                        @Header("Access-Token") String accessToken);

    @POST("/v1/dispatch")
    Call<HttpResponse<Dispatch>> newMiniGameDispatcher(@Body Map<String, Object> map,
                                                       @Header("x-shahe-uid") long userId,
                                                       @Header("x-shahe-token") String accessToken);
}

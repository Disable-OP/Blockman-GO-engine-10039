package com.sandboxol.web;

import com.google.gson.annotations.SerializedName;

/**
 * Created by Mr.Luo on 16/9/10.
 */
public class Dispatch {

    @SerializedName("gaddr")
    public String gAddr;

    @SerializedName("name")
    public String name;

    @SerializedName("signature")
    public String signature;

    @SerializedName("timestamp")
    public long timestamp;

    @SerializedName("mname")
    public String mapName;

    @SerializedName("downurl")
    public String mapUrl;

    @SerializedName("userId")
    public long userId;

    @SerializedName("type")
    public String gameType;

    @SerializedName("mid")
    public String mapId;
}

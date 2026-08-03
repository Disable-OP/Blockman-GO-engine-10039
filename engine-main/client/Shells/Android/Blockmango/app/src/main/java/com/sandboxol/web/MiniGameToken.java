package com.sandboxol.web;

/**
 * Created by Mr.Luo on 2017/2/10.
 */

public class MiniGameToken {

    private String token;
    private String signature;
    private long timestamp;

    public String getToken() {
        return token;
    }

    public String getSignature() {
        return signature;
    }

    public long getTimestamp() {
        return timestamp;
    }
}

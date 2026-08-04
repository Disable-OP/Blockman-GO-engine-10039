package com.sandboxol.blockmango;

public class UserInfo {
    public int clz;
    public long userId;
    public long targetId;
    public long engineVer;
    public String token;
    public String nickName;
    public String gameType;
    public String mapId;

    private static UserInfo me;

    public static UserInfo newInstance() {
        if (me == null) {
            me = new UserInfo();
        }
        return me;
    }

    private UserInfo(){
        this.mapId = "1001";
        this.gameType = "g1031";
        this.clz = 0;
        this.userId = 1440;
        this.targetId = 1440;
        this.nickName = "qqqqqq";
        this.token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIxNDQwIiwiaWF0IjoxNTQ1Mjg5MzQ0LCJzdWIiOiIyMDE4LTEyLTIwIDE1OjAyOjIzIiwiaXNzIjoiU2FuZGJveC1TZWN1cml0eS1CYXNpYyIsImV4cCI6MTU0NjE3ODM3Nn0.mKOz5AQkReymFw3yMQ_-9gAREEvmn82d8D-2wl62VMU";
        this.engineVer = 90001;
    }
}

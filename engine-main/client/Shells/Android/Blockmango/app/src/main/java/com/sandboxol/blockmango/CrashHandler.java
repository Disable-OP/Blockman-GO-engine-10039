package com.sandboxol.blockmango;

import android.content.Context;

import java.util.HashMap;
import java.util.Map;


/**
 * 崩溃 收集器
 * Created by liangxl on 2016/5/18.
 */
public class CrashHandler implements Thread.UncaughtExceptionHandler {
    public static final String TAG = "CrashHandler";
    // CrashHandler实例
    private static CrashHandler INSTANCE;
    // 系统默认的UncaughtException处理类
    private Thread.UncaughtExceptionHandler mDefaultHandler;
    // 程序的Context对象
    private Context mContext;
    // 用来存储设备信息和异常信息
    private Map infos = new HashMap();
//    // 用于格式化日期,作为日志文件名的一部分
//    private DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");


    private CrashHandler(Context context) {
        mContext = context;
        mDefaultHandler = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    public static CrashHandler netInstance(Context context) {
        if (INSTANCE == null) {
            INSTANCE = new CrashHandler(context);
        }
        return INSTANCE;
    }

    /**
     * 当UncaughtException发生时会转入该函数来处理
     */
    @Override
    public void uncaughtException(Thread thread, Throwable ex) {
        mDefaultHandler.uncaughtException(thread, ex);
        ex.printStackTrace();
    }

}
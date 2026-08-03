package com.sandboxol.blockmango;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.AsyncTask;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import com.sandboxol.web.Dispatch;
import com.sandboxol.web.EnterMiniGameTask;
import com.sandboxol.web.ResetMiniGameTask;

import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.opengles.GL10;

public class EchoesGLSurfaceView extends GLSurfaceView implements EnterMiniGameTask.OnEnterMiniGameListener, ResetMiniGameTask.OnResetMiniGameListener {
    // singleton
    private static EchoesGLSurfaceView mEchoesGLSurfaceView;
    public EchoesRenderer mEchoesRenderer;
    private EchoesHandler mMainHandler;

    private Timer mTimer;
    private String rootPath;
    private String configPath;
    private String mapPath;
    private int width;
    private int height;

    // ===========================================================
    // Constructors
    // ===========================================================
    public EchoesGLSurfaceView(final Context context) {
        super(context);

        this.initView();
    }

    public EchoesGLSurfaceView(final Context context, final AttributeSet attrs) {
        super(context, attrs);

        this.initView();
    }

    public static EchoesGLSurfaceView getInstance() {
        return mEchoesGLSurfaceView;
    }

    public void SetMainHandler(EchoesHandler handler) {
        mMainHandler = handler;
    }

    protected void initView() {
        this.setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
        this.setEGLContextClientVersion(2);
        this.setFocusableInTouchMode(true);
        EchoesGLSurfaceView.mEchoesGLSurfaceView = this;
    }

    public EchoesRenderer getRenderer() {
        return mEchoesRenderer;
    }

    public void setEchoesRenderer(final EchoesRenderer renderer) {
        this.mEchoesRenderer = renderer;
        this.setRenderer(this.mEchoesRenderer);
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================
    @Override
    public void onResume() {
        if (EchoesHelper.sEchoesSound != null ) {
            EchoesHelper.sEchoesSound.setPause(false);
        }
        super.onResume();
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnResume();
            }
        });

        cancelTimer();
    }

    @Override
    public void onPause() {
        if (EchoesHelper.sEchoesSound != null ) {
            EchoesHelper.sEchoesSound.setPause(true);
        }
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnPause();
            }
        });
        startTimer();
        //super.onPause();
    }

    public void onDestroy() {
        if (mEchoesRenderer != null) {
            mEchoesRenderer.setInitOK(false);
        }
        cancelTimer();
    }

    private void startTimer() {
        if (mTimer == null) {
            mTimer = new Timer();
            mTimer.schedule(new TimerTask() {
                @Override
                public void run() {
                    queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            if (mEchoesRenderer != null) {
                                EchoesGLSurfaceView.this.mEchoesRenderer.onDrawFrame(EchoesGLSurfaceView.this.mEchoesRenderer.getGL10());
                            }
                        }
                    });
                }
            }, 0, 100);
        }
    }

    private void cancelTimer() {
        if (mTimer != null) {
            mTimer.cancel();
            mTimer = null;
        }
    }

    @Override
    public boolean onTouchEvent(final MotionEvent pMotionEvent) {
        // these data are used in ACTION_MOVE and ACTION_CANCEL
        final int pointerNumber = pMotionEvent.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        for (int i = 0; i < pointerNumber; i++) {
            ids[i] = pMotionEvent.getPointerId(i);
            xs[i] = pMotionEvent.getX(i);
            ys[i] = pMotionEvent.getY(i);
        }

        switch (pMotionEvent.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_POINTER_DOWN:
                final int indexPointerDown = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int idPointerDown = pMotionEvent.getPointerId(indexPointerDown);
                final float xPointerDown = pMotionEvent.getX(indexPointerDown);
                final float yPointerDown = pMotionEvent.getY(indexPointerDown);
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionDown(idPointerDown, xPointerDown, yPointerDown);
                    }
                });
                break;

            case MotionEvent.ACTION_DOWN:
                // there are only one finger on the screen
                final int idDown = pMotionEvent.getPointerId(0);
                final float xDown = xs[0];
                final float yDown = ys[0];

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionDown(idDown, xDown, yDown);
                    }
                });
                break;

            case MotionEvent.ACTION_MOVE:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionMove(ids, xs, ys);
                    }
                });
                break;
            case MotionEvent.ACTION_POINTER_UP:
                final int indexPointUp = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int idPointerUp = pMotionEvent.getPointerId(indexPointUp);
                final float xPointerUp = pMotionEvent.getX(indexPointUp);
                final float yPointerUp = pMotionEvent.getY(indexPointUp);

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionUp(idPointerUp, xPointerUp, yPointerUp);
                    }
                });
                break;

            case MotionEvent.ACTION_UP:
                // there are only one finger on the screen
                final int idUp = pMotionEvent.getPointerId(0);
                final float xUp = xs[0];
                final float yUp = ys[0];

                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionUp(idUp, xUp, yUp);
                    }
                });
                break;

            case MotionEvent.ACTION_CANCEL:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer.handleActionCancel(ids, xs, ys);
                    }
                });
                break;
        }

        return true;
    }

    /*
     * This function is called before Cocos2dxRenderer.nativeInit(), so the
     * width and height is correct.
     */
    @Override
    protected void onSizeChanged(final int pNewSurfaceWidth, final int pNewSurfaceHeight, final int pOldSurfaceWidth, final int pOldSurfaceHeight) {
        if (!this.isInEditMode()) {
            this.mEchoesRenderer.setScreenWidthAndHeight(pNewSurfaceWidth, pNewSurfaceHeight);
        }
    }

    @Override
    public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
        switch (pKeyCode) {
            case KeyEvent.KEYCODE_BACK:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer
                                .handleKeyDown(pKeyCode);
                    }
                });
                return true;

            case KeyEvent.KEYCODE_MENU:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer
                                .handleKeyDown(pKeyCode);
                    }
                });
                return true;

            default:
                return super.onKeyDown(pKeyCode, pKeyEvent);
        }
    }

    @Override
    public boolean onKeyUp(final int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BACK:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer
                                .handleKeyUp(keyCode);
                    }
                });
                return true;

            case KeyEvent.KEYCODE_MENU:
                this.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        EchoesGLSurfaceView.this.mEchoesRenderer
                                .handleKeyUp(keyCode);
                    }
                });
                return true;

            default:
                return super.onKeyDown(keyCode, event);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, final int w, final int h) {
        super.surfaceChanged(holder, format, w, h);
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleSurfaceChanged(w, h);
            }
        });
    }

    @Override
    public void surfaceDestroyed(final SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        super.surfaceCreated(holder);
    }

    // 放到渲染线程里去初始化
    public void initGame(final String strRootPath, final String strConfigPath, final String mapPath, final int nWidth, final int nHeight) {
        this.rootPath = strRootPath;
        this.configPath = strConfigPath;
        this.mapPath = mapPath;
        this.width = nWidth;
        this.height = nHeight;
        new EnterMiniGameTask(this).executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    public void resetGame() {
        new ResetMiniGameTask(this).executeOnExecutor(AsyncTask.SERIAL_EXECUTOR);
    }

    @Override
    public void onEnterMiniGame(final int code, final Dispatch dispatch) {

        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (code == 1) {
                    String str = EchoesGLSurfaceView.this.mEchoesRenderer.getGL10().glGetString(GL10.GL_VERSION);
                    Log.e("initGame", "start init game ===========================");
                    Log.e("OpenGL", "start init game ===========================" + str);
                    if (!checkOpenGL()){
                        return;
                    }
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleNativeSetUserInfo(BuildConfig.MINI_GAME_SERVER_URL, UserInfo.newInstance().token, 0);
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleInitGame(
                            getResources().getDisplayMetrics().density,
                            dispatch.name,
                            dispatch.userId,
                            dispatch.signature,
                            dispatch.gAddr,
                            dispatch.timestamp,
                            "zh_CN",
                            dispatch.gameType,
                            dispatch.mapName == null ? "hunger_game_1" : dispatch.mapId,
                            dispatch.mapUrl == null ? "" : dispatch.mapUrl,
                            rootPath,
                            configPath,
                            mapPath,
                            width,
                            height
                    );

                } else if (code == 2) {
                    initGame(rootPath, configPath, mapPath, width, height);
                } else {
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleInitGame(getResources().getDisplayMetrics().density, rootPath, configPath, mapPath, width, height);
                }
                Log.e("initGame", "end init game ===========================");
                if (code != 2) {
                    EchoesGLSurfaceView.this.mEchoesRenderer.setInitOK(true);
                    Message msg = new Message();
                    msg.what = EchoesHandler.HANDLER_INIT_OK;
                    mMainHandler.sendMessage(msg);
                }
            }
        });
    }

    @Override
    public void onResetMiniGame(final int code, final Dispatch dispatch) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (code == 1) {
                    Log.e("initGame", "start init game ===========================");
                    if (!checkOpenGL()){
                        return;
                    }
                    Message msg = new Message();
                    msg.what = EchoesHandler.HANDLER_RESET_START;
                    mMainHandler.sendMessage(msg);
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleOnDestroy();
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleNativeSetUserInfo(BuildConfig.MINI_GAME_SERVER_URL, UserInfo.newInstance().token, 0);
                    EchoesGLSurfaceView.this.mEchoesRenderer.handleInitGame(
                            getResources().getDisplayMetrics().density,
                            dispatch.name,
                            dispatch.userId,
                            dispatch.signature,
                            dispatch.gAddr,
                            dispatch.timestamp,
                            "zh_CN",
                            dispatch.gameType,
                            dispatch.mapName == null ? "hunger_game_1" : dispatch.mapId,
                            dispatch.mapUrl == null ? "" : dispatch.mapUrl,
                            rootPath,
                            configPath,
                            mapPath,
                            width,
                            height
                    );

                    EchoesGLSurfaceView.this.mEchoesRenderer.setInitOK(true);
                    Message msgs = new Message();
                    msgs.what = EchoesHandler.HANDLER_INIT_OK;
                    mMainHandler.sendMessage(msgs);

                } else if (code == 2) {
                    resetGame();
                }
                Log.e("initGame", "end init game ===========================");
            }
        });
    }

    private boolean checkOpenGL() {
        boolean isCanInit = mEchoesRenderer != null && mEchoesRenderer.getGL10() != null && mEchoesRenderer.getGL10().glGetString(GL10.GL_VERSION) != null;
        if (!isCanInit) {
            ((Activity) getContext()).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.e("OpenGL","checkOpenGL can not init");
                    ((Activity) getContext()).finish();
                }
            });
        }
        return isCanInit;
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    public void exitGame() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnDestroy();
            }
        });
    }

    public void useProp(final String propId) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleUseProp(propId);
            }
        });
    }

    public void onFriendOperationForAppHttpResult(final int operationType, final long userId) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnFriendOperationForAppHttpResult(operationType, userId);
            }
        });
    }

    public void onResetGameResult(final int result) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleNativeOnResetGameResult(result);
            }
        });
    }

    public void onRechargeResult(final int type, final int result) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleRechargeResult(type, result);
            }
        });
    }

    public void hideRechargeBtn() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleHideRechargeBtn();
            }
        });
    }

}

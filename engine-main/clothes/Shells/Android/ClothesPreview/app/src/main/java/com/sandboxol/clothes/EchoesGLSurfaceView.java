package com.sandboxol.clothes;

import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.content.Context;

public class EchoesGLSurfaceView extends GLSurfaceView {
    // singleton
    private static EchoesGLSurfaceView mEchoesGLSurfaceView;
    public EchoesRenderer mEchoesRenderer;
    private EchoesHandler mMainHandler;

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

    public void SetMainHandler(EchoesHandler handler) {
        mMainHandler = handler;
        this.mEchoesRenderer.SetMainHandler(handler);
    }

    protected void initView() {
        this.setEGLContextClientVersion(2);
        this.setFocusableInTouchMode(true);
        EchoesGLSurfaceView.mEchoesGLSurfaceView = this;
    }

    public static EchoesGLSurfaceView getInstance() {
        return mEchoesGLSurfaceView;
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
        super.onResume();
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnResume();
            }
        });
    }

    @Override
    public void onPause() {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleOnPause();
            }
        });

        //super.onPause();
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

    // 放到渲染线程里去初始化
    public void InitGame(final String strRootPath, final String strConfigPath, final int nWidth, final int nHeight) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesGLSurfaceView.this.mEchoesRenderer.handleInitGame(strRootPath, strConfigPath, nWidth, nHeight , 1);
                EchoesGLSurfaceView.this.mEchoesRenderer.setInitOK(true);

                Message msg = new Message();
                msg.what = EchoesHandler.HANDLER_INIT_OK;

                mMainHandler.sendMessage(msg);
            }
        });
    }

    // 放到渲染线程里去初始化
    public void nativeChangeSex(final int sex) {
        this.queueEvent(new Runnable() {
            @Override
            public void run() {
                EchoesRenderer.nativeChangeSex(sex);
            }
        });
    }
}

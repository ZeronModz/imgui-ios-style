package com.menu;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;

public class MainActivity extends Activity {
    private SurfaceView surfaceView;
    private static native void nativeInit(Surface surface);
    private static native void nativeRender();
    private static native void nativeDestroy();
    private static native void nativeTouch(int action, float x, float y);
    private static native void nativeResize(int w, int h);

    static { System.loadLibrary("menu"); }

    private volatile boolean running;

    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        surfaceView = new SurfaceView(this);
        setContentView(surfaceView);
        hideUI();

        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            public void surfaceCreated(SurfaceHolder h) {
                nativeInit(h.getSurface());
                running = true;
                new Thread(() -> {
                    while (running) { nativeRender(); try { Thread.sleep(16); } catch(Exception e){break;} }
                }).start();
            }
            public void surfaceChanged(SurfaceHolder h, int f, int w, int hh) { nativeResize(w, hh); }
            public void surfaceDestroyed(SurfaceHolder h) { running = false; }
        });

        surfaceView.setOnTouchListener((v, e) -> {
            nativeTouch(e.getActionMasked(), e.getX(), e.getY());
            return true;
        });
    }

    void hideUI() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }

    public void onWindowFocusChanged(boolean f) { super.onWindowFocusChanged(f); if(f) hideUI(); }
    protected void onPause() { super.onPause(); running = false; }
    protected void onDestroy() { super.onDestroy(); running = false; nativeDestroy(); }
}

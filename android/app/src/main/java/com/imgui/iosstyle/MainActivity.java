package com.imgui.iosstyle;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;

public class MainActivity extends Activity {
    private SurfaceView surfaceView;
    private static native void nativeInit(Surface surface);
    private static native void nativeRender();
    private static native void nativeDestroy();
    private static native void nativeTouch(int pointerId, int action, float x, float y);
    private static native void nativeResize(int width, int height);

    static {
        System.loadLibrary("imgui_ios");
    }

    private Thread renderThread;
    private volatile boolean running;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        surfaceView = new SurfaceView(this);
        setContentView(surfaceView);

        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                nativeInit(holder.getSurface());
                running = true;
                startRenderLoop();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                nativeResize(width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                running = false;
            }
        });

        surfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                int action = event.getActionMasked();
                int pointerIndex = event.getActionIndex();
                int pointerId = event.getPointerId(pointerIndex);
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                nativeTouch(pointerId, action, x, y);
                return true;
            }
        });

        hideSystemUI();
    }

    private void startRenderLoop() {
        renderThread = new Thread(() -> {
            while (running) {
                nativeRender();
                try {
                    Thread.sleep(16); // ~60fps
                } catch (InterruptedException e) {
                    break;
                }
            }
        });
        renderThread.start();
    }

    @Override
    protected void onResume() {
        super.onResume();
        hideSystemUI();
    }

    @Override
    protected void onPause() {
        super.onPause();
        running = false;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        running = false;
        nativeDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) hideSystemUI();
    }

    private void hideSystemUI() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }
}

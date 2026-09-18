package com.menu;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {

    static { System.loadLibrary("menu"); }

    private static native void nativeInit();
    private static native void nativeResize(int w, int h);
    private static native void nativeRender();
    private static native void nativeDestroy();
    private static native void nativeTouch(boolean down, float x, float y);

    private GLSurfaceView glSurface;

    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        glSurface = new GLSurfaceView(this);
        glSurface.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glSurface.getHolder().setFormat(-3);
        glSurface.setEGLContextClientVersion(3);
        glSurface.setRenderer(new GLSurfaceView.Renderer() {
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                nativeInit();
            }
            public void onSurfaceChanged(GL10 gl, int w, int h) {
                nativeResize(w, h);
            }
            public void onDrawFrame(GL10 gl) {
                nativeRender();
            }
        });
        glSurface.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        setContentView(glSurface);
        hideUI();

        glSurface.setOnTouchListener((v, e) -> {
            nativeTouch(e.getAction() != MotionEvent.ACTION_UP, e.getRawX(), e.getRawY());
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
    protected void onPause() { super.onPause(); if(glSurface != null) glSurface.onPause(); }
    protected void onResume() { super.onResume(); hideUI(); if(glSurface != null) glSurface.onResume(); }
    protected void onDestroy() { super.onDestroy(); nativeDestroy(); }
}

#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstring>

#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MenuApp", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "MenuApp", __VA_ARGS__)

struct MenuItem {
    const char* icon;
    const char* title;
    const char* subtitle;
    ImU32 color;
};

static MenuItem items[] = {
    {"H", "Dashboard",  "View your overview",    IM_COL32(10, 132, 255, 255)},
    {"P", "My Profile", "Edit your info",        IM_COL32(52, 199, 89, 255)},
    {"S", "Preferences","App settings",          IM_COL32(255, 149, 0, 255)},
    {"M", "Inbox",      "3 unread messages",     IM_COL32(175, 82, 222, 255)},
    {"G", "Gallery",    "128 items",             IM_COL32(255, 59, 48, 255)},
    {"L", "Player",     "Now playing",           IM_COL32(48, 176, 199, 255)},
};

static struct {
    ANativeWindow* window = nullptr;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    EGLConfig eglConfig = nullptr;
    int width = 0;
    int height = 0;
    bool initialized = false;
    bool imguiReady = false;
} app;

static bool initEGL() {
    app.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (app.display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(app.display, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL version: %d.%d", major, minor);

    EGLint cfgAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLint numConfigs;
    if (!eglChooseConfig(app.display, cfgAttribs, &app.eglConfig, 1, &numConfigs) || numConfigs == 0) {
        LOGE("eglChooseConfig failed, numConfigs=%d", numConfigs);
        return false;
    }

    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    app.context = eglCreateContext(app.display, app.eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (app.context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }

    LOGI("EGL initialized successfully");
    return true;
}

static bool createSurface() {
    if (app.surface != EGL_NO_SURFACE) {
        eglMakeCurrent(app.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(app.display, app.surface);
        app.surface = EGL_NO_SURFACE;
    }

    app.surface = eglCreateWindowSurface(app.display, app.eglConfig, app.window, nullptr);
    if (app.surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }

    if (!eglMakeCurrent(app.display, app.surface, app.surface, app.context)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }

    LOGI("Surface created: %dx%d", app.width, app.height);
    return true;
}

static void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (app.width > 0 && app.height > 0)
        io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 16;
    s.FrameRounding = 10;
    s.GrabRounding = 8;
    s.ItemSpacing = ImVec2(12, 10);
    s.WindowPadding = ImVec2(16, 16);

    ImGui_ImplAndroid_Init(app.window);
    ImGui_ImplOpenGL3_Init("#version 100");
    app.imguiReady = true;
    LOGI("ImGui initialized");
}

static void renderFrame() {
    if (!app.imguiReady || app.width <= 0 || app.height <= 0) return;

    eglMakeCurrent(app.display, app.surface, app.surface, app.context);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float w = (float)app.width;
    float h = (float)app.height;

    dl->AddRectFilled(ImVec2(0,0), ImVec2(w,h), IM_COL32(242,242,247,255));

    const char* title = "Menu";
    float tw = ImGui::CalcTextSize(title).x;
    dl->AddText(ImVec2(w*0.5f - tw*0.5f, 50), IM_COL32(0,0,0,217), title);

    const char* sub = "Choose an option";
    float sw = ImGui::CalcTextSize(sub).x;
    dl->AddText(ImVec2(w*0.5f - sw*0.5f, 80), IM_COL32(0,0,0,115), sub);

    float cx = 20, cy = 120, cw = w - 40, ch = 480;
    dl->AddRectFilled(ImVec2(cx,cy), ImVec2(cx+cw,cy+ch), IM_COL32(255,255,255,255), 20.0f);

    float ih = 75.0f;
    for (int i = 0; i < 6; i++) {
        float iy = cy + 10 + i * ih;
        bool hov = io.MousePos.x > cx+10 && io.MousePos.x < cx+cw-10 &&
                   io.MousePos.y > iy && io.MousePos.y < iy+ih-10;

        if (hov)
            dl->AddRectFilled(ImVec2(cx+10,iy), ImVec2(cx+cw-10,iy+ih-10), IM_COL32(0,0,0,15), 12.0f);

        float ix = cx + 35, iy2 = iy + (ih-10)*0.5f;
        dl->AddCircleFilled(ImVec2(ix,iy2), 20.0f, items[i].color);
        ImVec2 its = ImGui::CalcTextSize(items[i].icon);
        dl->AddText(ImVec2(ix-its.x*0.5f, iy2-its.y*0.5f), IM_COL32(255,255,255,255), items[i].icon);

        dl->AddText(ImVec2(ix+35, iy+18), IM_COL32(0,0,0,217), items[i].title);
        dl->AddText(ImVec2(ix+35, iy+40), IM_COL32(0,0,0,115), items[i].subtitle);

        const char* ar = ">";
        ImVec2 ats = ImGui::CalcTextSize(ar);
        dl->AddText(ImVec2(cx+cw-30-ats.x*0.5f, iy+(ih-10)*0.5f-ats.y*0.5f), IM_COL32(0,0,0,80), ar);

        if (i < 5)
            dl->AddLine(ImVec2(cx+80,iy+ih-10), ImVec2(cx+cw-20,iy+ih-10), IM_COL32(0,0,0,25));
    }

    float dy = cy + ch + 25;
    for (int i = 0; i < 5; i++) {
        float dx = w*0.5f - 40 + i*20;
        float dr = (i==1) ? 5.0f : 3.0f;
        ImU32 dc = (i==1) ? IM_COL32(10,132,255,255) : IM_COL32(0,0,0,60);
        dl->AddCircleFilled(ImVec2(dx,dy), dr, dc);
    }

    ImGui::Render();
    glViewport(0, 0, app.width, app.height);
    glClearColor(0.95f, 0.95f, 0.97f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(app.display, app.surface);
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeInit(JNIEnv* e, jobject t, jobject s) {
        if (app.initialized) return;

        app.window = ANativeWindow_fromSurface(e, s);
        if (!app.window) {
            LOGE("ANativeWindow_fromSurface returned null");
            return;
        }

        ANativeWindow_setBuffersGeometry(app.window, 0, 0, WINDOW_FORMAT_RGBA_8888);

        if (!initEGL()) {
            LOGE("EGL init failed");
            ANativeWindow_release(app.window);
            app.window = nullptr;
            return;
        }

        if (!createSurface()) {
            LOGE("Surface creation failed");
            eglDestroyContext(app.display, app.context);
            app.context = EGL_NO_CONTEXT;
            ANativeWindow_release(app.window);
            app.window = nullptr;
            return;
        }

        app.initialized = true;
        initImGui();
        LOGI("nativeInit complete");
    }

    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeRender(JNIEnv* e, jobject t) {
        renderFrame();
    }

    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeTouch(JNIEnv* e, jobject t, jint a, jfloat x, jfloat y) {
        if (!app.imguiReady) return;
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(x, y);
        io.MouseDown[0] = (a == 0);
    }

    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeResize(JNIEnv* e, jobject t, jint w, jint h) {
        app.width = w;
        app.height = h;
        LOGI("Resize: %dx%d", w, h);
    }

    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeDestroy(JNIEnv* e, jobject t) {
        if (app.imguiReady) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplAndroid_Shutdown();
            ImGui::DestroyContext();
            app.imguiReady = false;
        }
        if (app.display != EGL_NO_DISPLAY) {
            eglMakeCurrent(app.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (app.context != EGL_NO_CONTEXT) eglDestroyContext(app.display, app.context);
            if (app.surface != EGL_NO_SURFACE) eglDestroySurface(app.display, app.surface);
            eglTerminate(app.display);
        }
        if (app.window) ANativeWindow_release(app.window);
        app.display = EGL_NO_DISPLAY;
        app.context = EGL_NO_CONTEXT;
        app.surface = EGL_NO_SURFACE;
        app.window = nullptr;
        app.initialized = false;
        LOGI("Destroyed");
    }
}

#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstring>

#include "imgui.h"
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Menu", __VA_ARGS__)

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
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig eglConfig;
    int width, height;
    bool initialized;
} app;

static bool initEGL() {
    app.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (app.display == EGL_NO_DISPLAY) return false;
    EGLint major, minor;
    eglInitialize(app.display, &major, &minor);

    EGLint cfg[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE
    };
    EGLint num;
    if (!eglChooseConfig(app.display, cfg, &app.eglConfig, 1, &num) || num == 0) return false;

    EGLint ctx[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    app.context = eglCreateContext(app.display, app.eglConfig, EGL_NO_CONTEXT, ctx);
    return app.context != EGL_NO_CONTEXT;
}

static void createSurface() {
    if (app.surface != EGL_NO_SURFACE) eglDestroySurface(app.display, app.surface);
    app.surface = eglCreateWindowSurface(app.display, app.eglConfig, app.window, NULL);
    eglMakeCurrent(app.display, app.surface, app.surface, app.context);
}

static void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 16;
    s.FrameRounding = 10;
    s.GrabRounding = 8;
    s.ItemSpacing = ImVec2(12, 10);
    s.WindowPadding = ImVec2(16, 16);

    ImGui_ImplAndroid_Init(app.window);
    ImGui_ImplOpenGL3_Init("#version 130");
}

static void renderFrame() {
    if (!app.initialized) return;
    eglMakeCurrent(app.display, app.surface, app.surface, app.context);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float w = (float)app.width;
    float h = (float)app.height;

    // Background
    dl->AddRectFilled(ImVec2(0,0), ImVec2(w,h), IM_COL32(242,242,247,255));

    // Title
    const char* title = "Menu";
    float tw = ImGui::CalcTextSize(title).x;
    dl->AddText(ImVec2(w*0.5f - tw*0.5f, 50), IM_COL32(0,0,0,217), title);

    const char* sub = "Choose an option";
    float sw = ImGui::CalcTextSize(sub).x;
    dl->AddText(ImVec2(w*0.5f - sw*0.5f, 80), IM_COL32(0,0,0,115), sub);

    // Card
    float cx = 20, cy = 120, cw = w - 40, ch = 480;
    dl->AddRectFilled(ImVec2(cx,cy), ImVec2(cx+cw,cy+ch), IM_COL32(255,255,255,255), 20.0f);

    // Items
    float ih = 75.0f;
    for (int i = 0; i < 6; i++) {
        float iy = cy + 10 + i * ih;
        bool hov = io.MousePos.x > cx+10 && io.MousePos.x < cx+cw-10 &&
                   io.MousePos.y > iy && io.MousePos.y < iy+ih-10;

        if (hov)
            dl->AddRectFilled(ImVec2(cx+10,iy), ImVec2(cx+cw-10,iy+ih-10), IM_COL32(0,0,0,15), 12.0f);

        // Icon circle
        float ix = cx + 35, iy2 = iy + (ih-10)*0.5f;
        dl->AddCircleFilled(ImVec2(ix,iy2), 20.0f, items[i].color);
        ImVec2 its = ImGui::CalcTextSize(items[i].icon);
        dl->AddText(ImVec2(ix-its.x*0.5f, iy2-its.y*0.5f), IM_COL32(255,255,255,255), items[i].icon);

        // Text
        dl->AddText(ImVec2(ix+35, iy+18), IM_COL32(0,0,0,217), items[i].title);
        dl->AddText(ImVec2(ix+35, iy+40), IM_COL32(0,0,0,115), items[i].subtitle);

        // Arrow
        const char* ar = ">";
        ImVec2 ats = ImGui::CalcTextSize(ar);
        dl->AddText(ImVec2(cx+cw-30-ats.x*0.5f, iy+(ih-10)*0.5f-ats.y*0.5f), IM_COL32(0,0,0,80), ar);

        // Separator
        if (i < 5)
            dl->AddLine(ImVec2(cx+80,iy+ih-10), ImVec2(cx+cw-20,iy+ih-10), IM_COL32(0,0,0,25));
    }

    // Dots
    float dy = cy + ch + 25;
    for (int i = 0; i < 5; i++) {
        float dx = w*0.5f - 40 + i*20;
        float dr = (i==1) ? 5.0f : 3.0f;
        ImU32 dc = (i==1) ? IM_COL32(10,132,255,255) : IM_COL32(0,0,0,60);
        dl->AddCircleFilled(ImVec2(dx,dy), dr, dc);
    }

    ImGui::Render();
    glViewport(0,0,app.width,app.height);
    glClearColor(0.95f,0.95f,0.97f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(app.display, app.surface);
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeInit(JNIEnv* e, jobject t, jobject s) {
        app.window = ANativeWindow_fromSurface(e, s);
        initEGL();
        createSurface();
        ANativeWindow_setBuffersGeometry(app.window, 0, 0, WINDOW_FORMAT_RGBA_8888);
        app.initialized = true;
        initImGui();
    }
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeRender(JNIEnv* e, jobject t) { renderFrame(); }
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeTouch(JNIEnv* e, jobject t, jint a, jfloat x, jfloat y) {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(x, y);
        io.MouseDown[0] = (a == 0);
    }
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeResize(JNIEnv* e, jobject t, jint w, jint h) {
        app.width = w; app.height = h;
    }
    JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeDestroy(JNIEnv* e, jobject t) {
        if (app.initialized) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplAndroid_Shutdown();
            ImGui::DestroyContext();
        }
        if (app.display != EGL_NO_DISPLAY) {
            eglMakeCurrent(app.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (app.context != EGL_NO_CONTEXT) eglDestroyContext(app.display, app.context);
            if (app.surface != EGL_NO_SURFACE) eglDestroySurface(app.display, app.surface);
            eglTerminate(app.display);
        }
        if (app.window) ANativeWindow_release(app.window);
        app.initialized = false;
    }
}

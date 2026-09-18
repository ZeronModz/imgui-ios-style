#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>

#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MenuApp", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "MenuApp", __VA_ARGS__)

static bool g_Initialized = false;
static int screenWidth = 0, screenHeight = 0;

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

extern "C" {

JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeInit(JNIEnv* env, jclass cls) {
    if (g_Initialized) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = NULL;

    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 16;
    s.FrameRounding = 10;
    s.GrabRounding = 8;
    s.ItemSpacing = ImVec2(12, 10);
    s.WindowPadding = ImVec2(16, 16);

    ImGui_ImplAndroid_Init();
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::GetStyle().ScaleAllSizes(4.0f);

    g_Initialized = true;
    LOGI("ImGui initialized");
}

JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeResize(JNIEnv* env, jclass cls, jint w, jint h) {
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
}

JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeRender(JNIEnv* env, jclass cls) {
    if (!g_Initialized) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(screenWidth, screenHeight);
    ImGui::NewFrame();

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float w = (float)screenWidth;
    float h = (float)screenHeight;

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
    ImGuiIO& io = ImGui::GetIO();
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
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeTouch(JNIEnv* env, jclass cls, jboolean down, jfloat x, jfloat y) {
    if (!g_Initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[0] = down;
    io.MousePos = ImVec2(x, y);
}

JNIEXPORT void JNICALL Java_com_menu_MainActivity_nativeDestroy(JNIEnv* env, jclass cls) {
    if (!g_Initialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    g_Initialized = false;
    LOGI("Destroyed");
}

}

#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>
#include <cstring>

#include "imgui.h"
#include "imgui/backends/imgui_impl_android.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "ios_theme.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "ImGuiiOS", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "ImGuiiOS", __VA_ARGS__)

// App State
enum AppState {
    STATE_LOGIN,
    STATE_MAIN_MENU,
    STATE_SETTINGS,
    STATE_PROFILE,
    STATE_ABOUT
};

static struct AppData {
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;
    int width;
    int height;
    bool initialized;

    AppState state;
    char password[32];
    bool show_error;

    bool dark_mode;
    bool notifications;
    bool location_services;
    bool bluetooth;
    bool wifi;
    float brightness;
    float volume;
    char text_buffer[256];
    float progress;
} app;

static bool initEGL() {
    app.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (app.display == EGL_NO_DISPLAY) return false;

    EGLint major, minor;
    if (!eglInitialize(app.display, &major, &minor)) return false;

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE
    };

    EGLint num_configs;
    if (!eglChooseConfig(app.display, config_attribs, &app.config, 1, &num_configs) || num_configs == 0)
        return false;

    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    app.context = eglCreateContext(app.display, app.config, EGL_NO_CONTEXT, ctx_attribs);
    if (app.context == EGL_NO_CONTEXT) return false;

    return true;
}

static bool createSurface() {
    if (!app.window) return false;
    if (app.surface != EGL_NO_SURFACE) {
        eglDestroySurface(app.display, app.surface);
    }
    app.surface = eglCreateWindowSurface(app.display, app.config, app.window, NULL);
    if (app.surface == EGL_NO_SURFACE) return false;
    return eglMakeCurrent(app.display, app.surface, app.surface, app.context);
}

static void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    iOSTheme::ApplyTheme();

    ImGui_ImplAndroid_Init(app.window);
    ImGui_ImplOpenGL3_Init("#version 130");
}

static void drawLoginScreen() {
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 window_size = ImVec2(380, 450);
    ImVec2 window_pos = ImVec2((io.DisplaySize.x - window_size.x) * 0.5f, (io.DisplaySize.y - window_size.y) * 0.5f);

    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32, 32));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));

    ImGui::Begin("##Login", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::SetCursorPosY(40);

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(window_size.x * 0.5f, ImGui::GetCursorScreenPos().y + 30);
    draw->AddCircleFilled(center, 30.0f, IM_COL32(4, 132, 255, 255));
    draw->AddRectFilled(ImVec2(center.x - 10, center.y - 8), ImVec2(center.x + 10, center.y + 5), IM_COL32(255, 255, 255, 255), 3.0f);

    ImGui::SetCursorPosY(80);
    const char* title = "Welcome Back";
    float tw = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((window_size.x - tw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.85f));
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();

    ImGui::Spacing();
    const char* sub = "Enter your password to continue";
    float sw = ImGui::CalcTextSize(sub).x;
    ImGui::SetCursorPosX((window_size.x - sw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.45f));
    ImGui::Text("%s", sub);
    ImGui::PopStyleColor();

    ImGui::SetCursorPosY(150);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 14));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.95f, 0.95f, 0.97f, 1.0f));
    ImGui::PushItemWidth(window_size.x - 64);
    ImGui::InputTextWithHint("##pw", "Enter Password", app.password, sizeof(app.password), ImGuiInputTextFlags_Password);
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    if (app.show_error) {
        ImGui::Spacing();
        const char* err = "Wrong password!";
        float ew = ImGui::CalcTextSize(err).x;
        ImGui::SetCursorPosX((window_size.x - ew) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.23f, 0.19f, 1));
        ImGui::Text("%s", err);
        ImGui::PopStyleColor();
    }

    ImGui::SetCursorPosY(260);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 16));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.04f, 0.52f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.62f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.42f, 0.9f, 1.0f));
    ImGui::PushItemWidth(window_size.x - 64);
    if (ImGui::Button("Login", ImVec2(window_size.x - 64, 50))) {
        if (strcmp(app.password, "1234") == 0) {
            app.state = STATE_MAIN_MENU;
            app.show_error = false;
            memset(app.password, 0, sizeof(app.password));
        } else {
            app.show_error = true;
            memset(app.password, 0, sizeof(app.password));
        }
    }
    ImGui::PopItemWidth();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    ImGui::SetCursorPosY(window_size.y - 80);
    const char* foot = "Powered by ImGui iOS Style";
    float fw = ImGui::CalcTextSize(foot).x;
    ImGui::SetCursorPosX((window_size.x - fw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.25f));
    ImGui::Text("%s", foot);
    ImGui::PopStyleColor();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

static void drawMainMenu() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1));

    ImGui::Begin("##Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    const char* hdr = "iOS Style App";
    float hw = ImGui::CalcTextSize(hdr).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - hw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.85f));
    ImGui::Text("%s", hdr);
    ImGui::PopStyleColor();

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));

    struct MI { const char* icon; const char* title; const char* sub; AppState st; };
    MI items[] = {
        {"[HOME]", "Home", "View your dashboard", STATE_MAIN_MENU},
        {"[USER]", "Profile", "Edit your profile", STATE_PROFILE},
        {"[GEAR]", "Settings", "App preferences", STATE_SETTINGS},
        {"[INFO]", "About", "App information", STATE_ABOUT}
    };

    for (int i = 0; i < 4; i++) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);

        char cid[32]; snprintf(cid, sizeof(cid), "mi%d", i);
        ImGui::BeginChild(cid, ImVec2(-1, 70), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.04f, 0.52f, 1, 1));
        ImGui::Text("%s", items[i].icon); ImGui::PopStyleColor();
        ImGui::SameLine(60);
        ImGui::Text("%s", items[i].title);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.45f));
        ImGui::Text("%s", items[i].sub); ImGui::PopStyleColor();

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.25f));
        ImGui::Text(">"); ImGui::PopStyleColor();
        ImGui::EndChild();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked()) app.state = items[i].st;
        ImGui::Spacing();
    }

    ImGui::SetCursorPosY(io.DisplaySize.y - 80);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0.23f, 0.19f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.17f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.17f, 0.15f, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    if (ImGui::Button("Logout", ImVec2(io.DisplaySize.x - 48, 50))) {
        app.state = STATE_LOGIN;
        memset(app.password, 0, sizeof(app.password));
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

static void drawSettingsScreen() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1));

    ImGui::Begin("##Set", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    if (ImGui::Button("< Back", ImVec2(80, 40))) app.state = STATE_MAIN_MENU;
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 40);
    const char* hdr = "Settings";
    float hw = ImGui::CalcTextSize(hdr).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - hw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.85f));
    ImGui::Text("%s", hdr); ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));

    auto toggle = [](const char* label, bool* val) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
        if (*val) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.8f, 0.4f, 1));
        else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.62f, 1));
        ImGui::PushID(label);
        if (ImGui::Button(*val ? "ON" : "OFF", ImVec2(51, 31))) *val = !*val;
        ImGui::PopID(); ImGui::PopStyleColor(); ImGui::PopStyleVar();
        ImGui::SameLine(); ImGui::Text("%s", label);
    };

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);

    ImGui::BeginChild("gs", ImVec2(-1, 180), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("General"); ImGui::Spacing();
    toggle("Dark Mode", &app.dark_mode); ImGui::Spacing();
    toggle("Notifications", &app.notifications); ImGui::Spacing();
    toggle("Location Services", &app.location_services);
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("cs", ImVec2(-1, 120), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Connectivity"); ImGui::Spacing();
    toggle("Bluetooth", &app.bluetooth); ImGui::Spacing();
    toggle("Wi-Fi", &app.wifi);
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("ss", ImVec2(-1, 140), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Sound & Haptics"); ImGui::Spacing();
    ImGui::Text("Volume");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##vol", &app.volume, 0, 1, "%.0f%%");
    ImGui::PopItemWidth(); ImGui::PopStyleVar(); ImGui::Spacing();
    ImGui::Text("Brightness");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##bri", &app.brightness, 0, 1, "%.0f%%");
    ImGui::PopItemWidth(); ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

static void drawProfileScreen() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1));
    ImGui::Begin("##Prof", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    if (ImGui::Button("< Back", ImVec2(80, 40))) app.state = STATE_MAIN_MENU;
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 30);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.85f));
    ImGui::Text("Profile"); ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("pc", ImVec2(-1, 100), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 c = ImVec2(ImGui::GetCursorScreenPos().x + 30, ImGui::GetCursorScreenPos().y + 30);
    draw->AddCircleFilled(c, 30.0f, IM_COL32(4, 132, 255, 255));
    draw->AddText(ImVec2(c.x - 8, c.y - 8), IM_COL32(255, 255, 255, 255), "D");
    ImGui::SameLine(80);
    ImGui::Text("DevZeron");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.45f));
    ImGui::Text("Developer"); ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("ep", ImVec2(-1, 250), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Edit Profile"); ImGui::Spacing();
    ImGui::Text("Username");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##un", app.text_buffer, sizeof(app.text_buffer));
    ImGui::PopItemWidth(); ImGui::PopStyleVar(); ImGui::Spacing();
    ImGui::Text("Bio");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##bio", app.text_buffer, sizeof(app.text_buffer), ImVec2(-1, 60));
    ImGui::PopItemWidth(); ImGui::PopStyleVar(); ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.04f, 0.52f, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::Button("Save Changes", ImVec2(-1, 44));
    ImGui::PopStyleVar(); ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

static void drawAboutScreen() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1));
    ImGui::Begin("##About", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    if (ImGui::Button("< Back", ImVec2(80, 40))) app.state = STATE_MAIN_MENU;
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 30);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0.85f));
    ImGui::Text("About"); ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::BeginChild("ai", ImVec2(-1, 200), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("App Information"); ImGui::Spacing();
    auto item = [](const char* l, const char* v) {
        ImGui::Text("%s", l); ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(v).x);
        ImGui::TextDisabled("%s", v);
    };
    item("App Name", "iOS Style ImGui"); ImGui::Spacing();
    item("Version", "1.0.0"); ImGui::Spacing();
    item("Developer", "DevZeron"); ImGui::Spacing();
    item("Platform", "Android");
    ImGui::EndChild();
    ImGui::PopStyleVar(); ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_imgui_iosstyle_MainActivity_nativeInit(JNIEnv* env, jobject thiz, jobject surface) {
    LOGI("nativeInit");

    memset(&app, 0, sizeof(app));
    app.state = STATE_LOGIN;
    app.volume = 0.5f;
    app.brightness = 0.7f;
    app.progress = 0.65f;
    app.window = ANativeWindow_fromSurface(env, surface);

    if (!initEGL()) {
        LOGE("EGL init failed");
        return;
    }

    app.surface = eglCreateWindowSurface(app.display, app.config, app.window, NULL);
    if (app.surface == EGL_NO_SURFACE) {
        LOGE("Surface creation failed");
        return;
    }
    eglMakeCurrent(app.display, app.surface, app.surface, app.context);

    app.initialized = true;
    LOGI("EGL ready");

    ANativeWindow_setBuffersGeometry(app.window, 0, 0, WINDOW_FORMAT_RGBA_8888);

    initImGui();
    LOGI("ImGui initialized");
}

JNIEXPORT void JNICALL
Java_com_imgui_iosstyle_MainActivity_nativeRender(JNIEnv* env, jobject thiz) {
    if (!app.initialized) return;

    eglMakeCurrent(app.display, app.surface, app.surface, app.context);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)app.width, (float)app.height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();

    switch (app.state) {
        case STATE_LOGIN: drawLoginScreen(); break;
        case STATE_MAIN_MENU: drawMainMenu(); break;
        case STATE_SETTINGS: drawSettingsScreen(); break;
        case STATE_PROFILE: drawProfileScreen(); break;
        case STATE_ABOUT: drawAboutScreen(); break;
    }

    ImGui::Render();
    glViewport(0, 0, app.width, app.height);
    glClearColor(0.95f, 0.95f, 0.97f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(app.display, app.surface);
}

JNIEXPORT void JNICALL
Java_com_imgui_iosstyle_MainActivity_nativeTouch(JNIEnv* env, jobject thiz,
    jint id, jint action, jfloat x, jfloat y) {
    ImGuiIO& io = ImGui::GetIO();
    int msg = -1;
    switch (action) {
        case 0: msg = ImGuiMouseButton_Left; break;
        case 1: msg = ImGuiMouseButton_Left; break;
        case 2: break;
    }
    if (msg >= 0) {
        io.MousePos = ImVec2(x, y);
        io.MouseDown[msg] = (action != 1);
    }
}

JNIEXPORT void JNICALL
Java_com_imgui_iosstyle_MainActivity_nativeResize(JNIEnv* env, jobject thiz, jint w, jint h) {
    LOGI("Resize: %dx%d", w, h);
    app.width = w;
    app.height = h;
    if (app.initialized) {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)w, (float)h);
    }
}

JNIEXPORT void JNICALL
Java_com_imgui_iosstyle_MainActivity_nativeDestroy(JNIEnv* env, jobject thiz) {
    LOGI("nativeDestroy");
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

} // extern "C"

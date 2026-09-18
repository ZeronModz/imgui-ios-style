#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "ios_theme.h"
#include <cstdio>
#include <cstring>

// App State
enum AppState {
    STATE_LOGIN,
    STATE_MAIN_MENU,
    STATE_SETTINGS,
    STATE_PROFILE,
    STATE_ABOUT
};

static AppState current_state = STATE_LOGIN;
static char password_buffer[32] = "";
static bool show_error = false;
static float error_timer = 0.0f;

// iOS Settings State
static bool dark_mode = false;
static bool notifications = true;
static bool location_services = false;
static bool bluetooth = true;
static bool wifi = true;
static float brightness = 0.7f;
static float volume = 0.5f;
static int selected_menu = 0;
static char text_buffer[256] = "";
static float progress = 0.65f;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.55f, 1.00f);

// iOS-style helper functions
void DrawIOSSwitch(const char* label, bool* value) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 10.0f));
    
    if (*value) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.80f, 0.40f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.85f, 0.45f, 1.00f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60f, 0.60f, 0.62f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.65f, 0.67f, 1.00f));
    }
    
    ImGui::PushID(label);
    if (ImGui::Button(*value ? "ON" : "OFF", ImVec2(51.0f, 31.0f))) {
        *value = !*value;
    }
    ImGui::PopID();
    
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    
    ImGui::SameLine();
    ImGui::Text("%s", label);
}

void DrawIOSSlider(const char* label, float* value, float min = 0.0f, float max = 1.0f) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.04f, 0.52f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.14f, 0.62f, 1.00f, 1.00f));
    
    ImGui::Text("%s", label);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderFloat("##slider", value, min, max, "%.0f%%");
    ImGui::PopItemWidth();
    
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

void DrawIOSListItem(const char* label, const char* value = nullptr) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f, 12.0f));
    
    ImGui::Text("%s", label);
    
    if (value) {
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(value).x);
        ImGui::TextDisabled("%s", value);
    }
    
    ImGui::PopStyleVar(2);
}

// Login Screen
void DrawLoginScreen() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Center the login window
    ImVec2 window_size = ImVec2(380, 450);
    ImVec2 window_pos = ImVec2(
        (io.DisplaySize.x - window_size.x) * 0.5f,
        (io.DisplaySize.y - window_size.y) * 0.5f
    );
    
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(32, 32));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    
    ImGui::Begin("##Login", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    
    // App Logo/Title
    ImGui::SetCursorPosY(40);
    
    // Draw a circle logo
    ImVec2 center = ImVec2(window_size.x * 0.5f, ImGui::GetCursorScreenPos().y + 30);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircleFilled(center, 30.0f, IM_COL32(4, 132, 255, 255));
    
    // Lock icon (simple rectangle)
    draw_list->AddRectFilled(
        ImVec2(center.x - 10, center.y - 8),
        ImVec2(center.x + 10, center.y + 5),
        IM_COL32(255, 255, 255, 255),
        3.0f
    );
    draw_list->AddRect(
        ImVec2(center.x - 7, center.y - 15),
        ImVec2(center.x + 7, center.y - 5),
        IM_COL32(255, 255, 255, 255),
        3.0f,
        0,
        2.0f
    );
    
    ImGui::SetCursorPosY(80);
    ImGui::PushFont(nullptr);
    
    // Title
    const char* title = "Welcome Back";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((window_size.x - title_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Subtitle
    const char* subtitle = "Enter your password to continue";
    float subtitle_width = ImGui::CalcTextSize(subtitle).x;
    ImGui::SetCursorPosX((window_size.x - subtitle_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
    ImGui::Text("%s", subtitle);
    ImGui::PopStyleColor();
    
    ImGui::SetCursorPosY(150);
    
    // Password Input
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f, 14.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.95f, 0.95f, 0.97f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.90f, 0.90f, 0.92f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.85f, 0.85f, 0.87f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.85f, 0.87f, 1.00f));
    
    ImGui::PushItemWidth(window_size.x - 64);
    ImGui::InputTextWithHint("##password", "Enter Password", password_buffer, sizeof(password_buffer), ImGuiInputTextFlags_Password);
    ImGui::PopItemWidth();
    
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    
    // Error message
    if (show_error) {
        ImGui::Spacing();
        const char* error_text = "Wrong password! Try again.";
        float error_width = ImGui::CalcTextSize(error_text).x;
        ImGui::SetCursorPosX((window_size.x - error_width) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.23f, 0.19f, 1.0f));
        ImGui::Text("%s", error_text);
        ImGui::PopStyleColor();
    }
    
    ImGui::SetCursorPosY(260);
    
    // Login Button
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.0f, 16.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.04f, 0.52f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.62f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.42f, 0.90f, 1.00f));
    
    ImGui::PushItemWidth(window_size.x - 64);
    if (ImGui::Button("Login", ImVec2(window_size.x - 64, 50))) {
        if (strcmp(password_buffer, "1234") == 0) {
            current_state = STATE_MAIN_MENU;
            show_error = false;
            strcpy(password_buffer, "");
        } else {
            show_error = true;
            error_timer = 3.0f;
            strcpy(password_buffer, "");
        }
    }
    ImGui::PopItemWidth();
    
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    
    ImGui::SetCursorPosY(window_size.y - 80);
    
    // Footer
    const char* footer = "Powered by ImGui iOS Style";
    float footer_width = ImGui::CalcTextSize(footer).x;
    ImGui::SetCursorPosX((window_size.x - footer_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
    ImGui::Text("%s", footer);
    ImGui::PopStyleColor();
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// Main Menu Screen
void DrawMainMenu() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Full screen window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1.00f));
    
    ImGui::Begin("##MainMenu", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    
    // Header
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    
    const char* header = "iOS Style App";
    float header_width = ImGui::CalcTextSize(header).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - header_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::Text("%s", header);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Menu Content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    
    // Menu Items
    struct MenuItem {
        const char* icon;
        const char* title;
        const char* subtitle;
        AppState state;
    };
    
    MenuItem menu_items[] = {
        {"[HOME]", "Home", "View your dashboard", STATE_MAIN_MENU},
        {"[USER]", "Profile", "Edit your profile", STATE_PROFILE},
        {"[GEAR]", "Settings", "App preferences", STATE_SETTINGS},
        {"[INFO]", "About", "App information", STATE_ABOUT}
    };
    
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);
    
    for (int i = 0; i < num_items; i++) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 12));
        
        char child_id[32];
        snprintf(child_id, sizeof(child_id), "MenuItem_%d", i);
        
        ImGui::BeginChild(child_id, ImVec2(-1, 70), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
        
        // Icon
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.04f, 0.52f, 1.00f, 1.00f));
        ImGui::Text("%s", menu_items[i].icon);
        ImGui::PopStyleColor();
        
        ImGui::SameLine(60);
        
        // Title and Subtitle
        ImGui::Text("%s", menu_items[i].title);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
        ImGui::Text("%s", menu_items[i].subtitle);
        ImGui::PopStyleColor();
        
        // Arrow
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
        ImGui::Text(">");
        ImGui::PopStyleColor();
        
        ImGui::EndChild();
        
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
        
        // Make the entire child clickable
        if (ImGui::IsItemClicked()) {
            current_state = menu_items[i].state;
        }
        
        ImGui::Spacing();
    }
    
    ImGui::PopStyleVar();
    
    // Logout Button at bottom
    ImGui::SetCursorPosY(io.DisplaySize.y - 80);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 0.23f, 0.19f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.17f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80f, 0.17f, 0.15f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 14));
    
    if (ImGui::Button("Logout", ImVec2(io.DisplaySize.x - 48, 50))) {
        current_state = STATE_LOGIN;
        strcpy(password_buffer, "");
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// Settings Screen
void DrawSettingsScreen() {
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1.00f));
    
    ImGui::Begin("##Settings", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    
    // Header with back button
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    
    if (ImGui::Button("< Back", ImVec2(80, 40))) {
        current_state = STATE_MAIN_MENU;
    }
    
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 40);
    
    const char* header = "Settings";
    float header_width = ImGui::CalcTextSize(header).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - header_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::Text("%s", header);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Settings Content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    
    // General Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 12));
    
    ImGui::BeginChild("GeneralSettings", ImVec2(-1, 180), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("General");
    ImGui::Spacing();
    DrawIOSSwitch("Dark Mode", &dark_mode);
    ImGui::Spacing();
    DrawIOSSwitch("Notifications", &notifications);
    ImGui::Spacing();
    DrawIOSSwitch("Location Services", &location_services);
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Connectivity Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 12));
    
    ImGui::BeginChild("ConnectivitySettings", ImVec2(-1, 120), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("Connectivity");
    ImGui::Spacing();
    DrawIOSSwitch("Bluetooth", &bluetooth);
    ImGui::Spacing();
    DrawIOSSwitch("Wi-Fi", &wifi);
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Sound Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 12));
    
    ImGui::BeginChild("SoundSettings", ImVec2(-1, 140), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("Sound & Haptics");
    ImGui::Spacing();
    DrawIOSSlider("Volume", &volume);
    ImGui::Spacing();
    DrawIOSSlider("Brightness", &brightness);
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// Profile Screen
void DrawProfileScreen() {
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1.00f));
    
    ImGui::Begin("##Profile", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    
    // Header with back button
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    
    if (ImGui::Button("< Back", ImVec2(80, 40))) {
        current_state = STATE_MAIN_MENU;
    }
    
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 40);
    
    const char* header = "Profile";
    float header_width = ImGui::CalcTextSize(header).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - header_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::Text("%s", header);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Profile Content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    
    // Profile Card
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 16));
    
    ImGui::BeginChild("ProfileCard", ImVec2(-1, 100), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    // Avatar circle
    ImVec2 center = ImVec2(ImGui::GetCursorScreenPos().x + 30, ImGui::GetCursorScreenPos().y + 30);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircleFilled(center, 30.0f, IM_COL32(4, 132, 255, 255));
    
    // User initials
    draw_list->AddText(
        ImVec2(center.x - 8, center.y - 8),
        IM_COL32(255, 255, 255, 255),
        "D"
    );
    
    ImGui::SameLine(80);
    
    ImGui::Text("DevZeron");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
    ImGui::Text("Developer");
    ImGui::PopStyleColor();
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Edit Profile Form
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 16));
    
    ImGui::BeginChild("EditProfile", ImVec2(-1, 250), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("Edit Profile");
    ImGui::Spacing();
    
    ImGui::Text("Username");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##username", text_buffer, sizeof(text_buffer));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    
    ImGui::Text("Bio");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
    ImGui::PushItemWidth(-1);
    ImGui::InputTextMultiline("##bio", text_buffer, sizeof(text_buffer), ImVec2(-1, 60));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.04f, 0.52f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.62f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.42f, 0.90f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
    
    if (ImGui::Button("Save Changes", ImVec2(-1, 44))) {
        // Save changes
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// About Screen
void DrawAboutScreen() {
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.95f, 0.95f, 0.97f, 1.00f));
    
    ImGui::Begin("##About", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    
    // Header with back button
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));
    
    if (ImGui::Button("< Back", ImVec2(80, 40))) {
        current_state = STATE_MAIN_MENU;
    }
    
    ImGui::SameLine(io.DisplaySize.x * 0.5f - 30);
    
    const char* header = "About";
    float header_width = ImGui::CalcTextSize(header).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - header_width) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));
    ImGui::Text("%s", header);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // About Content
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 16));
    
    // App Info Card
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 16));
    
    ImGui::BeginChild("AppInfo", ImVec2(-1, 200), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("App Information");
    ImGui::Spacing();
    DrawIOSListItem("App Name", "iOS Style ImGui");
    ImGui::Spacing();
    DrawIOSListItem("Version", "1.0.0");
    ImGui::Spacing();
    DrawIOSListItem("Build", "2024.1");
    ImGui::Spacing();
    DrawIOSListItem("Developer", "DevZeron");
    ImGui::Spacing();
    DrawIOSListItem("Platform", "Android/Termux");
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Credits Card
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildPadding, ImVec2(16, 16));
    
    ImGui::BeginChild("Credits", ImVec2(-1, 140), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Text("Credits");
    ImGui::Spacing();
    DrawIOSListItem("ImGui", "Omar Cornut");
    ImGui::Spacing();
    DrawIOSListItem("Design", "Apple iOS HIG");
    ImGui::Spacing();
    DrawIOSListItem("License", "MIT");
    
    ImGui::EndChild();
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

int main(int, char*[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow("iOS Style ImGui", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        420, 750, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    if (window == NULL) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Create OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    
    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Apply iOS Theme
    iOSTheme::ApplyTheme();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    // Main loop
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Draw based on current state
        switch (current_state) {
            case STATE_LOGIN:
                DrawLoginScreen();
                break;
            case STATE_MAIN_MENU:
                DrawMainMenu();
                break;
            case STATE_SETTINGS:
                DrawSettingsScreen();
                break;
            case STATE_PROFILE:
                DrawProfileScreen();
                break;
            case STATE_ABOUT:
                DrawAboutScreen();
                break;
        }
        
        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

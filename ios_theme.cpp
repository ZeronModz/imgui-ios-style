#include "ios_theme.h"

namespace iOSTheme {
    // Color definitions
    namespace Colors {
        ImVec4 Background = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
        ImVec4 SecondaryBackground = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        ImVec4 TertiaryBackground = ImVec4(0.97f, 0.97f, 0.98f, 1.00f);
        ImVec4 Label = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
        ImVec4 SecondaryLabel = ImVec4(0.00f, 0.00f, 0.00f, 0.55f);
        ImVec4 SystemBlue = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        ImVec4 SystemGreen = ImVec4(0.20f, 0.78f, 0.35f, 1.00f);
        ImVec4 SystemRed = ImVec4(1.00f, 0.23f, 0.19f, 1.00f);
        ImVec4 SystemOrange = ImVec4(1.00f, 0.58f, 0.00f, 1.00f);
        ImVec4 SystemGray = ImVec4(0.55f, 0.55f, 0.57f, 1.00f);
        ImVec4 Separator = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
    }
    
    namespace Style {
        float CornerRadius = 12.0f;
        float ButtonHeight = 44.0f;
        float Spacing = 16.0f;
    }
    
    void ApplyLightTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Window
        style.WindowRounding = Style::CornerRadius;
        style.WindowBorderSize = 0.0f;
        style.WindowPadding = ImVec2(Style::Spacing, Style::Spacing);
        
        // Frame
        style.FrameRounding = Style::CornerRadius;
        style.FrameBorderSize = 0.0f;
        style.FramePadding = ImVec2(12.0f, 12.0f);
        
        // Spacing
        style.ItemSpacing = ImVec2(Style::Spacing, Style::Spacing);
        style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
        
        // Scrollbar
        style.ScrollbarSize = 8.0f;
        style.ScrollbarRounding = 4.0f;
        
        // Grab
        style.GrabMinSize = 20.0f;
        style.GrabRounding = Style::CornerRadius;
        
        // Tabs
        style.TabRounding = Style::CornerRadius;
        
        // Colors
        ImVec4* colors = style.Colors;
        
        colors[ImGuiCol_WindowBg] = Colors::Background;
        colors[ImGuiCol_ChildBg] = Colors::SecondaryBackground;
        colors[ImGuiCol_PopupBg] = Colors::SecondaryBackground;
        
        colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.85f, 0.87f, 0.50f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.85f, 0.87f, 0.70f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.87f, 0.90f);
        
        colors[ImGuiCol_Button] = Colors::SystemBlue;
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.62f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.04f, 0.42f, 0.90f, 1.00f);
        
        colors[ImGuiCol_Header] = ImVec4(0.85f, 0.85f, 0.87f, 0.50f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.85f, 0.87f, 0.70f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.85f, 0.87f, 0.90f);
        
        colors[ImGuiCol_CheckMark] = Colors::SystemBlue;
        colors[ImGuiCol_SliderGrab] = Colors::SystemBlue;
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.14f, 0.62f, 1.00f, 1.00f);
        
        colors[ImGuiCol_Tab] = Colors::SecondaryBackground;
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.62f, 1.00f, 0.80f);
        colors[ImGuiCol_TabActive] = Colors::SystemBlue;
        
        colors[ImGuiCol_Text] = Colors::Label;
        colors[ImGuiCol_TextDisabled] = Colors::SecondaryLabel;
        
        colors[ImGuiCol_Separator] = Colors::Separator;
        
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.69f, 0.69f, 0.71f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.57f, 1.00f);
    }
    
    void ApplyDarkTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.WindowRounding = Style::CornerRadius;
        style.WindowBorderSize = 0.0f;
        style.WindowPadding = ImVec2(Style::Spacing, Style::Spacing);
        
        style.FrameRounding = Style::CornerRadius;
        style.FrameBorderSize = 0.0f;
        style.FramePadding = ImVec2(12.0f, 12.0f);
        
        style.ItemSpacing = ImVec2(Style::Spacing, Style::Spacing);
        style.ItemInnerSpacing = ImVec2(8.0f, 8.0f);
        
        style.ScrollbarSize = 8.0f;
        style.ScrollbarRounding = 4.0f;
        
        style.GrabMinSize = 20.0f;
        style.GrabRounding = Style::CornerRadius;
        
        style.TabRounding = Style::CornerRadius;
        
        ImVec4* colors = style.Colors;
        
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
        
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.36f, 0.37f, 1.00f);
        
        colors[ImGuiCol_Button] = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.62f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.04f, 0.42f, 0.90f, 1.00f);
        
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.36f, 0.37f, 1.00f);
        
        colors[ImGuiCol_CheckMark] = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.14f, 0.62f, 1.00f, 1.00f);
        
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.62f, 1.00f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
        colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
        
        colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.16f, 0.50f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.36f, 0.37f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.46f, 0.47f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.56f, 0.57f, 1.00f);
    }
    
    void ApplyTheme() {
        ApplyLightTheme();
    }
}

#pragma once

#include "imgui.h"

namespace iOSTheme {
    // Apply iOS-style theme to ImGui
    void ApplyTheme();
    
    // Apply iOS Light Theme
    void ApplyLightTheme();
    
    // Apply iOS Dark Theme
    void ApplyDarkTheme();
    
    // iOS Color Constants
    namespace Colors {
        // Light Mode Colors
        extern ImVec4 Background;
        extern ImVec4 SecondaryBackground;
        extern ImVec4 TertiaryBackground;
        extern ImVec4 Label;
        extern ImVec4 SecondaryLabel;
        extern ImVec4 SystemBlue;
        extern ImVec4 SystemGreen;
        extern ImVec4 SystemRed;
        extern ImVec4 SystemOrange;
        extern ImVec4 SystemGray;
        extern ImVec4 Separator;
    }
    
    // Style Constants
    namespace Style {
        extern float CornerRadius;
        extern float ButtonHeight;
        extern float Spacing;
    }
}

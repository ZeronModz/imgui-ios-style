# ImGui iOS Style Theme

A beautiful iOS-style theme for Dear ImGui, designed to mimic the look and feel of iOS applications.

## Features

- **iOS-style Rounded Corners**: All UI elements have the signature iOS rounded corners
- **System Colors**: Authentic iOS color palette including System Blue, Green, Red, etc.
- **Touch-friendly Controls**: Larger buttons and controls optimized for touch input
- **Modern Design**: Clean, minimalist design following iOS Human Interface Guidelines
- **Light and Dark Themes**: Support for both light and dark mode

## Screenshots

The project includes:
- iOS-style buttons with rounded corners
- Toggle switches (iOS-style)
- Sliders with iOS styling
- Tab bars
- Settings panels
- List items with separators

## Requirements

### For Desktop (Linux/macOS)
- SDL2 library
- OpenGL 3.0+ or OpenGL ES 3.0+
- C++17 compiler
- Dear ImGui (downloaded automatically by setup script)

### For Android
- Android SDK
- Android NDK
- CMake
- JDK (for Java compilation)

## Installation

### Android/Termux

```bash
# Install dependencies
pkg install sdl2

# Clone or download this project
cd /storage/emulated/0/Download/imgui

# Run setup script to download ImGui
chmod +x setup.sh
./setup.sh

# Build the project
make

# Run the demo
./ios_style_demo
```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt install libsdl2-dev

# Clone or download this project
cd ~/imgui-ios-style

# Run setup script
chmod +x setup.sh
./setup.sh

# Build
make

# Run
./ios_style_demo
```

### macOS

```bash
# Install dependencies
brew install sdl2

# Clone or download this project
cd ~/imgui-ios-style

# Run setup script
chmod +x setup.sh
./setup.sh

# Build
make

# Run
./ios_style_demo
```

### Android APK Build

To build an Android APK:

```bash
cd /storage/emulated/0/Download/imgui

# Make build script executable
chmod +x build_simple.sh

# Build APK
./build_simple.sh
```

The APK will be created at: `ios-style.apk`

To install on your Android device:
```bash
adb install ios-style.apk
```

Or transfer the APK file to your phone and install it manually.

## Project Structure

```
imgui-ios-style/
├── imgui/              # Dear ImGui source (downloaded by setup)
├── backends/           # SDL2 and OpenGL backends
├── android/            # Android app structure
│   └── app/src/main/
│       ├── AndroidManifest.xml
│       ├── java/       # Java source code
│       ├── jni/        # Native C++ code
│       └── res/        # Android resources
├── ios_theme.h         # iOS theme header
├── ios_theme.cpp       # iOS theme implementation
├── main.cpp            # Demo application (desktop)
├── Makefile            # Build system (desktop)
├── setup.sh            # Setup script
├── build_simple.sh     # Android build script
└── README.md           # This file
```

## Usage

### Basic Usage

```cpp
#include "ios_theme.h"

// Initialize ImGui...

// Apply iOS theme
iOSTheme::ApplyTheme();

// Or for dark mode
iOSTheme::ApplyDarkTheme();

// Your ImGui code here...
```

### Custom Colors

You can customize the iOS colors by modifying the values in `ios_theme.cpp`:

```cpp
namespace iOSTheme {
    namespace Colors {
        ImVec4 SystemBlue = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
        // ... other colors
    }
}
```

### Custom Style

Adjust the style parameters in `ios_theme.cpp`:

```cpp
namespace iOSTheme {
    namespace Style {
        float CornerRadius = 12.0f;  // Default iOS corner radius
        float ButtonHeight = 44.0f;  // iOS recommended button height
        float Spacing = 16.0f;       // Standard iOS spacing
    }
}
```

## iOS Design Guidelines

This theme follows Apple's Human Interface Guidelines:

- **Corner Radius**: 12pt for most elements, 16pt for cards
- **Colors**: Uses iOS system colors (Blue, Green, Red, Orange, etc.)
- **Typography**: SF Pro style (you can load SF Pro font for authentic look)
- **Spacing**: 16pt standard spacing between elements
- **Button Height**: 44pt minimum for touch targets

## Integration with Existing Projects

1. Copy `ios_theme.h` and `ios_theme.cpp` to your project
2. Include the header and call `iOSTheme::ApplyTheme()` after ImGui initialization
3. Customize colors and style as needed

## Customization

### Adding New iOS-style Widgets

You can extend the theme by adding custom widgets in a new file:

```cpp
#pragma once

#include "imgui.h"

namespace iOSWidgets {
    // Custom iOS-style widget
    bool MyCustomWidget(const char* label, float* value) {
        // Your custom widget implementation
        return false;
    }
}
```

### Loading iOS Fonts

For an authentic iOS look, load the SF Pro font:

```cpp
// After ImGui context creation
ImFontConfig config;
config.FontDataOwnedByAtlas = false;

// Load SF Pro font (you need to provide the font file)
io.Fonts->AddFontFromFileTTF("SF-Pro-Text-Regular.otf", 16.0f, &config);
```

## Troubleshooting

### Build Errors

If you get build errors:

1. Make sure SDL2 is installed: `pkg-config --cflags --libs sdl2`
2. Ensure ImGui is downloaded: `ls imgui/imgui.h`
3. Check OpenGL availability: `glxinfo | grep "OpenGL version"`

### Display Issues

If the UI looks wrong:

1. Check your display scaling settings
2. Ensure OpenGL context is created properly
3. Try different ImGui font sizes

## License

This project is released under the MIT License. See the ImGui license for the underlying library.

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) by Omar Cornut
- iOS design inspiration from Apple's Human Interface Guidelines
- SDL2 for cross-platform window management and input

## Contributing

Contributions are welcome! Feel free to:

1. Report bugs
2. Suggest new features
3. Submit pull requests
4. Improve documentation

## Author

Created by DevZeron (Hasan)

## Support

If you find this project useful, consider giving it a star on GitHub!

---

**Note**: This is an unofficial iOS-style theme and is not affiliated with Apple Inc. iOS is a trademark of Apple Inc.

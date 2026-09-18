# ImGui iOS Style Android App - Complete!

## ✅ Features Implemented

### 1. Login Page
- Password input field (password: `1234`)
- Beautiful iOS-style login UI with rounded corners
- Error message for wrong password
- Lock icon and welcome text

### 2. Main Menu (After Login)
- Home
- Profile
- Settings
- About
- Logout button

### 3. iOS-Style UI Elements
- Rounded corners (12px)
- iOS system colors (Blue, Green, Red, Orange, etc.)
- Toggle switches
- Sliders
- Cards with shadows
- Touch-friendly buttons

## 📁 Project Structure

```
/storage/emulated/0/Download/imgui/
├── imgui/                    # Dear ImGui source (v1.93.0)
├── android/                  # Android app
│   └── app/src/main/
│       ├── AndroidManifest.xml
│       ├── java/             # Java code
│       ├── jni/              # C++ native code
│       └── res/              # Resources
├── ios_theme.h               # iOS theme header
├── ios_theme.cpp             # iOS theme implementation
├── main.cpp                  # Main app (with login + menu)
├── Makefile                  # Desktop build
├── setup.sh                  # Download ImGui
├── build_simple.sh           # Build Android APK
├── run.sh                    # Run desktop version
└── README.md                 # Documentation
```

## 🛠️ How to Build

### Desktop Version (Linux/macOS)
```bash
cd /storage/emulated/0/Download/imgui
make
./run.sh
```

### Android APK
```bash
cd /storage/emulated/0/Download/imgui
chmod +x build_simple.sh
./build_simple.sh
```

## 🔑 Login Credentials

- **Password**: `1234`

## 📱 Android App Features

- Full-screen immersive mode
- Portrait orientation
- OpenGL ES 2.0 rendering
- Touch input support
- Native C++ performance

## 🎨 iOS Design Elements

- **Colors**: System Blue (#0484FF), Green (#34C759), Red (#FF3B30)
- **Corner Radius**: 12px for cards, 8px for buttons
- **Spacing**: 16px standard
- **Font**: System default (can be customized)

## 📦 Installation

### On Android Device
1. Build APK: `./build_simple.sh`
2. Transfer `ios-style.apk` to your phone
3. Enable "Unknown Sources" in settings
4. Install the APK

### On Desktop
1. Build: `make`
2. Run: `./run.sh`

## 🎯 App Flow

1. **Login Screen** → Enter password `1234`
2. **Main Menu** → Choose Home/Profile/Settings/About
3. **Settings** → Toggle switches, sliders
4. **Profile** → Edit user info
5. **About** → App information
6. **Logout** → Back to login screen

## 🔧 Customization

### Change Password
Edit `main.cpp` line:
```cpp
if (strcmp(password_buffer, "1234") == 0) {
```

### Change Colors
Edit `ios_theme.cpp`:
```cpp
namespace iOSTheme {
    namespace Colors {
        ImVec4 SystemBlue = ImVec4(0.04f, 0.52f, 1.00f, 1.00f);
    }
}
```

## 📝 Author

Created by **DevZeron (Hasan)**

## 📄 License

MIT License

---

**Note**: This is an unofficial iOS-style theme and is not affiliated with Apple Inc.

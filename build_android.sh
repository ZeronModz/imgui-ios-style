#!/bin/bash
# Android Build Script for ImGui iOS Style App

set -e

# Paths
ANDROID_SDK="/data/data/com.termux/files/usr/opt/android-sdk"
ANDROID_NDK="$ANDROID_SDK/ndk/29.0.14206865"
CMAKE="$ANDROID_SDK/cmake/3.22.1/bin/cmake"
BUILD_TOOLS="$ANDROID_SDK/build-tools/36.0.0"
PROJECT_DIR="/storage/emulated/0/Download/imgui"
ANDROID_DIR="$PROJECT_DIR/android"
BUILD_DIR="$ANDROID_DIR/build"

echo "=========================================="
echo "  Building Android APK"
echo "=========================================="

# Create build directory
mkdir -p "$BUILD_DIR"

# Step 1: Build native code with CMake
echo ""
echo "Step 1: Building native code..."
cd "$BUILD_DIR"

"$CMAKE" \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DANDROID_STL=c++_shared \
    -DCMAKE_BUILD_TYPE=Release \
    "$ANDROID_DIR/app/src/main/jni"

make -j$(nproc)

# Step 2: Copy native library
echo ""
echo "Step 2: Copying native library..."
mkdir -p "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a"
cp "$BUILD_DIR/libimgui_ios.so" "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a/"

# Step 3: Build APK using aapt
echo ""
echo "Step 3: Building APK..."

# Create R.java (required for Android resources)
"$BUILD_TOOLS/aapt" package -f -m \
    -S "$ANDROID_DIR/app/src/main/res" \
    -J "$ANDROID_DIR/gen" \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -I "$ANDROID_SDK/platforms/android-36/android.jar"

# Compile Java files
javac -source 1.8 -target 1.8 \
    -bootclasspath "$ANDROID_SDK/platforms/android-36/android.jar" \
    -classpath "$ANDROID_SDK/platforms/android-36/android.jar" \
    -d "$ANDROID_DIR/classes" \
    "$ANDROID_DIR/gen/com/imgui/iosstyle/R.java" \
    "$ANDROID_DIR/app/src/main/java/com/imgui/iosstyle/MainActivity.java"

# Convert to DEX
"$BUILD_TOOLS/d8" \
    --output "$ANDROID_DIR" \
    --lib "$ANDROID_SDK/platforms/android-36/android.jar" \
    "$ANDROID_DIR/classes/com/imgui/iosstyle/MainActivity.class"

# Package APK
"$BUILD_TOOLS/aapt" package -f \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -S "$ANDROID_DIR/app/src/main/res" \
    -A "$ANDROID_DIR/app/src/main/assets" \
    -I "$ANDROID_SDK/platforms/android-36/android.jar" \
    -F "$BUILD_DIR/ios-style-unsigned.apk"

# Add DEX to APK
cd "$ANDROID_DIR"
zip -j "$BUILD_DIR/ios-style-unsigned.apk" classes.dex

# Add native libraries
cd "$ANDROID_DIR/app/src/main"
zip -r "$BUILD_DIR/ios-style-unsigned.apk" jniLibs/

# Step 4: Sign APK
echo ""
echo "Step 4: Signing APK..."

# Generate keystore if not exists
if [ ! -f "$ANDROID_DIR/debug.keystore" ]; then
    keytool -genkeypair \
        -keystore "$ANDROID_DIR/debug.keystore" \
        -storepass android \
        -alias androiddebugkey \
        -keypass android \
        -keyalg RSA \
        -keysize 2048 \
        -validity 10000 \
        -dname "CN=Debug, OU=Debug, O=Debug, L=Debug, ST=Debug, C=US"
fi

# Sign the APK
"$BUILD_TOOLS/apksigner" sign \
    --ks "$ANDROID_DIR/debug.keystore" \
    --ks-pass pass:android \
    --key-pass pass:android \
    --ks-key-alias androiddebugkey \
    --out "$BUILD_DIR/ios-style.apk" \
    "$BUILD_DIR/ios-style-unsigned.apk"

# Step 5: Verify APK
echo ""
echo "Step 5: Verifying APK..."
"$BUILD_TOOLS/apksigner" verify "$BUILD_DIR/ios-style.apk"

# Copy APK to project root
cp "$BUILD_DIR/ios-style.apk" "$PROJECT_DIR/"

echo ""
echo "=========================================="
echo "  Build Complete!"
echo "=========================================="
echo ""
echo "APK location: $PROJECT_DIR/ios-style.apk"
echo ""
echo "To install on device:"
echo "  adb install $PROJECT_DIR/ios-style.apk"
echo ""
echo "Or transfer to phone and install manually"

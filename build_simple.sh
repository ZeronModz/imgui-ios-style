#!/bin/bash
# Simple Android Build Script for ImGui iOS Style App
# Uses Termux's native tools

set -e

PROJECT_DIR="/storage/emulated/0/Download/imgui"
ANDROID_DIR="$PROJECT_DIR/android"

echo "=========================================="
echo "  Building Android APK (Simple Mode)"
echo "=========================================="

# Check if required tools are installed
echo "Checking dependencies..."

if ! command -v aapt &> /dev/null; then
    echo "Installing aapt..."
    pkg install aapt -y
fi

if ! command -v d8 &> /dev/null; then
    echo "Installing d8..."
    pkg install d8 -y
fi

if ! command -v apksigner &> /dev/null; then
    echo "Installing apksigner..."
    pkg install apksigner -y
fi

if ! command -v keytool &> /dev/null; then
    echo "Installing keytool..."
    pkg install keytool -y
fi

# Create build directory
mkdir -p "$ANDROID_DIR/build"
mkdir -p "$ANDROID_DIR/gen"
mkdir -p "$ANDROID_DIR/classes"
mkdir -p "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a"

echo ""
echo "Step 1: Building native library..."

# Build native library using NDK
cd "$PROJECT_DIR"

# Find NDK
NDK_PATH="/data/data/com.termux/files/usr/opt/android-sdk/ndk/29.0.14206865"
TOOLCHAIN="$NDK_PATH/build/cmake/android.toolchain.cmake"

if [ ! -f "$TOOLCHAIN" ]; then
    echo "NDK not found at expected path"
    echo "Please install Android NDK: pkg install android-ndk"
    exit 1
fi

# Create temporary build directory for native
mkdir -p "$ANDROID_DIR/native_build"
cd "$ANDROID_DIR/native_build"

# Run cmake
cmake \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DANDROID_STL=c++_shared \
    -DCMAKE_BUILD_TYPE=Release \
    "$ANDROID_DIR/app/src/main/jni"

make -j$(nproc)

# Copy native library
cp libimgui_ios.so "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a/"

echo ""
echo "Step 2: Compiling Java code..."

# Find android.jar
ANDROID_JAR="/data/data/com.termux/files/usr/opt/android-sdk/platforms/android-36/android.jar"
if [ ! -f "$ANDROID_JAR" ]; then
    echo "Android platform not found"
    echo "Please install: pkg install android-platform"
    exit 1
fi

# Generate R.java
aapt package -f -m \
    -S "$ANDROID_DIR/app/src/main/res" \
    -J "$ANDROID_DIR/gen" \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -I "$ANDROID_JAR"

# Compile Java
javac -source 1.8 -target 1.8 \
    -bootclasspath "$ANDROID_JAR" \
    -classpath "$ANDROID_JAR" \
    -d "$ANDROID_DIR/classes" \
    "$ANDROID_DIR/gen/com/imgui/iosstyle/R.java" \
    "$ANDROID_DIR/app/src/main/java/com/imgui/iosstyle/MainActivity.java"

# Convert to DEX
d8 --output "$ANDROID_DIR" \
    --lib "$ANDROID_JAR" \
    "$ANDROID_DIR/classes/com/imgui/iosstyle/MainActivity.class"

echo ""
echo "Step 3: Packaging APK..."

# Create APK with resources
aapt package -f \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -S "$ANDROID_DIR/app/src/main/res" \
    -A "$ANDROID_DIR/app/src/main/assets" \
    -I "$ANDROID_JAR" \
    -F "$ANDROID_DIR/build/ios-style-unsigned.apk"

# Add DEX to APK
cd "$ANDROID_DIR"
zip -j "$ANDROID_DIR/build/ios-style-unsigned.apk" classes.dex

# Add native libraries
cd "$ANDROID_DIR/app/src/main"
zip -r "$ANDROID_DIR/build/ios-style-unsigned.apk" jniLibs/

echo ""
echo "Step 4: Signing APK..."

# Generate debug keystore if not exists
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
apksigner sign \
    --ks "$ANDROID_DIR/debug.keystore" \
    --ks-pass pass:android \
    --key-pass pass:android \
    --ks-key-alias androiddebugkey \
    --out "$PROJECT_DIR/ios-style.apk" \
    "$ANDROID_DIR/build/ios-style-unsigned.apk"

echo ""
echo "=========================================="
echo "  Build Complete!"
echo "=========================================="
echo ""
echo "APK: $PROJECT_DIR/ios-style.apk"
echo ""
echo "Install command:"
echo "  adb install $PROJECT_DIR/ios-style.apk"

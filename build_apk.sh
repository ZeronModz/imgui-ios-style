#!/bin/bash
# Android APK Build - Fixed for Termux FUSE issue
set -e

PROJECT_DIR="/storage/emulated/0/Download/imgui"
NDK_PATH="/data/data/com.termux/files/usr/opt/android-sdk/ndk/29.0.14206865"
ANDROID_JAR="/data/data/com.termux/files/usr/opt/android-sdk/platforms/android-36/android.jar"
BUILD_DIR="$HOME/imgui_build"
ANDROID_DIR="$PROJECT_DIR/android"

echo "=== Building Android APK ==="

# Step 1: Build native .so in HOME (FUSE blocks on /storage)
echo ""
echo "[1/5] Compiling native .so..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

cmake \
    -DCMAKE_TOOLCHAIN_FILE="$NDK_PATH/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DANDROID_STL=c++_shared \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="$BUILD_DIR" \
    "$ANDROID_DIR/app/src/main/jni" 2>&1

make -j$(nproc) 2>&1

if [ ! -f "$BUILD_DIR/libimgui_ios.so" ]; then
    echo "ERROR: .so build failed!"
    exit 1
fi
echo "OK: libimgui_ios.so built"

# Step 2: Copy .so to jniLibs
echo ""
echo "[2/5] Copying native library..."
mkdir -p "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a"
cp "$BUILD_DIR/libimgui_ios.so" "$ANDROID_DIR/app/src/main/jniLibs/arm64-v8a/"

# Step 3: Compile Java + DEX in HOME
echo ""
echo "[3/5] Compiling Java..."
mkdir -p "$BUILD_DIR/gen" "$BUILD_DIR/classes"

aapt package -f -m \
    -S "$ANDROID_DIR/app/src/main/res" \
    -J "$BUILD_DIR/gen" \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -I "$ANDROID_JAR"

javac -source 1.8 -target 1.8 \
    -bootclasspath "$ANDROID_JAR" \
    -classpath "$ANDROID_JAR" \
    -d "$BUILD_DIR/classes" \
    "$BUILD_DIR/gen/com/imgui/iosstyle/R.java" \
    "$ANDROID_DIR/app/src/main/java/com/imgui/iosstyle/MainActivity.java"

d8 --output "$BUILD_DIR" \
    --lib "$ANDROID_JAR" \
    "$BUILD_DIR/classes/com/imgui/iosstyle/MainActivity.class"

# Step 4: Package APK in HOME
echo ""
echo "[4/5] Packaging APK..."
aapt package -f \
    -M "$ANDROID_DIR/app/src/main/AndroidManifest.xml" \
    -S "$ANDROID_DIR/app/src/main/res" \
    -I "$ANDROID_JAR" \
    -F "$BUILD_DIR/ios-style-unsigned.apk"

cd "$BUILD_DIR"
zip -j "$BUILD_DIR/ios-style-unsigned.apk" classes.dex

cd "$ANDROID_DIR/app/src/main"
zip -r "$BUILD_DIR/ios-style-unsigned.apk" jniLibs/

# Step 5: Sign
echo ""
echo "[5/5] Signing APK..."
if [ ! -f "$BUILD_DIR/debug.keystore" ]; then
    keytool -genkeypair \
        -keystore "$BUILD_DIR/debug.keystore" \
        -storepass android \
        -alias androiddebugkey \
        -keypass android \
        -keyalg RSA \
        -keysize 2048 \
        -validity 10000 \
        -dname "CN=Debug, OU=Debug, O=Debug, L=Debug, ST=Debug, C=US"
fi

apksigner sign \
    --ks "$BUILD_DIR/debug.keystore" \
    --ks-pass pass:android \
    --key-pass pass:android \
    --ks-key-alias androiddebugkey \
    --out "$PROJECT_DIR/ios-style.apk" \
    "$BUILD_DIR/ios-style-unsigned.apk"

apksigner verify "$PROJECT_DIR/ios-style.apk"

echo ""
echo "=========================================="
echo "  BUILD COMPLETE!"
echo "=========================================="
echo "APK: $PROJECT_DIR/ios-style.apk"
echo ""
echo "Install: adb install $PROJECT_DIR/ios-style.apk"
ls -lh "$PROJECT_DIR/ios-style.apk"

#!/bin/bash
# Direct NDK build - use system NDK directly, build .so in HOME
set -e

PROJECT_DIR="/storage/emulated/0/Download/imgui"
SDK="/data/data/com.termux/files/usr/opt/android-sdk"
NDK="$SDK/ndk/29.0.14206865"
SYSROOT="$NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot"
BUILD_DIR="$HOME/imgui_build"
IMGUI="$PROJECT_DIR/imgui"

echo "=== Building Android .so ==="
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Detect host arch
HOST_ARCH=$(uname -m)
if [ "$HOST_ARCH" = "aarch64" ]; then
    PREBUILT="linux-arm64"
else
    PREBUILT="linux-x86_64"
fi
NDK_BIN="$NDK/toolchains/llvm/prebuilt/$PREBUILT/bin"
SYSROOT="$NDK/toolchains/llvm/prebuilt/$PREBUILT/sysroot"

echo "NDK: $NDK"
echo "Host: $HOST_ARCH / $PREBUILT"
echo "clang++: $(ls $NDK_BIN/clang++ 2>/dev/null || echo NOT FOUND)"

# Verify clang++ exists and works
if [ ! -f "$NDK_BIN/clang++" ]; then
    echo "ERROR: clang++ not found at $NDK_BIN/clang++"
    ls "$NDK_BIN/" | head -10
    exit 1
fi

CC="$NDK_BIN/clang++"
TARGET="--target=aarch64-linux-android24"
CFLAGS="$TARGET -DANDROID -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -O2 -fPIC -std=c++17"
INCLUDES="-I$IMGUI -I$IMGUI/backends -I$PROJECT_DIR"

echo ""
echo "Compiling..."
SOURCES=(
    "$IMGUI/imgui.cpp"
    "$IMGUI/imgui_draw.cpp"
    "$IMGUI/imgui_tables.cpp"
    "$IMGUI/imgui_widgets.cpp"
    "$IMGUI/imgui_demo.cpp"
    "$IMGUI/backends/imgui_impl_android.cpp"
    "$IMGUI/backends/imgui_impl_opengl3.cpp"
    "$PROJECT_DIR/android/app/src/main/jni/native_code.cpp"
    "$PROJECT_DIR/ios_theme.cpp"
)

for src in "${SOURCES[@]}"; do
    out="$BUILD_DIR/$(basename $src .cpp).o"
    echo "  CC: $(basename $src)"
    if ! "$CC" $CFLAGS $INCLUDES -c "$src" -o "$out" 2>&1; then
        echo "FAILED: $(basename $src)"
        exit 1
    fi
done

echo ""
echo "Linking..."
LDFLAGS="$TARGET --sysroot=$SYSROOT -shared -Wl,--gc-sections"
LIBS="-L$SYSROOT/usr/lib/aarch64-linux-android/24 -llog -landroid -lGLESv2 -lEGL -latomic -lm"

"$CC" $LDFLAGS -o "$BUILD_DIR/libimgui_ios.so" "$BUILD_DIR"/*.o $LIBS 2>&1

if [ -f "$BUILD_DIR/libimgui_ios.so" ]; then
    echo ""
    echo "SUCCESS!"
    ls -lh "$BUILD_DIR/libimgui_ios.so"
else
    echo "FAILED: .so not created"
    exit 1
fi

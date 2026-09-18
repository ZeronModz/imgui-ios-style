#!/bin/bash
# Rebuild the iOS Style ImGui Demo

cd /storage/emulated/0/Download/imgui

echo "Cleaning previous build..."
make clean

echo "Building project..."
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run the demo with: ./run.sh"
else
    echo "Build failed!"
    exit 1
fi

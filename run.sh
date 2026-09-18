#!/bin/bash
# Run the iOS Style ImGui Demo

cd /storage/emulated/0/Download/imgui

if [ ! -f ios_style_demo ]; then
    echo "Building project..."
    make
fi

echo "Starting iOS Style ImGui Demo..."
./ios_style_demo

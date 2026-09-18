#!/bin/bash
# ImGui iOS Style Project - Setup Script
# Downloads ImGui and sets up the project

set -e

PROJECT_DIR="/storage/emulated/0/Download/imgui"
IMGUI_DIR="$PROJECT_DIR/imgui"

echo "=========================================="
echo "  ImGui iOS Style Project Setup"
echo "=========================================="

# Create project directory
mkdir -p "$PROJECT_DIR"

# Check if ImGui already exists
if [ -f "$IMGUI_DIR/imgui.h" ]; then
    echo "ImGui already exists. Updating..."
    cd "$IMGUI_DIR"
    git pull
else
    echo "Downloading ImGui..."
    cd "$PROJECT_DIR"
    git clone --depth 1 https://github.com/ocornut/imgui.git
fi

echo ""
echo "Setup complete!"
echo ""
echo "Project structure:"
ls -la "$PROJECT_DIR"
echo ""
echo "ImGui version:"
grep -o "IMGUI_VERSION.*\"" "$IMGUI_DIR/imgui.h" | head -1
echo ""
echo "Next steps:"
echo "1. Build the project: cd $PROJECT_DIR && make"
echo "2. Run the demo: ./ios_style_demo"

cmake_minimum_required(VERSION 3.10)
project(imgui_ios)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(GLESv2-lib GLESv2)
find_library(EGL-lib EGL)

# Absolute paths to source directories
set(ROOT_DIR "/storage/emulated/0/Download/imgui")
set(IMGUI_DIR "${ROOT_DIR}/imgui")

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)

# Backend source files
set(BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_android.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# App source files
set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/native_code.cpp
    ${ROOT_DIR}/ios_theme.cpp
)

# Include directories
include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${ROOT_DIR}
)

# Add library
add_library(imgui_ios SHARED
    ${IMGUI_SOURCES}
    ${BACKEND_SOURCES}
    ${APP_SOURCES}
)

# Link libraries
target_link_libraries(imgui_ios
    ${log-lib}
    ${android-lib}
    ${GLESv2-lib}
    ${EGL-lib}
)

# Compiler flags
target_compile_options(imgui_ios PRIVATE -Wall -Wextra -O2)

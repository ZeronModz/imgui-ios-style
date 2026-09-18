cmake_minimum_required(VERSION 3.18)
project(imgui_ios)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(log-lib log)
find_library(android-lib android)
find_library(GLESv2-lib GLESv2)
find_library(EGL-lib EGL)

# Use relative paths from CMakeLists.txt location (android/app/src/main/jni/)
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../imgui")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)

set(BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_android.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/native_code.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ios_theme.cpp
)

include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

add_library(imgui_ios SHARED
    ${IMGUI_SOURCES}
    ${BACKEND_SOURCES}
    ${APP_SOURCES}
)

target_link_libraries(imgui_ios
    ${log-lib}
    ${android-lib}
    ${GLESv2-lib}
    ${EGL-lib}
)

target_compile_options(imgui_ios PRIVATE -Wall -Wextra -O2)

cmake_minimum_required(VERSION 3.18)
project(menu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(log-lib log)
find_library(android-lib android)
find_library(GLESv2-lib GLESv2)
find_library(EGL-lib EGL)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../imgui")

add_library(menu SHARED
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_android.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    native_code.cpp
)

target_include_directories(menu PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(menu ${log-lib} ${android-lib} ${GLESv2-lib} ${EGL-lib})
target_compile_options(menu PRIVATE -O2 -Wall -DIMGUI_IMPL_OPENGL_ES3)

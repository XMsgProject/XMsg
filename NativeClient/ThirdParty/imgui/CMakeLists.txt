cmake_minimum_required(VERSION 3.10)

project("imgui")

FILE(GLOB ImGuiSrc *.cpp)
FILE(GLOB ImGuiHeaders *.h)

add_library(imgui STATIC ${ImGuiSrc} ${ImGuiHeaders} "backends/imgui_impl_glfw.h" "backends/imgui_impl_glfw.cpp"
"backends/imgui_impl_opengl3.h" "backends/imgui_impl_opengl3.cpp")

target_link_libraries(imgui PRIVATE glad glfw)

target_include_directories(imgui PUBLIC "")

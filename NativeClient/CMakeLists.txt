cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("XMsg")

add_subdirectory("ThirdParty")

set(IMGUI_FILES 
"ThirdParty/imgui/imgui.cpp" 
"ThirdParty/imgui/imgui_demo.cpp" 
"ThirdParty/imgui/imgui_tables.cpp"
"ThirdParty/imgui/imgui_widgets.cpp" 
"ThirdParty/imgui/imgui_draw.cpp"
"ThirdParty/imgui/imgui.cpp"

"ThirdParty/imgui/imgui_internal.h"
"ThirdParty/imgui/imstb_rectpack.h"
"ThirdParty/imgui/imstb_textedit.h"
"ThirdParty/imgui/imstb_truetype.h"

"ThirdParty/imgui/backends/imgui_impl_opengl3.h"
"ThirdParty/imgui/backends/imgui_impl_glfw.h"

"ThirdParty/imgui/backends/imgui_impl_opengl3.cpp"
"ThirdParty/imgui/backends/imgui_impl_glfw.cpp"

)

add_executable(XMsg.Client
"src/main.cpp"
"src/Window/Window.h" "src/Window/Window.cpp"
"src/GUI/GUI.h" "src/GUI/GUI.cpp"
"src/LoginForm/LoginForm.h" "src/LoginForm/LoginForm.cpp" ${IMGUI_FILES}
"src/LoginForm/langs/lang_eu.h"
"src/LoginForm/langs/lang_ru.h"
"src/Font/Font.h" 
"src/Font/Font.cpp" 
"src/Api/Api.h" "src/Api/Api.cpp" 
"src/ChatForm/ChatForm.h" "src/ChatForm/ChatForm.cpp" "src/FormManager/FormManager.h" "src/FormManager/FormManager.cpp" "src/Texture/Texture.h" "src/Texture/Texture.cpp" "src/stb_image/stb_image.h" "src/stb_image/stb_image.cpp" "src/ChatForm/langs/lang_eu.h" "src/ChatForm/langs/lang_ru.h"
"src/stb_image_write/stb_image_write.cpp"
)

target_include_directories(XMsg.Client PRIVATE ThirdParty/imgui/)

target_link_libraries(XMsg.Client glfw glad OpenAL XPlatform.core XPlatform.net-headers XFmt XMsg.API-headers)

target_compile_features(XMsg.Client PRIVATE cxx_std_17)

XPLATFORM_COPY_FILE(XMsg.Client "${CMAKE_CURRENT_SOURCE_DIR}/configs/project.json" $<TARGET_FILE_DIR:XMsg.Client>)
XPLATFORM_COPY_FILE(XMsg.Client "${CMAKE_CURRENT_SOURCE_DIR}/configs/XPlatform.json" $<TARGET_FILE_DIR:XMsg.Client>/Engine/XPlatform.json)

XPLATFORM_ADD_DEPENDENCE(XMsg.Client OpenAL)
XPLATFORM_ADD_DEPENDENCE(XMsg.Client XPlatform.core)
XPLATFORM_ADD_DEPENDENCE_MODULE(XMsg.Client XPlatform.net)
XPLATFORM_ADD_DEPENDENCE_MODULE(XMsg.Client XMsg.API)

target_compile_definitions(XMsg.Client PRIVATE _XMSG_LANG_RU)

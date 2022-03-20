cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("Server")

add_executable(XMsg.Server "src/main.cpp" "src/Application/Application.h" "src/Application/Application.cpp" "src/Api/Api.h" "src/Api/Api.cpp" "src/ProtocolHandler/ProtocolHandler.h" "src/ProtocolHandler/ProtocolHandler.cpp" "src/UserBase/UserBase.h" "src/UserBase/UserBase.cpp" "src/stb_image/stb_image.cpp" "src/stb_image/stb_image.h")

target_link_libraries(XMsg.Server PRIVATE XPlatform.core XPlatform.net-headers XFmt)


if(WIN32)
target_link_libraries(XMsg.Server PRIVATE ws2_32 wsock32)

endif(WIN32)

XPLATFORM_COPY_FILE(XMsg.Server "${CMAKE_CURRENT_SOURCE_DIR}/configs/project.json" $<TARGET_FILE_DIR:XMsg.Server>)
XPLATFORM_COPY_FILE(XMsg.Server "${CMAKE_CURRENT_SOURCE_DIR}/configs/XPlatform.json" $<TARGET_FILE_DIR:XMsg.Server>/Engine/XPlatform.json)
XPLATFORM_COPY_FILE(XMsg.Server "${CMAKE_CURRENT_SOURCE_DIR}/certs/cert.pem" $<TARGET_FILE_DIR:XMsg.Server>/ssl/cert.pem)
XPLATFORM_COPY_FILE(XMsg.Server "${CMAKE_CURRENT_SOURCE_DIR}/certs/key.pem" $<TARGET_FILE_DIR:XMsg.Server>/ssl/key.pem)

XPLATFORM_COPY_FOLDER(XMsg.Server "${CMAKE_CURRENT_SOURCE_DIR}/assets" $<TARGET_FILE_DIR:XMsg.Server>/assets)

XPLATFORM_ADD_DEPENDENCE(XMsg.Server XPlatform.core)
XPLATFORM_ADD_DEPENDENCE_MODULE(XMsg.Server XPlatform.net)

target_compile_features(XMsg.Server PRIVATE cxx_std_17)
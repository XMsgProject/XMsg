cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project("XMsg.API")

add_library(XMsg.API SHARED "src/ext_main.cpp" "src/XMsg.h" "src/Interface/Interface.h" "src/Interface/Interface.cpp" "include/XMsg.Api/XMsg.h")
add_library(XMsg.API-headers INTERFACE)

target_include_directories(XMsg.API-headers INTERFACE "include/")
target_link_libraries(XMsg.API XPlatform.core XPlatform.net-headers XFmt)

if(WIN32)
target_link_libraries(XMsg.API wsock32 ws2_32)
endif(WIN32)
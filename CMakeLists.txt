cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project("XMsg")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

include("CMake/CMakeUtils.cmake")

add_subdirectory("XPlatform")
add_subdirectory("ThirdParty")

add_subdirectory("Api")

add_subdirectory("NativeClient")
add_subdirectory("Server")
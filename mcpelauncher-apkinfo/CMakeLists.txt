cmake_minimum_required(VERSION 3.0...4.0.0)
project(mcpelauncher-apkinfo)

set(CMAKE_CXX_STANDARD 11)

add_library(mcpelauncher-apkinfo src/apkinfo.cpp include/mcpelauncher/apkinfo.h)
target_include_directories(mcpelauncher-apkinfo PUBLIC include)
target_link_libraries(mcpelauncher-apkinfo axml_parser)

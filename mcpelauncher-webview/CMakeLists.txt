cmake_minimum_required(VERSION 2.8.12)

project(mcpelauncher-webview)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


find_package(Qt5 COMPONENTS Core Widgets WebEngineWidgets Qml REQUIRED)

add_executable(mcpelauncher-webview src/main.cpp src/qml.qrc src/main.h)
target_link_libraries(mcpelauncher-webview Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets Qt5::Qml)

if (QT_RPATH AND NOT QT_RPATH STREQUAL "")
    set_target_properties(mcpelauncher-webview PROPERTIES INSTALL_RPATH ${QT_RPATH})
endif()

install(TARGETS mcpelauncher-webview RUNTIME COMPONENT webview DESTINATION bin)

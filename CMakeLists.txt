cmake_minimum_required(VERSION 3.9)
project(imgui4xp VERSION 1.0.0 DESCRIPTION "imgui4xp X-Plane plugin")

include_directories("/opt/XSDK/CHeaders/XPLM")
include_directories("/opt/XSDK/CHeaders/Wrappers")
include_directories("/opt/XSDK/CHeaders/Widgets")
include_directories("/opt/XSDK/CHeaders")

include_directories("/src")
include_directories("/src/ImgWindow")
include_directories("/src/imgui")

if(WIN32)
    link_directories("/opt/XSDK/Libraries/Win")
    link_directories("/usr/x86_64-w64-mingw32/lib")
endif(WIN32)

if(APPLE)
    
elseif(UNIX)
    
endif()





add_subdirectory(src)

# imgui4xp build script.
#

cmake_minimum_required(VERSION 3.9)
project(imgui4xp VERSION 1.0.0 DESCRIPTION "imgui4xp X-Plane plugin")

set(CMAKE_CXX_STANDARD 14)

# Set include directories used by our code and dependencies.
include_directories("${CMAKE_SOURCE_DIR}/")
include_directories("${CMAKE_SOURCE_DIR}/imgui")
include_directories("${CMAKE_SOURCE_DIR}/imgui/misc/cpp")
include_directories("${CMAKE_SOURCE_DIR}/ImgWindow")
include_directories("${CMAKE_SOURCE_DIR}/XPSDK302/CHeaders/XPLM")
include_directories("${CMAKE_SOURCE_DIR}/XPSDK302/CHeaders/Widgets")
include_directories("${CMAKE_SOURCE_DIR}/XPSDK302/CHeaders/Wrappers")

# Specify library search locations.
if (WIN32)
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/XPSDK302/Libraries/Win")
elseif (APPLE)
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_SOURCE_DIR}/XPSDK302/Libraries/Mac")
elseif (UNIX)

endif ()

# Enable all X-Plane SDK APIs up to the newest version.
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)
# Define platform macros.
add_definitions(-DAPL=$<BOOL:${APPLE}> -DIBM=$<BOOL:${WIN32}> -DLIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>)

# Visual Studio defines compiler switches via CMakeSettings.json
if (NOT MSVC)
    # Force-enable exception support. This is most likely redundant, although for C
    # code the default is the opposite. Since we are mixing C++ and C libraries,
    # safer to set it on?
    add_compile_options(-fexceptions)
    # On UNIX systems this makes symbols non-exported by default. On Windows this
    # option is simply ignored, since symbol visibility works differently there.
    add_compile_options(-fvisibility=hidden)

    # Enable stricter warnings and then disable some we are not interested in.
    add_compile_options(-Wall -Wpedantic -Wshadow -Wextra)
    add_compile_options(-Wno-unused)

    # Always use position-independent code and highest optimization level (FPS!).
    add_compile_options(-O3)
    if (NOT WIN32)
        add_compile_options(-fPIC)
    endif()
endif ()

# X-Plane plugin
# FIXME: Split this into individual targets.
list(APPEND IMGUI4XP_SRCS
    imgui4xp.cpp
    imgui_starter_window.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    ImgWindow/ImgFontAtlas.cpp
    ImgWindow/ImgWindow.cpp
)
add_library(imgui4xp SHARED ${IMGUI4XP_SRCS})
target_compile_features(imgui4xp PUBLIC cxx_std_11)

if (APPLE)
    # X-Plane supports OS X 10.10+, so this should ensure FlyWithLua can run on
    # all supported versions.
    target_compile_options(imgui4xp PUBLIC -mmacosx-version-min=10.10)
    target_link_libraries(imgui4xp -mmacosx-version-min=10.10)
endif ()

# Link OpenGL and OpenAL related libraries.
# find_package(OpenGL REQUIRED)  # apt install freeglut3-dev
find_library(OPENGL_LIBRARY NAMES OpenGL)
find_library(OPENAL_LIBRARY NAMES openal OpenAL OpenAL32)  # apt install libopenal-dev
find_library(GLUT_LIBRARY NAMES glut GLUT glut64)  # apt install freeglut3-dev

# Link X-Plane plugin system libraries. They are only provided for OS X and Windows.
if (WIN32 OR APPLE)
# if (WIN32)
    find_library(XPLM_LIBRARY NAMES XPLM XPLM_64.lib)
    target_link_libraries(imgui4xp ${XPLM_LIBRARY})
#    find_library(XPWIDGETS_LIBRARY NAMES XPWidgets XPWidgets_64.lib)
#    target_link_libraries(imgui4xp ${XPLM_LIBRARY} ${XPWIDGETS_LIBRARY})
endif ()

# Link library for dynamic loading of shared objects on UNIX systems.
if (UNIX)
    find_library(DL_LIBRARY dl)
    target_link_libraries(imgui4xp ${DL_LIBRARY})
endif ()

# Link OS X core system libraries.
if (APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    find_library(GLUT_LIBRARY GLUT)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(Cocoa_LIBRARY Cocoa)

    target_link_libraries(imgui4xp ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY} ${GLUT_LIBRARY})
    target_link_libraries(imgui4xp ${OpenGL_LIBRARY} ${Cocoa_LIBRARY})

endif ()

if (WIN32)
    # Unlike OS X and Linux we build standard libraries statically since X-Plane does not provide them.
    if (MSVC)
        # Visual Studio includes many standard libraries already, just need to make sure OpenGL is linked
        target_link_libraries(imgui4xp opengl32)
    else ()
        target_link_libraries(imgui4xp -static-libgcc -static-libstdc++ opengl32)
        # MSYS2 for some reason always links against winpthread, so we have to work around it by statically linking the
        # entire plugin. This allows building the plugin nativaly on Windows.
        target_link_libraries(imgui4xp -static)
    endif ()
elseif (APPLE)
    # Restrict set of symbols exported from the plugin. This reduces changes of
    # conflict with other plugins, in particular ones with Lua interpreter
    # embedded.
    target_link_libraries(imgui4xp "-exported_symbols_list ${CMAKE_SOURCE_DIR}/imgui4xp.sym_mac")
    target_link_libraries(imgui4xp "-framework XPLM -framework XPWidgets -ldl")
elseif (UNIX)
    # Specify additional runtime search laths for dynamically-linked libraries.
    # Restrict set of symbols exported from the plugin. This reduces changes of
    # conflict with other plugins, in particular ones with Lua interpreter
    # embedded.
    target_link_libraries(imgui4xp -Wl,--version-script -Wl,${CMAKE_SOURCE_DIR}/imgui4xp.sym)
endif ()

set_target_properties(imgui4xp PROPERTIES PREFIX "")
set_target_properties(imgui4xp PROPERTIES OUTPUT_NAME "imgui4xp")
set_target_properties(imgui4xp PROPERTIES SUFFIX ".xpl")

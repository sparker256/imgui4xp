# Shared library without any Qt functionality
TEMPLATE = lib
QT -= gui core

CONFIG += warn_on plugin release
CONFIG -= thread exceptions qt rtti debug

VERSION = 1.0.0

INCLUDEPATH += /opt/XSDK/CHeaders/XPLM
INCLUDEPATH += /opt/XSDK/CHeaders/Wrappers
INCLUDEPATH += /opt/XSDK/CHeaders/Widgets
INCLUDEPATH += ../src
INCLUDEPATH += ../src/ImgWindow
INCLUDEPATH += ../src/imgui
INCLUDEPATH += ..
VPATH = ..

# Defined to use X-Plane SDK 3.0.0 capabilities - no backward compatibility before 11.10
DEFINES +=  XPLM301 \
            XPLM300 \
            XPLM210 \
            XPLM200


unix:!macx {
    DEFINES += APL=0 IBM=0 LIN=1 LINUX=1
    TARGET = lin.xpl
    # WARNING! This requires the latest version of the X-SDK !!!!
    QMAKE_CXXFLAGS += -fvisibility=hidden -O2 -Wall -Wextra -Wshadow -Wfloat-equal -Wformat -Wformat-security \
         --param ssp-buffer-size=4 -fstack-protector -D_FORTIFY_SOURCE=2 -std=gnu++11 -fpermissive
    QMAKE_CFLAGS += -fvisibility=hidden -O2 -Wall -Wextra -Wshadow -Wfloat-equal -Wformat -Wformat-security \
         --param ssp-buffer-size=4 -fstack-protector -D_FORTIFY_SOURCE=2
    LIBS += -ldl -Wl,--version-script -Wl,../src/exports.txt
    LIBS += -static-libgcc -static-libstdc++ -fPIC
}


HEADERS +=  ../src/imgui4xp.h \
            ../src/imgui_starter_window.h \
            ../src/SysOpenGL.h \
            ../src/XOGLUtils.h \
            ../src/stb_image.h \
            ../src/imgui/imconfig.h \
            ../src/imgui/imgui.h \
            ../src/imgui/imgui_internal.h \
            ../src/imgui/stb_rect_pack.h \
            ../src/imgui/stb_textedit.h \
            ../src/imgui/stb_truetype.h \
            ../src/ImgWindow/ImgWindow.h


SOURCES +=  ../src/imgui4xp.cpp \
            ../src/imgui_starter_window.cpp \
            ../src/imgui/imgui.cpp \
            ../src/imgui/imgui_demo.cpp \
            ../src/imgui/imgui_draw.cpp \
            ../src/ImgWindow/ImgWindow.cpp

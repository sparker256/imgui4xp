/*
 *   Imgui Starter Window for X-Plane
 *   William Good
 *
 *   This is a templete to allow you to use Imgui with X-Plane
 *
 *
 *
 *
 */
#ifndef SRC_IMGUIWIDGET_H_
#define SRC_IMGUIWIDGET_H_

#include <functional>
#include <map>
#include <memory>
#include <stdint.h>     // uint64_t
#include <cstring> // memcpy
#include <string.h>
#include "../src/ImgWindow/ImgWindow.h"

#include <XPLMGraphics.h>
#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>


class ImguiWidget: public ImgWindow {
public:
    ImguiWidget(int left, int top, int right, int bot, int decoration);
protected:
    void configureImguiContext() override;
    void buildInterface() override;
private:
    ImFont* font2;
    ImFont* font3;
    ImFont* font4;
    ImFont* font5;
    ImFont* font6;
    ImFont* font7;
    ImFont* font8;
    ImFont* font9;
};

#endif /* SRC_IMGUIWIDGET_H_ */

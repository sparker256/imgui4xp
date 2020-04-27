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

extern void configureImgWindow();

class ImguiWidget: public ImgWindow {
public:
    ImguiWidget(int left, int top, int right, int bot, int decoration);
protected:
    void buildInterface() override;
private:

};

#endif /* SRC_IMGUIWIDGET_H_ */

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

#include "../src/ImgWindow/ImgWindow.h"

extern void configureImgWindow();

class ImguiWidget: public ImgWindow {
protected:
    // texture number and size of the image we want to show
    static int      image_id;
    static ImVec2   image_size;
    // These are the values read/written from the UI
    int choice = 1;
    bool makeRed = false;
public:
    ImguiWidget(int left, int top, int right, int bot, XPLMWindowDecoration decoration);
protected:
    void buildInterface() override;
private:

};

#endif /* SRC_IMGUIWIDGET_H_ */

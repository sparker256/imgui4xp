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
#include <vector>

extern void configureImgWindow();

class ImguiWidget: public ImgWindow {
protected:
    // texture number and size of the image we want to show
    static int      image_id;
    static ImVec2   image_size;
    // These are the values read/written from the UI
    int choice = 1;
    bool makeRed = false;
    // Structure and data used for displaying a table
public:
    struct tableDataTy {
        std::string     reg;
        std::string     model;
        std::string     typecode;
        std::string     owner;
        float           heading = 0.0f;
        bool            turnsLeft = false;
        bool            filtered = true;    // included in search result?
        
        // is s (upper cased!) in any text?
        bool contains (const std::string& s) const;
        
    };
    typedef std::vector<tableDataTy> tableDataListTy;
protected:
    tableDataListTy     tableList;
public:
    ImguiWidget(int left, int top, int right, int bot, XPLMWindowDecoration decoration);
protected:
    void buildInterface() override;
private:

};

#endif /* SRC_IMGUIWIDGET_H_ */

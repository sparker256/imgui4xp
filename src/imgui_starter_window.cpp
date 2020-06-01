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

// All our headers combined
#include "imgui4xp.h"

// Image processing (for reading "imgui_demo.jpg"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void configureImgWindow();

// To show how global values synch between window instances we declare here 2 global variables
// Values in node "Drag Controls"
float       g_dragVal1  = 0.0f;
int         g_dragVal2  = 0;


// Trying to find a way to get a image to be displayed
const std::string IMAGE_NAME = "./Resources/plugins/imgui4xp/imgui_demo.jpg";

/// Uses "stb_image" library to load a picture into memory
/// @param fileName Path to image file
/// @param[out] imgWidth Image width in pixel
/// @param[out] imgHeight Image height in pixel
/// @return texture id
/// @exception std::runtime_error if image not found
int loadImage(const std::string& fileName, int& imgWidth, int& imgHeight) {
    int nComps;
    uint8_t *data = stbi_load(fileName.c_str(), &imgWidth, &imgHeight, &nComps, sizeof(uint32_t));

    if (!data) {
        throw std::runtime_error(std::string("Couldn't load image: ") + stbi_failure_reason());
    }

    int id;
    XPLMGenerateTextureNumbers(&id, 1);
    XPLMBindTexture2d(id, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA, imgWidth, imgHeight, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return id;
}

/// Wrapper around loadImage() capturing the possible exception
/// @param fileName Path to image file
/// @param[out] imgSize Image size in pixel
/// @return texture id for loaded image, or 0 in case of failure
int try2load_image(const std::string& fileName, ImVec2& imgSize) {
    try {
        int imgWidth=0, imgHeight=0;
        int ret = loadImage(fileName, imgWidth, imgHeight);
        imgSize.x = float(imgWidth);
        imgSize.y = float(imgHeight);
        return ret;
    } catch (const std::exception &e) {
        std::string err = std::string("imgui4xp Error: ") + e.what() + " in " + fileName + "\n";
        XPLMDebugString(err.c_str());
        return 0;
    }
}

void configureImgWindow()
{
  ImgWindow::sFontAtlas = std::make_shared<ImgFontAtlas>();

  // use actual parameters to configure the font, or use one of the other methods.

  // this is a post from kuroneko on x-plane.org explaining this use.

  // Basic setup looks something like:
  // To avoid bleeding VRAM like it's going out of fashion, there is only one font atlas shared over all ImgWindows
  // and we keep the manged pointer to it in the ImgWindow class statics.

  // I use the C++11 managed/smart pointers to enforce RAII behaviours rather than encouraging use of new/delete.
  //  This means the font atlas will only get destroyed when you break all references to it.
  // (ie: via ImgWindow::sFontAtlas.reset())  You should never really need to do that though,
  // unless you're being disabled (because you'll lose your texture handles anyway and it's probably a good idea
  // to forcibly tear down the font atlas then).

  // It's probably a bug that the instance of ImgWindow doesn't actually take a copy of the shared_ptr to ensure
  // the font atlas remains valid until it's destroyed.  I was working on a lot of things when I threw that update
  // together and I was heading down that path, but I think I forgot to finish it.


  // you can use any of these fonts that are provided with X-Plane or find you own.
  // Currently you can only load one font and not sure if this might change in the future.
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSans.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Inconsolata.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/ProFontWindows", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Bold.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/RobotoCondensed-Regular.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Light.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 13.0f);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/tahomabd.ttf", 13.0f);

  ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 13.0f);
}

//
// MARK: ImguiWidget (our example implementation of ImguiWindow)
//

// texture number and size of the image we want to show
// (static, because we want to load the image into a texture just once)
int      ImguiWidget::image_id = 0;
ImVec2   ImguiWidget::image_size;

// Counter for the number of windows opened
int      ImguiWidget::num_win = 0;

ImguiWidget::ImguiWidget(int left, int top, int right, int bot,
                         XPLMWindowDecoration decoration,
                         XPLMWindowLayer layer) :
    ImgWindow(left, top, right, bot, decoration, layer),
    myWinNum(++num_win)             // assign a unique window number
{
    SetWindowTitle("Imgui v" IMGUI_VERSION " for X-Plane  by William Good");
    SetVisible(true);
    
    // Initialize the list content
    listContent = {
        "1st line", "2nd line", "3rd line", "4th line", "5th line",
        "6th line", "7th line", "8th line", "9th line", "10th line"
    };
    
    // if not yet loaded: try loading an image for display
    if (!image_id)
        image_id = try2load_image(IMAGE_NAME, image_size);
}

void ImguiWidget::buildInterface() {

    float win_width = ImGui::GetWindowWidth();
    float win_height = ImGui::GetWindowHeight();

    ImGui::TextUnformatted("Hello, World!");
    
    // Button with fixed width 30 and standard height
    // to pop out the window in an OS window
    if (!IsPoppedOut()) {
        // Same line, but right-alinged
        static float btnWidth = ImGui::CalcTextSize("Pop out").x + 5;
        ImGui::SameLine(ImGui::GetWindowContentRegionWidth()-btnWidth);
        if (ImGui::Button("Pop out", ImVec2(btnWidth,0)))
            SetWindowPositioningMode(xplm_WindowPopOut);
    }

    // Grouping a few lines...
    ImGui::BeginGroup();
    ImGui::Text("Window size: width = %f  height = %f", win_width, win_height);
    
    ImGui::TextUnformatted("Two Widgets");
    ImGui::SameLine();
    ImGui::TextUnformatted("One Line.");
    ImGui::EndGroup();
    
    // ...to put the side-by-side with another group of lines
    ImGui::SameLine(0.0f, 20.0f);
    ImGui::BeginGroup();
    ImGui::Text("Window #%d", myWinNum);
    ImGui::Text("X-Plane Window Id: %p", GetWindowId());
    ImGui::EndGroup();

    if (ImGui::TreeNode("Styling Widgets")) {
        const char *text = "Centered Text";
        ImVec2 text_size = ImGui::CalcTextSize(text, NULL, true);
        float center = win_width / 2 - text_size[0] / 2;
        ImGui::SetCursorPosX(center);
        ImGui::TextUnformatted(text);
        ImVec4 col = ImColor(43, 101, 236, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted("Ocean Blue Text");
        ImGui::TextUnformatted("Also Ocean Blue");
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("No longer Ocean Blue");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Buttons")) {
        if (ImGui::Button("Push Me")) {
            makeRed = !makeRed;
        }

        if (makeRed) {
            ImVec4 col = ImColor(255, 0, 0, 255);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        } else {
            ImVec4 col = ImColor(0, 255, 0, 255);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        }

        ImGui::TextUnformatted("Some Text");
        ImGui::PopStyleColor();

        ImGui::Button("Click and Hold");
        if (ImGui::IsItemActive()) {
            makeRed = !makeRed;
        }

        if (ImGui::RadioButton("Choice 1", radioChoice == 1)) {
            radioChoice = 1;
        }

        if (ImGui::RadioButton("Choice 2", radioChoice == 2)) {
            radioChoice = 2;
        }

        if (ImGui::RadioButton("Choice 3", radioChoice == 3)) {
            radioChoice = 3;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Checkboxes")) {
        ImGui::Checkbox("Make text red", &makeRed);
        if (makeRed) {
            ImVec4 col = ImColor(255, 0, 0, 255);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        } else {
            ImVec4 col = ImColor(0, 255, 0, 255);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        }

        ImGui::TextUnformatted("Some Text");
        ImGui::PopStyleColor();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sliders")) {
        ImGui::SliderFloat("Slider [0..1000]", &sliderVal1, 0, 1000, "Value %.2f");
        ImGui::SliderFloat("Power Slider [0..100000]", &sliderVal2, 0, 100000, "Value %.2f", 3.0);
        ImGui::SliderInt("Int Slider [0..100]", &sliderVal3, 0, 100, "Value %.0f");
        ImGui::SliderAngle("Angle Slider", &sliderAngle, -180, 180);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("ComboBox")) {
        static const char * choices[] = {"Choice 1", "Choice 2", "Choice 3"};
        if (ImGui::BeginCombo("Combo Box", choices[choice1])) {
            for (int i = 0; i < 3; i++) {
                if (ImGui::Selectable(choices[i], choice1 == i))
                    choice1 = i;
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Combo Box 2", "", ImGuiComboFlags_NoPreview)) {
            if (ImGui::Selectable("Choice A", choice2 == 1)) {
                choice2 = 1;
            }
            if (ImGui::Selectable("Choice B", choice2 == 2)) {
                choice2 = 2;
            }
            if (ImGui::Selectable("Choice C", choice2 == 3)) {
                choice2 = 3;
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drag Controls / Global Variables")) {
        ImGui::DragFloat("Drag Float", &g_dragVal1, 1.0, 0, 1000, "%.2f", 1.0);
        ImGui::DragInt("Drag Int", &g_dragVal2, 1.0, 0, 1000, "%.2f");
        ImGui::TextUnformatted("Note: This values are global and synched between windows.");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Plots")) {
        static float values[] = {1, 3, 2, 5, 0, 6, -3};
        ImGui::PlotLines("Value Plot", values, IM_ARRAYSIZE(values));
        ImGui::PlotLines("", values, IM_ARRAYSIZE(values) - 1, 1, "Plot", -10, FLT_MAX, ImVec2(win_width, 100));
        ImGui::PlotHistogram("Value Histogram", values, IM_ARRAYSIZE(values));
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Input")) {
        // Uses stdlib wrapper implemented in imgui/misc/cpp/imgui_stdlib.c/.h
        ImGui::InputText ("Text", &userText);
        ImGui::InputInt("Input int", &userI1);
        ImGui::InputInt("Input int2", &userI2, 10);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drawing")) {
        // local cx, cy = imgui.GetCursorScreenPos()
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // -- Create a kind of canvas of size 100x100.
        // -- The previous variable remembers the top left
        // -- edge of the canvas so we can use it as relative
        // -- coordinates to draw inside the widget using
        // -- absolute coordinates.
        // imgui.Dummy(100, 100)
        ImVec2 canvas_size(100, 100);
        ImGui::Dummy(canvas_size);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        float thickness = 2.0;
        // -- Parameters: x1, y1, x2, y2, color, thickness
        // ImGui::DrawList_AddLine(cx, cy, cx + 20, cy + 20, 0xFF0000FF, 2);
        // ImGui::DrawList_AddLine(cx, cy + 20, cx + 20, cy, 0xFF0000FF, 2);
        draw_list->AddLine(ImVec2(pos.x, pos.y), ImVec2(pos.x + 20, pos.y + 20), IM_COL32(255, 0, 0, 255), thickness);
        draw_list->AddLine(ImVec2(pos.x, pos.y + 20), ImVec2(pos.x + 20, pos.y), IM_COL32(255, 0, 0, 255), thickness);

        float roundness = 0.5;
        // -- Prameters: x1, y1, x2, y2, color, roundness
        // imgui.DrawList_AddRect(cx, cy, cx + 20, cy + 20, 0xFFFF0000, 0.5);
        draw_list->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + 20, pos.y + 20), IM_COL32(0, 0, 255, 255), roundness);

        // -- Prameters: x1, y1, x2, y2, color, roundness
        // imgui.DrawList_AddRectFilled(cx + 20, cy, cx + 40, cy + 20, 0xFFFF0000, 0.5);
        draw_list->AddRectFilled(ImVec2(pos.x + 20, pos.y), ImVec2(pos.x + 40, pos.y + 20), IM_COL32(0, 0, 255, 255), roundness);

        // -- Parameters: x1, y1, x2, y2, x3, y3, color
        // imgui.DrawList_AddTriangle(cx, cy + 50, cx + 40, cy + 50, cx + 20, cy + 20, 0xFF00FF00);
        draw_list->AddTriangle(ImVec2(pos.x, pos.y + 50), ImVec2(pos.x + 40, pos.y + 50), ImVec2(pos.x + 20, pos.y + 20), IM_COL32(0, 255, 0, 255), thickness);

        // -- Parameters: x1, y1, x2, y2, x3, y3, color
        // imgui.DrawList_AddTriangleFilled(cx + 40, cy + 50, cx + 80, cy + 50, cx + 40, cy + 20, 0xFF00FF00);
        draw_list->AddTriangleFilled(ImVec2(pos.x + 40, pos.y + 50), ImVec2(pos.x + 80, pos.y + 50), ImVec2(pos.x + 40, pos.y + 20), IM_COL32(0, 255, 0, 255));

        // -- Parameters: x1, y1, r, color
        // imgui.DrawList_AddCircle(cx + 80, cy + 20, 20, 0xFF00FF00);
        draw_list->AddCircle(ImVec2(pos.x + 80, pos.y + 20), 20, IM_COL32(0, 255, 0, 255));

        // -- Parameters: x1, y1, r, color
        // imgui.DrawList_AddCircleFilled(cx + 80, cy + 20, 10, 0xFF00FF00);
        draw_list->AddCircleFilled(ImVec2(pos.x + 80, pos.y + 20), 10, IM_COL32(0, 255, 0, 255));

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Images")) {
        ImGui::Text("image_id = %d", image_id);
        // Draw a previously loaded image
        if (image_id)
            ImGui::Image((void*)(intptr_t)image_id, image_size);

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Misc", ImGuiTreeNodeFlags_Selected)) {
        // Create a bullet style enumeration
        ImGui::Bullet(); ImGui::TextUnformatted("Bullet");
        ImGui::Bullet(); ImGui::TextUnformatted("Style");
        ImGui::Bullet(); ImGui::TextUnformatted("Enumeration");

        // Draw a separation line
        ImGui::Separator();

        // Show a progress bar, 1 = 100%
        ImGui::ProgressBar(0.5);

        ImGui::Button("Button##1");
        ImGui::Button("Button##2");

        for (int i = 0; i < 5; i++) {
            ImGui::PushID(i);
            ImGui::Button("Button");
            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    // Let#s play with lists
    if (ImGui::TreeNode("List")) {
        ImGui::ListBox ("A simple List Box", &listSelItem,
                        listContent.data(),
                        int(listContent.size()),
                        int(listContent.size())/2);  // half as high as number of list elements to show how vertical scrolling works
        ImGui::TreePop();
    }

    // Let#s play with Columns
    if (ImGui::TreeNode("Columns")) {
        ImGui::Columns(3);
        ImGui::Text("first");
        ImGui::NextColumn();
        ImGui::Text("second");
        ImGui::NextColumn();
        ImGui::Text("third");
        ImGui::NextColumn();
        ImGui::Text("first2");
        ImGui::NextColumn();
        ImGui::Text("second2");
        ImGui::NextColumn();
        ImGui::Text("third2");
        ImGui::NextColumn();
        ImGui::Text("first3");

        ImGui::Columns();

        ImGui::TreePop();

    }


    if (ImGui::TreeNode("Fonts")) {

        ImGui::Text("Default DejaVuSansMono.ttf 13.0f font \n");

        ImGui::TextUnformatted("");
        // Green color
        ImVec4 col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted("Some Green Text with DejaVuSansMono 13 font");
        ImGui::PopStyleColor();

        ImGui::TextUnformatted("");
        // Red color
        col = ImColor(255, 0, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::SetWindowFontScale(1.5);
        ImGui::TextUnformatted("Some Red Text with DejaVuSansMono 13 font with font scale of 1.5");
        ImGui::SetWindowFontScale(1.0);
        ImGui::PopStyleColor();

        ImGui::TextUnformatted("");
        // Blue color
        col = ImColor(0, 0, 255, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::SetWindowFontScale(1.25);
        ImGui::TextUnformatted("Some Blue Text with DejaVuSansMono 13 font with font scale of 1.25");
        ImGui::SetWindowFontScale(1.0);
        ImGui::PopStyleColor();

        ImGui::TextUnformatted("");
        ImGui::Text("Using ShowStyleEditor() to see if new fonts have loaded\n");
        ImGui::ShowStyleEditor();
        ImGui::TreePop();

    }
}

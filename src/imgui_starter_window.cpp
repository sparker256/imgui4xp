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
#include "XPLMDisplay.h"
#include "imgui_starter_window.h"
#include "../src/imgui/imgui.h"
#include "../src/ImgWindow/ImgWindow.h"
#include "../src/ImgWindow/ImgFontAtlas.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include <stdint.h>     // uint64_t
#include <cstring> // memcpy
#include <string.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<GLuint> textureIDs;

void configureImgWindow();

int count;
int choice = 1;
float win_width = 0;
float win_height = 0;
float cursor_posy = 0;
ImVec2 text_size;
float center;
bool makeRed = false;

// Trying to find a way to get a image to be displayed
ImTextureID Imimage_id;
int image_id;
std::string image_name = "./Resources/plugins/imgui4xp/imgui_demo.jpg";

int loadImage(const std::string&fileName) {
    int imgWidth, imgHeight, nComps;
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
    textureIDs.push_back(id);

    return id;
}

int try2load_image() {
    try {
        image_id = loadImage(image_name);
        return 1;
    } catch (const std::exception &e) {
        std::string err = std::string("imgui4xp Error: ") + e.what() + " in " + image_name + "\n";
        XPLMDebugString(err.c_str());
        return 0;
    }
}

void configureImgWindow()
{
  ImgWindow::sFontAtlas = std::make_shared<ImgFontAtlas>();
  // use actual parameters to configure the font, or use one of the other methods.
  ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 13.0f);
}


ImguiWidget::ImguiWidget(int left, int top, int right, int bot, int decoration):
    ImgWindow(left, top, right, bot, decoration)
{
    SetWindowTitle("Imgui for X-Plane  by William Good");
    SetVisible(true);
    try2load_image();
}

void ImguiWidget::buildInterface() {

    win_width = ImGui::GetWindowWidth();
    win_height = ImGui::GetWindowHeight();

    ImGui::TextUnformatted("Hello, World!");

    ImGui::Text("Window size: width = %f  height = %f", win_width, win_height);

    ImGui::TextUnformatted("Two Widgets");

    ImGui::SameLine();
    ImGui::TextUnformatted("One Line.");

    if (ImGui::TreeNode("Styling Widgets")) {
        const char *text = "Centered Text";
        text_size = ImGui::CalcTextSize(text, NULL, true);
        center = win_width / 2 - text_size[0] / 2;
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

        if (ImGui::RadioButton("Choice 1", choice == 1)) {
            choice = 1;
        }

        if (ImGui::RadioButton("Choice 2", choice == 2)) {
            choice = 2;
        }

        if (ImGui::RadioButton("Choice 3", choice == 3)) {
            choice = 3;
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
        static float sliderVal;
        ImGui::SliderFloat("Slider Caption", &sliderVal, 0, 100000, "Value %.2f");

        static float sliderVal2;
        ImGui::SliderFloat("Power Slider", &sliderVal2, 0, 100000, "Value %.2f", 3.0);

        static int sliderVal3;
        ImGui::SliderInt("Int Slider", &sliderVal3, 0, 100000, "Value %.0f");

        static float angle;
        ImGui::SliderAngle("Angle Slider", &angle, -180, 180);


        ImGui::TreePop();
    }

    if (ImGui::TreeNode("ComboBox")) {
        static const char * choices[] = {"Choice 1", "Choice 2", "Choice 3"};
        if (ImGui::BeginCombo("Combo Box", choices[choice])) {
            int i;
            for (i = 0; i < 3; i++) {
                if (ImGui::Selectable(choices[i], choice == i)) {
                    choice = i;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Combo Box 2", "", ImGuiComboFlags_NoPreview)) {
            if (ImGui::Selectable("Choice A", choice == 1)) {
                choice = 1;
            }
            if (ImGui::Selectable("Choice B", choice == 2)) {
                choice = 2;
            }
            if (ImGui::Selectable("Choice C", choice == 3)) {
                choice = 3;
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drag Controls")) {
        static float sliderVal;
        ImGui::DragFloat("Drag Float", &sliderVal, 1.0, 0, 1000, "%.2f", 1.0);
        static int sliderVal2;
        ImGui::DragInt("Drag Int", &sliderVal2, 1.0, 0, 1000, "%.2f");
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
        static char text[255];
        ImGui::InputText("Text", text, IM_ARRAYSIZE(text));

        static int i0 = 123;
        ImGui::InputInt("Input int", &i0);

        static int i02 = 1234;
        ImGui::InputInt("Input int2", &i02, 10);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drawing")) {
        // local cx, cy = imgui.GetCursorScreenPos()
        static ImVec2 pos = ImGui::GetCursorScreenPos();

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
        // ImGui::Image((void*)(intptr_t)my_opengl_texture, ImVec2(my_image_width, my_image_height));
        ImGui::Image((void*)(intptr_t)image_id, ImVec2(800, 450));
        // Prameters: image id returned by float_wnd_load_image, diplay width, display height

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Misc")) {
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

    if (ImGui::TreeNode("Fonts")) {
        ImGui::Text("Using ShowStyleEditor() to see if new fonts have loaded\n");
        ImGui::ShowStyleEditor();
        ImGui::Text("DejaVuSans.ttf 13.0f \n");
        ImGui::TreePop();

    }
}

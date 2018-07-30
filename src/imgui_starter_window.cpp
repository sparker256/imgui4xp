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


int count;

ImguiWidget::ImguiWidget(int left, int top, int right, int bot, int decoration):
    ImgWindow(left, top, right, bot, decoration)
{
    SetWindowTitle("Imgui for X-Plane  by William Good");
    SetVisible(true);

}

void ImguiWidget::buildInterface() {

    ImGui::Text("This is my first try");
    if (ImGui::Button("First Button")) {
        count = count + 1;
    }
    ImGui::Text("count = %d", count);

}

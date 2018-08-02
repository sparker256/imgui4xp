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

#include <stdint.h>     // uint64_t
#include <cstring> // memcpy
#include <string.h>
#include <string>

int count;
int choice = 1;
float win_width = 0;
float win_height = 0;
float cursor_posy = 0;
ImVec2 text_size;
float center;
bool makeRed = false;

ImguiWidget::ImguiWidget(int left, int top, int right, int bot, int decoration):
    ImgWindow(left, top, right, bot, decoration)
{
    SetWindowTitle("Imgui for X-Plane  by William Good");
    SetVisible(true);

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
}

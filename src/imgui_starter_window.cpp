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
#include "imgui_internal.h"

// Image processing (for reading "imgui_demo.jpg"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//
// MARK: ImGui extension: formatted IDs
//

namespace ImGui {

/// @brief Helper for creating unique IDs
/// @details Required when creating many widgets in a loop, e.g. in a table
IMGUI_API void PushID_formatted(const char* format, ...)    IM_FMTARGS(1);

IMGUI_API void PushID_formatted(const char* format, ...)
{
    // format the variable string
    va_list args;
    char sz[500];
    va_start (args, format);
    vsnprintf(sz, sizeof(sz), format, args);
    va_end (args);
    // Call the actual push function
    PushID(sz);
}

/// @brief Button with on-hover popup helper text
/// @param label Text on Button
/// @param tip Tooltip text when hovering over the button (or NULL of none)
/// @param colFg Foreground/text color (optional, otherwise no change)
/// @param colBg Background color (optional, otherwise no change)
/// @param size button size, 0 for either axis means: auto size
IMGUI_API bool ButtonTooltip(const char* label,
                             const char* tip = nullptr,
                             ImU32 colFg = IM_COL32(1,1,1,0),
                             ImU32 colBg = IM_COL32(1,1,1,0),
                             const ImVec2& size = ImVec2(0,0))
{
    // Setup colors
    if (colFg != IM_COL32(1,1,1,0))
        ImGui::PushStyleColor(ImGuiCol_Text, colFg);
    if (colBg != IM_COL32(1,1,1,0))
        ImGui::PushStyleColor(ImGuiCol_Button, colBg);

    // do the button
    bool b = ImGui::Button(label, size);
    
    // restore previous colors
    if (colBg != IM_COL32(1,1,1,0))
        ImGui::PopStyleColor();
    if (colFg != IM_COL32(1,1,1,0))
        ImGui::PopStyleColor();

    // do the tooltip (using stationary TimedTooltip with Markdown support)
    if (tip)
        ImGui::TimedTooltip::MD::Text("%s", tip);
    
    // return if button pressed
    return b;
}

/// @brief 1-Click Toolbar Button that bypasses ActiveId blocking (Lance's Pattern)
/// @param label Text on Button
/// @param tip Optional Markdown tooltip text
/// @param size Button size (0 for auto)
IMGUI_API bool ToolbarButton(const char* label,
                             const char* tip = nullptr,
                             const ImVec2& size = ImVec2(0, 0))
{
    bool clicked = ImGui::Button(label, size);
    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (!clicked && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        clicked = true;
    }
    if (tip && hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), hovered, "%s", tip);
    }
    return clicked;
}

/// Lance's 1-Click Active-ID Toolbar Checkbox:
/// Allows checkboxes to toggle on the first click even when InputTextMultiline holds g.ActiveId,
/// eliminating the two-click penalty, preventing double-toggle flickering, and preserving active text selection.
IMGUI_API bool ToolbarCheckbox(const char* label, bool* v, const char* tip = nullptr)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiID id = window->GetID(label);

    bool editor_was_active = (g.ActiveId != 0 && g.InputTextState.ID != 0 && g.ActiveId == g.InputTextState.ID);
    bool editor_just_deactivated = (g.ActiveId == 0 && (g.InputTextDeactivatedState.ID != 0 || g.DeactivatedItemData.ID != 0));
    bool active_id_blocked = (g.ActiveId != 0 && g.ActiveId != id);

    bool changed = ImGui::Checkbox(label, v);
    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    if (!changed && (active_id_blocked || editor_just_deactivated) && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        *v = !(*v);
        changed = true;
        if (editor_was_active) {
            g.InputTextReactivateId = g.ActiveId;
        } else if (editor_just_deactivated) {
            ImGuiID reactivate_id = (g.InputTextDeactivatedState.ID != 0) ? g.InputTextDeactivatedState.ID : g.DeactivatedItemData.ID;
            g.InputTextReactivateId = reactivate_id;
            if (g.ActiveId == id) {
                ImGui::ClearActiveID();
            }
        }
    }

    if (tip && hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), hovered, "%s", tip);
    }
    return changed;
}

/// Lance's 1-Click Active-ID Toolbar ToggleButton:
/// Allows animated ToggleButton controls to toggle on the first click even when InputTextMultiline
/// holds g.ActiveId, eliminating the two-click penalty, preserving active text selection,
/// and supporting mouse-wheel cycling and TimedTooltip markdown tooltips.
IMGUI_API bool ToolbarToggleButton(const char* label, bool* v, const char* off_tooltip, const char* on_tooltip)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiID id = window->GetID(label);

    bool editor_was_active = (g.ActiveId != 0 && g.InputTextState.ID != 0 && g.ActiveId == g.InputTextState.ID);
    bool editor_just_deactivated = (g.ActiveId == 0 && (g.InputTextDeactivatedState.ID != 0 || g.DeactivatedItemData.ID != 0));
    bool active_id_blocked = (g.ActiveId != 0 && g.ActiveId != id);

    bool changed = ImGui::ToggleButton(label, v, off_tooltip, on_tooltip);
    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    if (!changed && (active_id_blocked || editor_just_deactivated) && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        *v = !(*v);
        changed = true;
        if (editor_was_active) {
            g.InputTextReactivateId = g.ActiveId;
        } else if (editor_just_deactivated) {
            ImGuiID reactivate_id = (g.InputTextDeactivatedState.ID != 0) ? g.InputTextDeactivatedState.ID : g.DeactivatedItemData.ID;
            g.InputTextReactivateId = reactivate_id;
            if (g.ActiveId == id) {
                ImGui::ClearActiveID();
            }
        }
    } else if (editor_was_active && hovered) {
        ImGuiIO& io = ImGui::GetIO();
        float wheel = io.MouseWheel != 0.0f ? io.MouseWheel : io.MouseWheelH;
        if (wheel != 0.0f) {
            if (wheel > 0.0f && !(*v)) {
                *v = true;
                changed = true;
            } else if (wheel < 0.0f && *v) {
                *v = false;
                changed = true;
            }
            io.MouseWheel = 0.0f;
            io.MouseWheelH = 0.0f;
        }
    }

    return changed;
}

IMGUI_API bool ToolbarToggleButton(const char* label, bool* v, const char* tip = nullptr)
{
    return ToolbarToggleButton(label, v, tip, tip);
}

inline float CalcToggleButtonWidth(const char* label_text) {
    float font_size = ImGui::GetFontSize();
    float height = std::floor(font_size * 0.78f + 0.5f);
    float width = std::floor(height * 1.90f + 0.5f);
    const char* id_pos = std::strstr(label_text, "##");
    ImVec2 text_size = id_pos ? ImGui::CalcTextSize(label_text, id_pos) : ImGui::CalcTextSize(label_text);
    return width + ImGui::GetStyle().ItemInnerSpacing.x + text_size.x;
}

}
using ImGui::ToolbarButton;
using ImGui::ToolbarCheckbox;
using ImGui::ToolbarToggleButton;
using ImGui::CalcToggleButtonWidth;

//
// MARK: Global data and functions
//

// The raw TTF data of OpenFontIcons has been generated into the following file
#include "fa-solid-900.inc"
#include "gf-roboto-bold-italic.inc"

// Initial data for the example table
ImguiWidget::tableDataListTy TABLE_CONTENT = {
    {"6533","MH-65C Dolphin","AS65","United States Coast Guard",0.0f,false},
    {"N493TR","SR22T","S22T","Aircraft Guaranty Corp Trustee",0.0f,true},
    {"N77FK","G-IV","GLF4","Wilmington Trust Co Trustee",0.0f,true},
    {"N911XB","EC135T1","EC35","Air Med Services Llc",0.0f,false},
    {"OY-JRJ","Avions de Transport Regional ATR 42 310","AT43","Danish Air Transport",0.0f,false},
    {"CB-8001","C-17A Globemaster III","C17","Indian Air Force",0.0f,false},
    {"G-DVIP","AGUSTA A109E","A109","Castle Air",0.0f,true},
    {"OE-KSD","91 D Safir","SB91","Patrick Lohr",0.0f,false},
    {"D-ITOR","Citation CJ2+","C25A","Hormann Kg",0.0f,false},
    {"N544XL","Citation Excel","C56X","High Tec Industries Services Inc",0.0f,true},
    {"N368MS","R44 II","R44","Silvestri Mark J",0.0f,true},
    {"N451QX","DHC-8-402","DH8D","Horizon Air Industries Inc",0.0f,true},
    {"N1125J","1125 WESTWIND ASTRA","ASTR","Djb Air Llc",0.0f,false},
    {"N250SH","AS 350 B2","AS50","Sundance Helicopters Inc",0.0f,true},
};

// To show how global values synch between window instances we declare here 2 global variables
// Values in node "Drag Controls"
float       g_dragVal1  = 0.0f;
int         g_dragVal2  = 0;


// Trying to find a way to get a image to be displayed
// Statically embedded 1x1 RGBA pixel test array (user test pattern):
static const unsigned char s_AvionicsPixelPng[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0x15, 0xc4, 0x89, 0x00, 0x00, 0x00,
    0x0a, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0x00, 0x01, 0x00, 0x00,
    0x05, 0x00, 0x01, 0x0d, 0x0a, 0x2d, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

// Statically embedded 16x16 avionics radio beacon icon (emerald radar pulse rings):
static const unsigned char s_AvionicsIconPng[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
    0x4f, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0x60, 0xa0, 0x36, 0xd0,
    0x3b, 0x53, 0x98, 0x82, 0x0f, 0x13, 0xa5, 0x99, 0x2c, 0x79, 0x64, 0x09,
    0x20, 0xfb, 0x0e, 0x36, 0x8c, 0x4d, 0x2d, 0x86, 0xc9, 0xe8, 0x8a, 0xd1,
    0x0d, 0xc5, 0xea, 0x12, 0x42, 0x9a, 0x71, 0x19, 0x82, 0xd5, 0x00, 0x24,
    0xb1, 0xff, 0xc8, 0x18, 0xd9, 0x90, 0xe1, 0x6e, 0x00, 0xd9, 0x81, 0x48,
    0x51, 0x34, 0xa2, 0x9b, 0x48, 0x72, 0x42, 0xc2, 0xe6, 0x12, 0x72, 0xe4,
    0x51, 0x14, 0x91, 0x95, 0x99, 0xc8, 0x01, 0x00, 0x3a, 0x65, 0xcf, 0x6d,
    0xc5, 0x47, 0x7e, 0x46, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82
};

const std::string IMAGE_NAME = "./Resources/plugins/imgui4xp/imgui_demo.jpg";

// Font size, also roughly defines height of one line
constexpr float FONT_SIZE = 13.0f;

/// Uses "stb_image" library to load a picture into memory
/// @param fileName Path to image file
/// @param[out] imgWidth Image width in pixel
/// @param[out] imgHeight Image height in pixel
/// @return texture id
/// @exception std::runtime_error if image not found
ImTextureID loadImage(const std::string& fileName, int& imgWidth, int& imgHeight) {
    int nComps;
    uint8_t *data = stbi_load(fileName.c_str(), &imgWidth, &imgHeight, &nComps, 4);

    if (!data) {
        throw std::runtime_error(std::string("Couldn't load image: ") + stbi_failure_reason());
    }

    // Unified Multiplexing API: Let ImgWindow abstract Panel Graphics vs OpenGL fallback
    ImTextureID id = ImgWindow::CreateTexture(data, imgWidth, imgHeight);

    stbi_image_free(data);

    return id;
}

/// Wrapper around loadImage() capturing the possible exception
/// @param fileName Path to image file
/// @param[out] imgSize Image size in pixel
/// @return texture id for loaded image, or 0 in case of failure
ImTextureID try2load_image(const std::string& fileName, ImVec2& imgSize) {
    try {
        int imgWidth=0, imgHeight=0;
        ImTextureID ret = loadImage(fileName, imgWidth, imgHeight);
        imgSize.x = float(imgWidth);
        imgSize.y = float(imgHeight);
        return ret;
    } catch (const std::exception &e) {
        std::string err = std::string("imgui4xp Error: ") + e.what() + " in " + fileName + "\n";
        XPLMDebugString(err.c_str());
        return (ImTextureID)0;
    }
}

// Helper: Turns a string upper case
inline std::string& toupper (std::string& s)
{
    std::for_each(s.begin(), s.end(), [](char& c) { c = toupper(c); });
    return s;
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


  // you can use any of these fonts that are provided with X-Plane 12 or find you own.
  // We can now use more than one font so will show two.
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/B612Mono-Regular.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSans.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Inconsolata.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Menlo-Regular.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/NotoSansCJK-SC-Regular.otf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Bold.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/RobotoCondensed-Regular.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Light.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", FONT_SIZE);
  // ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/tahomabd.ttf", FONT_SIZE);

  // 1. Primary Default UI Font (Fonts[0]) - Proportional Roboto-Regular 14px
  Font7 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Regular.ttf", 14);
  if (!Font7) {
      Font7 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 14);
  }
  if (!Font7) {
      Font7 = ImgWindow::sFontAtlas->AddFontDefault();
  }

  // Merge FontAwesome icons directly into primary UI font (Fonts[0])
  ImFontConfig icon_config;
  icon_config.MergeMode = true;
  static ImVector<ImWchar> icon_ranges;
  ImFontGlyphRangesBuilder builder;
  builder.AddText(ICON_FA_TRASH_ALT ICON_FA_SEARCH
                  ICON_FA_EXTERNAL_LINK_SQUARE_ALT
                  ICON_FA_WINDOW_MAXIMIZE ICON_FA_WINDOW_MINIMIZE
                  ICON_FA_WINDOW_RESTORE ICON_FA_WINDOW_CLOSE
                  ICON_FA_MOUSE_POINTER ICON_FA_MOUSE ICON_FA_HAND_POINTER
                  ICON_FA_CHEVRON_DOWN ICON_FA_CHEVRON_UP);
  builder.BuildRanges(&icon_ranges);
  ImgWindow::sFontAtlas->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data,
                                                        fa_solid_900_compressed_size,
                                                        14.0f,
                                                        &icon_config,
                                                        icon_ranges.Data);

  // 2. Bold and Heading Fonts
  Font8 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Bold.ttf", 15);
  if (!Font8) {
      Font8 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Bold.ttf", 15);
  }
  if (!Font8) {
      Font8 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 15);
  }
  Font9 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Bold.ttf", 20);
  if (!Font9) {
      Font9 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Bold.ttf", 20);
  }
  if (!Font9) {
      Font9 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 20);
  }
  FontBold = Font8;

  // 3. Italic Font - Roboto-Italic 14, 15 (H2/H3), 20 (H1)
  FontItalic = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Italic.ttf", 14);
  if (!FontItalic) {
      FontItalic = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Italic.ttf", 14);
  }
  FontItalicH1 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Italic.ttf", 20);
  if (!FontItalicH1) {
      FontItalicH1 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Italic.ttf", 20);
  }
  FontItalicH2 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Italic.ttf", 15);
  if (!FontItalicH2) {
      FontItalicH2 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Italic.ttf", 15);
  }

  // 3b. Bold-Italic Font - Roboto-BoldItalic 14px (compressed memory font)
  ImFontConfig biConfig;
  strncpy(biConfig.Name, "Roboto-BoldItalic-700-14px*", 40);
  FontBoldItalic = ImgWindow::sFontAtlas->AddFontFromMemoryCompressedTTF(
      gf_roboto_bold_italic_compressed_data,
      gf_roboto_bold_italic_compressed_size,
      14.0f, &biConfig);

  // 4. Monospace Font - Prefer RobotoMono-Regular, fallback to B612Mono / Menlo / DejaVuSansMono
  FontMono = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/RobotoMono-Regular.ttf", 14);
  if (!FontMono) {
      FontMono = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/RobotoMono-Regular.ttf", 14);
  }
  if (!FontMono) {
      FontMono = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/B612Mono-Regular.ttf", 14);
  }
  if (!FontMono) {
      FontMono = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Menlo-Regular.ttf", 14);
  }
  if (!FontMono) {
      FontMono = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/DejaVuSansMono.ttf", 14);
  }
  if (!FontMono) {
      FontMono = Font7;
  }

  // 4. Secondary demonstration fonts for Bill's font picker tree node
  Font4 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Light.ttf", 13);
  Font5 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Light.ttf", 15);
  Font6 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Light.ttf", 20);

  // Remap Font1..Font3 to RobotoMono (Froggy / ProFont completely purged!)
  Font1 = FontMono;
  Font2 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/RobotoMono-Regular.ttf", 16);
  if (!Font2) Font2 = FontMono;
  Font3 = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/RobotoMono-Regular.ttf", 20);
  if (!Font3) Font3 = Font9;

  // Load dedicated typography fonts for Small (12px) and Tiny (10px)
  FontSmall = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Regular.ttf", 12);
  if (!FontSmall) {
      FontSmall = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 12);
  }
  FontTiny = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/plugins/imgui4xp/fonts/Roboto-Regular.ttf", 10);
  if (!FontTiny) {
      FontTiny = ImgWindow::sFontAtlas->AddFontFromFileTTF("./Resources/fonts/Roboto-Regular.ttf", 10);
  }

  // Configure default Markdown font associations & link callback
  static ImGui::MarkdownConfig s_MarkdownConfig;
  s_MarkdownConfig.h1Font = Font9;      // Roboto-Bold 20
  s_MarkdownConfig.h2Font = Font8;      // Roboto-Bold 15
  s_MarkdownConfig.h3Font = Font8;      // Roboto-Bold 15
  s_MarkdownConfig.h1ItalicFont = FontItalicH1; // Roboto-Italic 20
  s_MarkdownConfig.h2ItalicFont = FontItalicH2; // Roboto-Italic 15
  s_MarkdownConfig.h3ItalicFont = FontItalicH2; // Roboto-Italic 15
  s_MarkdownConfig.headingItalicFont = FontItalicH2; // Fallback heading italic
  s_MarkdownConfig.boldFont = FontBold; // Roboto-Bold 15
  s_MarkdownConfig.italicFont = FontItalic; // Roboto-Italic 14
  s_MarkdownConfig.boldItalicFont = FontBoldItalic; // Roboto-BoldItalic 14
  s_MarkdownConfig.regularFont = Font7; // Roboto-Regular 14
  s_MarkdownConfig.monoFont = FontMono; // RobotoMono 14
  s_MarkdownConfig.headingFormats[0] = { Font9, true };
  s_MarkdownConfig.headingFormats[1] = { Font8, true };
  s_MarkdownConfig.headingFormats[2] = { Font8, false };
  s_MarkdownConfig.headingFormats[3] = { Font8, false };
    s_MarkdownConfig.setClipboardTextFn = [](const char* text, void*) {
        if (text) {
            ImGui::SetClipboardText(text);
        }
    };
    s_MarkdownConfig.linkCallback = [](ImGui::MarkdownLinkCallbackData data) {
        if (!data.isImage) {
            std::string url(data.link, data.linkLength);
            if (ImGui::GetPlatformIO().Platform_OpenInShellFn) {
                ImGui::GetPlatformIO().Platform_OpenInShellFn(ImGui::GetCurrentContext(), url.c_str());
            }
        }
    };
    ImGui::SetDefaultMarkdownConfig(&s_MarkdownConfig);

    // 5. Semantic Font Role Registry mappings for StyledText & Typography
    ImGui::SetFontRole(ImGui::ImFontRole::Regular, Font7);
    ImGui::SetFontRole(ImGui::ImFontRole::Bold, FontBold);
    ImGui::SetFontRole(ImGui::ImFontRole::BoldLarge, Font9);
    ImGui::SetFontRole(ImGui::ImFontRole::Italic, FontItalic);
    if (FontBoldItalic) ImGui::SetFontRole(ImGui::ImFontRole::BoldItalic, FontBoldItalic);
    ImGui::SetFontRole(ImGui::ImFontRole::Monospace, FontMono);
    ImGui::SetFontRole(ImGui::ImFontRole::Heading1, Font9);
    ImGui::SetFontRole(ImGui::ImFontRole::Heading2, Font8);
    ImGui::SetFontRole(ImGui::ImFontRole::Heading3, Font8);
    ImGui::SetFontRole(ImGui::ImFontRole::Title, Font8);
    if (FontSmall) ImGui::SetFontRole(ImGui::ImFontRole::Small, FontSmall);
    if (FontTiny)  ImGui::SetFontRole(ImGui::ImFontRole::Tiny, FontTiny);

    // 6. Register In-Memory Images (mirroring into Markdown & StyledText registries)
    ImgWindow::RegisterMemoryImage("avionics_beacon", s_AvionicsIconPng, sizeof(s_AvionicsIconPng));
    ImgWindow::RegisterMemoryImage("avionics_pixel", s_AvionicsPixelPng, sizeof(s_AvionicsPixelPng));

    // 7. Custom Style Tags & Template Aliases (RegisterMarkdownStyleTag / RegisterMarkdownTagAlias)
    ImGui::RegisterMarkdownStyleTag("cam", "green bold mono");
    ImGui::RegisterMarkdownTagAlias("tip", "<style gold><b>:star: Tip:</b></style> %s");
    ImGui::RegisterMarkdownTagAlias("hotkey", "**[<style #40c0ff mono>%s</style>]**");
}

// Undo what we did in configureImgWindow()
void cleanupAfterImgWindow()
{
    s_pShowcaseWidget.reset();
    // Clean up our custom loaded image texture
    if (ImguiWidget::image_id) {
        ImgWindow::DeleteTexture(ImguiWidget::image_id);
        ImguiWidget::image_id = (ImTextureID)0;
    }

    // Unregister in-memory test textures & clear semantic font roles
    ImgWindow::UnregisterMemoryImage("avionics_beacon");
    ImgWindow::UnregisterMemoryImage("avionics_pixel");
    ImGui::ClearFontRoles();

#if defined(IMGUI_VERSION_NUM) && (IMGUI_VERSION_NUM >= 19200) /* only on v1.92+ */
    // Immediately sever ImGui's internal reference first to prevent dangling pointer / Use-After-Free races
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui::GetIO().Fonts = NULL;
    }
#endif

    // Destroy the font atlas
    if (ImgWindow::sFontAtlas) {
        ImgWindow::sFontAtlas.reset();
    }

    // Flush the queue to ensure all deferred textures are deleted from VRAM
    ImgWindow::Shutdown();
}

//
// MARK: ImguiWidget (our example implementation of ImguiWindow)
//

// texture number and size of the image we want to show
// (static, because we want to load the image into a texture just once)
ImTextureID  ImguiWidget::image_id = (ImTextureID)0;
ImVec2       ImguiWidget::image_size;

// Counter for the number of windows opened
int      ImguiWidget::num_win = 0;

// Does any text contain the characters in s?
bool ImguiWidget::tableDataTy::contains (const std::string& s) const
{
    // try finding s in all our texts
    for (const std::string& t: {reg, model, typecode, owner} )
    {
        std::string l = t;
        if (toupper(l).find(s) != std::string::npos)
            return true;
    }
    
    // not found
    return false;
}


ImguiWidget::ImguiWidget(int width, int height,
                         XPLMWindowDecoration decoration,
                         XPLMWindowLayer layer) :
    ImguiWidget(WindowGeometry::Centered(width, height), decoration, layer)
{
}

ImguiWidget::ImguiWidget(const WindowGeometry& geometry,
                         XPLMWindowDecoration decoration,
                         XPLMWindowLayer layer) :
    ImguiWidget(geometry.left, geometry.top, geometry.right, geometry.bottom, decoration, layer)
{
}


ImguiWidget::ImguiWidget(int left, int top, int right, int bot,
                         XPLMWindowDecoration decoration,
                         XPLMWindowLayer layer) :
    ImgWindow(left, top, right, bot, decoration, layer, true /* cursors */),
    myWinNum(++num_win)             // assign a unique window number
{
    // Disable reading/writing of "imgui.ini"
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    
    mIsGlassHUD = (decoration == xplm_WindowDecorationSelfDecoratedResizable ||
                   decoration == xplm_WindowDecorationSelfDecorated);
    if (mIsGlassHUD)
    {
        // Modern Avionics Glass HUD: 80% smoked glass backdrop with subtle border
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.17f, mWindowOpacity);
        style.Colors[ImGuiCol_Border]   = ImVec4(0.35f, 0.42f, 0.50f, 0.70f);
        style.WindowBorderSize = 1.0f;
        style.WindowRounding   = 6.0f;
        // Drag area across top header (roughly double text height)
        SetWindowDragArea(0, 0, INT_MAX, int(FONT_SIZE * 2.2f));
    }

    // Create a flight loop id, but don't schedule it yet
    XPLMCreateFlightLoop_t flDef = {
        sizeof(flDef),                              // structSize
        xplm_FlightLoop_Phase_BeforeFlightModel,    // phase
        cbFlightLoop,                               // callbackFunc
        (void*)this,                                // refcon
    };
    flId = XPLMCreateFlightLoop(&flDef);
    
    // Define our own window title
    SetWindowTitle("imgui4xp Using imgui v" IMGUI_VERSION " for X-Plane 12 by William Good");
    SetWindowResizingLimits(100, 100, 1024, 1024);
    SetVisible(true);
    
    // Initialize the list content
    listContent = {
        "1st line", "2nd line", "3rd line", "4th line", "5th line",
        "6th line", "7th line", "8th line", "9th line", "10th line"
    };
    
    // if not yet loaded: try loading an image for display
    if (!image_id)
        image_id = try2load_image(IMAGE_NAME, image_size);
    
    // copy initial table example data, init with random heading
    tableList = TABLE_CONTENT;
    for (tableDataTy& td: tableList)
        td.heading = float(std::rand() % 3600) / 10.0f;
}

ImguiWidget::~ImguiWidget()
{
    if (flId)
        XPLMDestroyFlightLoop(flId);
}

void ImguiWidget::buildInterface() {

    float win_width = ImGui::GetWindowWidth();
    float win_height = ImGui::GetWindowHeight();

    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::TextUnformatted("Hello, World!");
    
    // Modern Avionics Glass Top Grip Handle
    if (HasWindowDragArea()) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        float avail_w = ImGui::GetWindowWidth();
        float center_x = win_pos.x + avail_w * 0.5f;
        float grip_w = 48.0f;
        float grip_y = win_pos.y + 6.0f;
        draw_list->AddRectFilled(ImVec2(center_x - grip_w * 0.5f, grip_y),
                                 ImVec2(center_x + grip_w * 0.5f, grip_y + 4.0f),
                                 IM_COL32(255, 255, 255, 100), 2.0f);
    }
    
    // Button with fixed width 30 and standard height
    // to pop out the window in an OS window, plus close button for custom-decorated windows
    static float btnWidth = ImGui::CalcTextSize(ICON_FA_WINDOW_MAXIMIZE).x + 5;
    const bool bBtnPopOut = !IsPoppedOut();
    const bool bBtnPopIn  = IsPoppedOut() || IsInVR();
    const bool bBtnVR     = vr_is_enabled && !IsInVR();
    const bool bBtnClose  = IsSelfDecorated();
    int numBtn = bBtnPopOut + bBtnPopIn + bBtnVR + bBtnClose;
    if (numBtn > 0) {
        // Setup colors for window sizing buttons
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_ScrollbarGrabActive)); // dark gray
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);                           // transparent
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_ScrollbarGrab)); // lighter gray

        if (bBtnClose) {
            // Right-aligned close button for frameless custom-decorated windows
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_WINDOW_CLOSE, "Close window"))
                SetVisible(false);
            --numBtn;
        }
        if (bBtnVR) {
            // Same line, but right-aligned
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_EXTERNAL_LINK_SQUARE_ALT, "Move into VR"))
                nextWinPosMode = xplm_WindowVR;
            --numBtn;
        }
        if (bBtnPopIn) {
            // Same line, but right-aligned
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_WINDOW_RESTORE, "Move back into X-Plane"))
                nextWinPosMode = xplm_WindowPositionFree;
            --numBtn;
        }
        if (bBtnPopOut) {
            // Same line, but right-aligned
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_WINDOW_MAXIMIZE, "Pop out into separate window"))
                nextWinPosMode = xplm_WindowPopOut;
            --numBtn;
        }

        // Restore colors
        ImGui::PopStyleColor(3);

        // Window mode should be set outside drawing calls to avoid crashes
        if (nextWinPosMode >= 0)
            XPLMScheduleFlightLoop(flId, -1.0, 1);
    }

    // Grouping a few lines...
    ImGui::BeginGroup();
    ImGui::Text("Window size: width = %.0f  height = %.0f", win_width, win_height);
    
    ImGui::TextUnformatted("Two Widgets");
    ImGui::SameLine();
    ImGui::TextUnformatted("One Line.");
    ImGui::EndGroup();
    
    // ...to put them side-by-side with another group of lines
    ImGui::SameLine(0.0f, 20.0f);
    ImGui::BeginGroup();
    ImGui::Text("Window #%d", myWinNum);
    ImGui::Text("X-Plane Window Id: %p", GetWindowId());
    ImGui::EndGroup();

    // Modern Backdrop Opacity & Window Mode Controller
    ImGui::Spacing();
    ImGui::Separator();
    if (IsSelfDecorated()) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextMD("**Glass Opacity:**");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(140.0f);
        if (ImGui::PercentSliderWithReset("##BackdropOpacity", &mWindowOpacity, 0.80f, 0.10f, 1.00f, "%.0f%%")) {
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = std::min(1.0f, mWindowOpacity * 0.8f + 0.2f);
        }
        char opac_tip[256];
        std::snprintf(opac_tip, sizeof(opac_tip),
            "**Continuous Backdrop Opacity**:\n"
            "Drag or mouse-wheel to seamlessly tune glass transparency over the 3D cockpit.\n"
            "Current: **%.0f%%** | Click [Reset] to restore factory 80%% glass.",
            mWindowOpacity * 100.0f);
        ImGui::TimedTooltip::MD::Text("%s", opac_tip);

        ImGui::SameLine(0.0f, 10.0f);
        if (ImGui::SmallButton("100% Solid")) {
            mWindowOpacity = 1.0f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 1.0f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "100% Opaque (max contrast for high-glare daytime flight).");

        ImGui::SameLine();
        if (ImGui::SmallButton("80% Glass")) {
            mWindowOpacity = 0.80f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.70f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "80% Smoked Glass (ideal balance of contrast & runway visibility).");

        ImGui::SameLine();
        if (ImGui::SmallButton("45% HUD")) {
            mWindowOpacity = 0.45f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.45f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "45% Semi-Transparent HUD overlay.");

        ImGui::SameLine();
        if (ImGui::SmallButton("15% Ghost")) {
            mWindowOpacity = 0.15f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.25f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "15% Ultra-light see-through HUD mode.");

        ImGui::SameLine(0.0f, 14.0f);
        if (ImGui::SmallButton("Switch to Native Bezel...")) {
            requestSwapDecoration = true;
            XPLMScheduleFlightLoop(flId, -1.0, 1);
        }
        ImGui::TimedTooltip::MD::Text("%s",
            "**X-Plane Native Bezel**:\n"
            "Re-creates window with standard X-Plane rounded rectangle bezel.");
    } else {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Backdrop: X-Plane Native Bezel (Solid Maps Style)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Switch to Modern Glass HUD Mode...")) {
            requestSwapDecoration = true;
            XPLMScheduleFlightLoop(flId, -1.0, 1);
        }
        ImGui::TimedTooltip::MD::Text("%s",
            "**Modern Cockpit Glass Mode** (SelfDecoratedResizable):\n"
            "Converts window to a frameless, resizable glass HUD panel with continuous opacity controls.");
    }
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::TreeNode("ImGee UX & Modern Cockpit Suite")) {
        ImGui::TextMD("# ImGee UX Showcase");
        ImGui::TextMD("Turnkey ImGee extensions from `ImgWindow/contrib/` (`#include \"ImgWindow/contrib.h\"`) for modern cockpit plugins:");
        
        ImGui::Bullet();
        ImGui::TextMD("**Semantic Markdown:** `# H1`, `## H2`, `**bold**`, `*italic*`, and `monospace`.");
        
        ImGui::Bullet();
        ImGui::TextMD("**Interactive Tags:** <pill color=#2ecc71>ONLINE</pill>  <pill color=#e74c3c>WARNING</pill>  <badge>v2.3.0</badge>  <kbd>Alt</kbd>+<kbd>Click</kbd>");
        
        ImGui::Bullet();
        ImGui::TextMD("**Stationary TimedTooltips:** Polite, zero-jitter hover tooltips with dwell timeouts.");
        
        ImGui::Bullet();
        ImGui::TextMD("**Avionics Controls:** Animated toggle switches, mouse-wheel sliders with 1-click restore [Reset].");

        ImGui::Spacing();
        static bool s_DemoToggle = true;
        // Korry 389 Split-Lens Pushbutton: active glow vs dark smoked glass stencil
        ImGui::ToggleButton("AP Master Switch", &s_DemoToggle,
                            "AP Master: <pill color=#e67e22>DISARMED</pill> <pill color=#1a252f fg=#555555>ENGAGED</pill>",
                            "AP Master: <pill color=#1a252f fg=#555555>DISARMED</pill> <pill color=#2ecc71>ENGAGED</pill>");

        ImGui::SameLine();
        ImGui::ExternalTextLink("Official ImgWindow Docs", "https://github.com/slgoldberg/ImgWindow");
        ImGui::TimedTooltip::MD::Text("**External Documentation**:\nOpens https://github.com/slgoldberg/ImgWindow in your default web browser.");

        ImGui::Spacing();
        if (ImGui::Button("Open ImGee Cockpit Showcase Window...")) {
            ShowImGeeShowcaseWindow();
        }
        ImGui::TimedTooltip::MD::Text("Opens the dedicated, non-scrollable ImGee showcase window featuring avionics knobs, procedural dials, and live scratchpad.");

        ImGui::TreePop();
    }

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

        ImGui::Separator();
        ImGui::TextMD("### Markdown & Styling Tags (ImGee)");
        ImGui::TextMD("Direct color tag: <color #2b65ec>Ocean Blue via `<color #2b65ec>` tag</color>");
        ImGui::TextMD("Status pills: <pill color=#2ecc71>ONLINE</pill>  <pill color=#e74c3c>FAIL</pill>  <pill color=#f39c12>ARMED</pill>");
        ImGui::TextMD("Keybinding badges: <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>F12</kbd>");
        ImGui::TextMD("<highlight>Highlight tags replace manual PushStyleColor / PopStyleColor boilerplate!</highlight>");

        ImGui::Separator();
        ImGui::TextMD("### Inline Sized Text & Semantic Helpers (New in Phase 3)");
        ImGui::StyledText("StyledText: <tiny color=#888>Tiny 10px</tiny> | <small color=#3498db>Small 12px</small> | <regular>Regular 14px</regular> | <big color=#f39c12>Big 16px</big> | <huge color=#2ecc71>Huge 20px</huge>");
        ImGui::TextMD("Markdown: <tiny color=#888>Tiny Tag</tiny> | <small color=#3498db>Small Tag</small> | <regular>Regular</regular> | <large color=#e67e22>Large Tag</large> | <huge color=#e74c3c>Huge Tag</huge>");
        ImGui::AlignTextToFramePadding();
        ImGui::Text("C++ Helpers:");
        ImGui::SameLine();
        ImGui::TinyText("<tiny> 10px");
        ImGui::SameLine();
        ImGui::SmallText("<small> 12px");
        ImGui::SameLine();
        ImGui::TitleText("<title> 16px");
        ImGui::SameLine();
        ImGui::MonoText("<mono> 14px");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Buttons")) {
        if (ImGui::Button("Push Me")) {
            makeRed = !makeRed;
        }
        ImGui::TimedTooltip::MD::Text("Toggles between **red** and **green** text colors.");

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
        ImGui::TimedTooltip::MD::Text("Activates while mouse button is **held down**.");
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

        ImGui::Separator();
        ImGui::TextMD("#### Modern Cockpit Switch (ImGee):");
        ImGui::ToggleButton("Master Caution Switch", &makeRed);
        ImGui::TimedTooltip::MD::Text("Animated cockpit rocker switch with automatic dual-state tooltip.");

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sliders")) {
        ImGui::SliderFloat("Slider [0..1000]", &sliderVal1, 0, 1000, "Value %.2f");
        ImGui::SliderFloat("Power Slider [0..100000]", &sliderVal2, 0, 100000, "Value %.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderInt("Int Slider [0..100]", &sliderVal3, 0, 100, "Value %d");
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
        ImGui::DragFloat("Drag Float", &g_dragVal1, 1.0f, 0.0f, 1000.0f, "%.2f");
        ImGui::DragInt("Drag Int", &g_dragVal2, 1.0, 0, 1000, "%d");
        ImGui::TextUnformatted("Note: These values are global and synched between windows.");
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
        static char text2[1024 * 16] =
            "/*\n"
            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
            " the hexadecimal encoding of one offending instruction,\n"
            " more formally, the invalid operand with locked CMPXCHG8B\n"
            " instruction bug, is a design flaw in the majority of\n"
            " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
            " processors (all in the P5 microarchitecture).\n"
            "*/\n\n"
            "label:\n"
            "\tlock cmpxchg8b eax\n";
        static char text3[1024] = "";
        ImGui::InputText ("One Line Text", &userText);
        ImGui::InputTextMultiline ("Multiline Text", text2, IM_ARRAYSIZE(text2));
        ImGui::InputTextWithHint ("Text With a Hint", "hint", text3, IM_ARRAYSIZE(text3));
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
        ImGui::Text("image_id = %llu", (unsigned long long)image_id);
        // Draw a previously loaded image
        if (image_id) {
            ImGui::Image(image_id, image_size);
        }

    ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Misc")) {
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

    // Tables are brand-new to ImGui, as of 1.74 it is alpha status
    if (ImGui::TreeNode("Table")) {
        
        // -- Filter by text search --
        
        static char sFilter[100];
        // Note: "##SearchText" creates a unique id, but due to ## it doesn't create a label"
        if (ImGui::InputTextWithHint("##SearchText", ICON_FA_SEARCH " Search", sFilter, IM_ARRAYSIZE(sFilter),
                             ImGuiInputTextFlags_CharsUppercase |
                             ImGuiInputTextFlags_CharsNoBlank))
        {
            // determine if an item is to be shown
            for (tableDataTy& td: tableList) {
                if (sFilter[0])         // some filter defined
                    td.filtered = td.contains(sFilter);
                else                    // no filter defined -> display all
                    td.filtered = true;
            }
        }
        
        // -- The table --
        
        // Alternating rows slighly grayish
        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(0x1a, 0x1a, 0x1a, 0xff));
        // A table with pretty flexible columns, first column frozen, fully scrollable
        if (ImGui::BeginTable("Table", 7,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
                              ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable |
                              ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY))
        {
            // Prepare our data: We fake some movement by turning the planes (1° per second)
            const ImGuiIO& io = ImGui::GetIO();
            for (tableDataTy& td: tableList) {
                if (td.turnsLeft) {
                    if ((td.heading -= io.DeltaTime) <    0.0f) td.heading += 360.0;
                } else {
                    if ((td.heading += io.DeltaTime) >= 360.0f) td.heading -= 360.0;
                }
            }
            
            // Set up the columns of the table
            ImGui::TableSetupColumn("Tail", ImGuiTableColumnFlags_DefaultSort, 60);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 50);
            ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_None, 180);
            ImGui::TableSetupColumn("Owner", ImGuiTableColumnFlags_None, 200);
            ImGui::TableSetupColumn("Heading", ImGuiTableColumnFlags_None, 110);
            ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_None, 30);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_NoSort, 150);
            ImGui::TableSetupScrollFreeze(1,1);
            ImGui::TableHeadersRow();
            
            // Sort the data if and as needed
            ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
            if (sortSpecs && sortSpecs->SpecsDirty &&
                sortSpecs->Specs && sortSpecs->SpecsCount >= 1 &&
                tableList.size() > 1)
            {
                // We sort only by one column, no multi-column sort yet
                const ImGuiTableColumnSortSpecs& colSpec = *(sortSpecs->Specs);
                // We directly sort the tableList
                std::sort(tableList.begin(), tableList.end(),
                          [colSpec](const tableDataTy& a, const tableDataTy& b)
                {
                    int cmp =       // less than 0 if a < b
                    colSpec.ColumnIndex == 0 ? a.reg.compare(b.reg)             :
                    colSpec.ColumnIndex == 1 ? a.typecode.compare(b.typecode)   :
                    colSpec.ColumnIndex == 2 ? a.model.compare(b.model)         :
                    colSpec.ColumnIndex == 3 ? a.owner.compare(b.owner)         :
                    colSpec.ColumnIndex == 4 ? int(a.heading - b.heading)       :
                    colSpec.ColumnIndex == 5 ? a.turnsLeft - b.turnsLeft        : 0;
                    
                    return colSpec.SortDirection == ImGuiSortDirection_Ascending ?
                    cmp < 0 : cmp > 0;
                });
                sortSpecs->SpecsDirty = false;
            }

            // Here we remember which row to delete if any
            tableDataListTy::iterator delIter = tableList.end();

            // Add rows to the table
            for (auto iter = tableList.begin(); iter != tableList.end(); ++iter)
            {
                tableDataTy& td = *iter;
                
                // Skip rows which are not currently filtered
                if (!td.filtered) continue;
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(td.reg.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(td.typecode.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(td.model.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(td.owner.c_str());
                ImGui::TableNextColumn();
                // Heading: left = red / right = green
                ImGui::TextColored(td.turnsLeft ? ImColor(255, 0, 0) : ImColor(0, 255, 0),
                                   "%03.0f", td.heading);
                // Checkbox
                ImGui::TableNextColumn();
                ImGui::PushID_formatted("Left_%p", (void*)&td); // action widget require a unique id per table row (otherwise only the first line's widget work)
                ImGui::Checkbox("", &td.turnsLeft);
                ImGui::PopID();
                
                // Actions: A few buttons
                ImGui::TableNextColumn();

                ImGui::PushID_formatted("N_%p", (void*)&td);   // North
                if (ImGui::ArrowButton("", ImGuiDir_Up))
                    td.heading = 0.0f;
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::PushID_formatted("E_%p", (void*)&td);   // East
                if (ImGui::ArrowButton("", ImGuiDir_Right))
                    td.heading = 90.0f;
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::PushID_formatted("S_%p", (void*)&td);   // South
                if (ImGui::ArrowButton("", ImGuiDir_Down))
                    td.heading = 180.0f;
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::PushID_formatted("W_%p", (void*)&td);   // West
                if (ImGui::ArrowButton("", ImGuiDir_Left))
                    td.heading = 270.0f;
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::PushID_formatted("Del_%p", (void*)&td);
                if (ImGui::ButtonTooltip(ICON_FA_TRASH_ALT, "Delete row"))
                    // remember the row to delete, but don't delete right now
                    delIter = iter;
                ImGui::PopID();
            }
            
            // Now only delete a row if requested to do so
            if (delIter != tableList.end())
                tableList.erase(delIter);

            // -- Add a row to enter new data
            static char sTail[10] = "", sType[5] = "", sModel[100] = "", sOwner[100] = "";
            static int iHead = 0;
            static bool bLeft = false;
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::InputTextWithHint("##New_Tail", "Tail", sTail, IM_ARRAYSIZE(sTail));

            ImGui::TableNextColumn();
            ImGui::InputTextWithHint("##New_Type", "Type", sType, IM_ARRAYSIZE(sType));

            ImGui::TableNextColumn();
            ImGui::InputTextWithHint("##New_Model", "Model", sModel, IM_ARRAYSIZE(sModel));

            ImGui::TableNextColumn();
            ImGui::InputTextWithHint("##New_Owner", "Owner", sOwner, IM_ARRAYSIZE(sOwner));

            ImGui::TableNextColumn();
            ImGui::InputInt("##New_Heading", &iHead);

            ImGui::TableNextColumn();
            ImGui::Checkbox("##New_TurnLeft", &bLeft);

            ImGui::TableNextColumn();
            // something entered for all texts?
            if (sTail[0] && sType[0] && sModel[0] && sOwner[0]) {
                ImGui::PushID("New_Add");
                if (ImGui::SmallButton("Add")) {
                    tableList.emplace_back(tableDataTy{
                        sTail, sModel, sType, sOwner, float(iHead), bLeft
                    });
                    // init our static text for a new entry
                    sTail[0] = '\0';
                    sType[0] = '\0';
                    sModel[0] = '\0';
                    sOwner[0] = '\0';
                    iHead = 0;
                    bLeft = false;
                }
                ImGui::PopID();
            } else {
                // There is nothing like a disabled button...
                ImGui::TextDisabled("[Add]");
            }
            
            // End of table
            ImGui::EndTable();

            
        }
        ImGui::PopStyleColor();
        
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Fonts Scaling")) {

        ImGui::Text("Default Roboto-Regular.ttf %.1f font \n", FONT_SIZE);

        ImGui::TextUnformatted("");
        // Green color
        ImVec4 col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font");
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(NULL, style.FontSizeBase * 0.5f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 0.5");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.625);
        ImGui::PushFont(NULL, style.FontSizeBase * 0.625f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 0.625");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.75);
        ImGui::PushFont(NULL, style.FontSizeBase * 0.75f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 0.75");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.875);
        ImGui::PushFont(NULL, style.FontSizeBase * 0.875f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 0.875");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();


        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 1.0");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(1.125);
        ImGui::PushFont(NULL, style.FontSizeBase * 1.125f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 1.125");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(1.25);
        ImGui::PushFont(NULL, style.FontSizeBase * 1.125f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 1.25");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(1.375);
        ImGui::PushFont(NULL, style.FontSizeBase * 1.375f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 1.375");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(1.5);
        ImGui::PushFont(NULL, style.FontSizeBase * 1.5f);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular 14 font with font scale of 1.5");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");

        ImGui::Text("Using ShowStyleEditor() to see if new fonts have loaded\n");
        ImGui::ShowStyleEditor();
        ImGui::PopFont();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Fonts different fonts with different sizes and colors")) {

        // Green color
        ImVec4 col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font1, Font1->LegacySize);
        ImGui::TextUnformatted("Some Green Text with RobotoMono size 14");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Bright Teal color
        col = ImColor(0, 255, 255, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font2, Font2->LegacySize);
        ImGui::TextUnformatted("Some Bright Teal Text with RobotoMono size 16");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Red color
        col = ImColor(255, 0, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font3, Font3->LegacySize);
        ImGui::TextUnformatted("Some Red Text with RobotoMono size 20");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();


        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font4, Font4->LegacySize);
        ImGui::TextUnformatted("Some Green Text with Roboto-Light size 13");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Bright Teal color
        col = ImColor(0, 255, 255, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font5, Font5->LegacySize);
        ImGui::TextUnformatted("Some Bright Teal Text with Roboto-Light size 15");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Red color
        col = ImColor(255, 0, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font6, Font6->LegacySize);
        ImGui::TextUnformatted("Some Red Text with Roboto-Light size 20");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();



        // Green color
        col = ImColor(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font7, Font7->LegacySize);
        ImGui::TextUnformatted("Some Green Text with Roboto-Regular size 13");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Bright Teal color
        col = ImColor(0, 255, 255, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font8, Font8->LegacySize);
        ImGui::TextUnformatted("Some Bright Teal Text with Roboto-Regular size 15");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();

        // Red color
        col = ImColor(255, 0, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        // ImGui::SetWindowFontScale(0.5);
        ImGui::PushFont(Font9, Font9->LegacySize);
        ImGui::TextUnformatted("Some Red Text with Roboto-Regular size 20");
        // ImGui::SetWindowFontScale(1.0);
        ImGui::PopFont();
        ImGui::PushFont(NULL, style.FontSizeBase * 1.0f);
        ImGui::PopStyleColor();
        ImGui::TextUnformatted("");
        ImGui::PopFont();


        ImGui::TreePop();
    }

    if (ImGui::TreeNode("show imgui demo window")) {

        ImGui::ShowDemoWindow();
        ImGui::TreePop();
    }
}

// Outside all rendering we can change things like window mode
float ImguiWidget::cbFlightLoop(float, float, int, void* inRefcon)
{
    // refcon is pointer to ImguiWidget
    ImguiWidget& wnd = *reinterpret_cast<ImguiWidget*>(inRefcon);

    // Has user requested decoration swap (Solid <-> Modern Glass HUD)?
    if (wnd.requestSwapDecoration) {
        wnd.requestSwapDecoration = false;
        int left, top, right, bottom;
        wnd.GetCurrentWindowGeometry(left, top, right, bottom);
        bool is_vis = wnd.GetVisible();
        XPLMWindowDecoration cur_dec = wnd.GetWindowDecoration();
        XPLMWindowDecoration new_dec = (cur_dec == xplm_WindowDecorationRoundRectangle)
            ? xplm_WindowDecorationSelfDecoratedResizable
            : xplm_WindowDecorationRoundRectangle;

        auto new_wnd = std::make_shared<ImguiWidget>(left, top, right, bottom, new_dec, xplm_WindowLayerFloatingWindows);
        new_wnd->mWindowOpacity = wnd.mWindowOpacity;

        // Hide and retire the old window immediately so it stops rendering underneath!
        wnd.SetVisible(false);

        // Keep old window alive in static graveyard to prevent destruction while inside its own callback
        static ImgWindowSPtrTy s_DeferredGraveyard = nullptr;
        s_DeferredGraveyard = nullptr;

        for (auto& p : gWndList) {
            if (p.get() == &wnd) {
                s_DeferredGraveyard = p;
                p = new_wnd;
                break;
            }
        }
        new_wnd->SetVisible(is_vis);
        return 0.0f;
    }

    // Has user requested a change in window mode?
    if (wnd.nextWinPosMode >= 0) {
        wnd.SetWindowPositioningMode(wnd.nextWinPosMode);
        // If we pop in, then we need to explicitely set a position for the window to appear
        if (wnd.nextWinPosMode == xplm_WindowPositionFree) {
            int left, top, right, bottom;
            wnd.GetCurrentWindowGeometry(left, top, right, bottom);
            // Normalize to our starting position (WIN_PAD|WIN_PAD), but keep size unchanged
            const int width  = right-left;
            const int height = top-bottom;
            CalcWinCoords(left, top, right, bottom);
            right  = left + width;
            bottom = top - height;
            wnd.SetWindowGeometry(left, top, right, bottom);
        }
        wnd.nextWinPosMode = -1;
    }
    
    // don't call me again
    return 0.0f;
}


// MARK: - ImGeeShowcaseWidget Implementation

// Static File Helpers for Live Scratchpad
static bool LoadTextFile(const std::string& filepath, std::string& out_text) {
    FILE* f = fopen(filepath.c_str(), "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) { fclose(f); return false; }
    out_text.resize(sz);
    if (sz > 0) {
        size_t read_bytes = fread(&out_text[0], 1, sz, f);
        out_text.resize(read_bytes);
    }
    fclose(f);
    return true;
}

static bool SaveTextFile(const std::string& filepath, const std::string& text) {
    FILE* f = fopen(filepath.c_str(), "wb");
    if (!f) return false;
    if (!text.empty()) {
        fwrite(text.data(), 1, text.size(), f);
    }
    fclose(f);
    return true;
}

static std::string FormatShieldsIoUrl(const std::string& label, const std::string& message, const std::string& color) {
    auto sanitize = [](const std::string& s) {
        std::string res;
        for (char c : s) {
            if (c == '-') res += "--";
            else if (c == '_') res += "__";
            else if (c == ' ') res += "_";
            else res += c;
        }
        return res;
    };
    std::string l = sanitize(label);
    std::string m = sanitize(message);
    if (l.empty()) {
        return "https://img.shields.io/badge/" + m + "-" + color + ".png";
    }
    return "https://img.shields.io/badge/" + l + "-" + m + "-" + color + ".png";
}

void ImGeeShowcaseWidget::LoadNote(const std::string& path) {
    if (LoadTextFile(path, scratchpadText)) {
        size_t slash = path.find_last_of("/\\");
        currentNoteFilename = (slash != std::string::npos) ? path.substr(slash + 1) : path;
        noteStatusMsg = "Loaded " + currentNoteFilename;
        noteStatusTimer = 3.0f;
    } else {
        noteStatusMsg = "Failed to load note";
        noteStatusTimer = 3.0f;
    }
}

void ImGeeShowcaseWidget::SaveNote(const std::string& path) {
    if (SaveTextFile(path, scratchpadText)) {
        size_t slash = path.find_last_of("/\\");
        currentNoteFilename = (slash != std::string::npos) ? path.substr(slash + 1) : path;
        noteStatusMsg = "Saved " + currentNoteFilename;
        noteStatusTimer = 3.0f;
    } else {
        noteStatusMsg = "Failed to save note";
        noteStatusTimer = 3.0f;
    }
}

ImGeeShowcaseWidget::ImGeeShowcaseWidget(int width, int height,
                                         XPLMWindowDecoration decoration,
                                         XPLMWindowLayer layer) :
    ImgWindow(WindowGeometry::Centered(width, height), decoration, layer, true /* cursors */)
{
    mIsGlassHUD = (decoration == xplm_WindowDecorationSelfDecoratedResizable ||
                   decoration == xplm_WindowDecorationSelfDecorated);
    if (mIsGlassHUD)
    {
        // Modern Avionics Glass HUD: 80% smoked glass backdrop with subtle border
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.17f, mWindowOpacity);
        style.Colors[ImGuiCol_Border]   = ImVec4(0.35f, 0.42f, 0.50f, 0.70f);
        style.WindowBorderSize = 1.0f;
        style.WindowRounding   = 6.0f;
        // Drag area across top header (roughly double text height)
        SetWindowDragArea(0, 0, INT_MAX, int(FONT_SIZE * 2.2f));
    }

    // Create flight loop for dynamic decoration swap
    XPLMCreateFlightLoop_t flDef = {
        sizeof(flDef),
        xplm_FlightLoop_Phase_BeforeFlightModel,
        cbFlightLoop,
        (void*)this,
    };
    flId = XPLMCreateFlightLoop(&flDef);

    SetWindowTitle("ImGee Cockpit Showcase");
    SetWindowResizingLimits(565, 330, 2560, 1440);
    SetUnscaledChrome(0, 68);
    // Pre-load default note from disk if available
    std::string default_note = "./Resources/plugins/imgui4xp/notes/flight_briefing.md";
    if (!LoadTextFile(default_note, scratchpadText)) {
        scratchpadText = "### Live Flight Briefing\n"
                         "* Status: <pill color=#2ecc71>CLEARED</pill>\n"
                         "* Departure: **KSEA** Rwy 16L\n"
                         "* Route: `SEA8 JAWBN Q29`\n"
                         "* Clearance: <big color=#3498db>FL180</big> <tiny color=#888888>SUB-BUS 2A</tiny> <small color=#27ae60>14.2V</small> <huge color=#e74c3c>ALERT</huge>\n"
                         "---\n"
                         "| Waypoint | Target Alt | Fuel | ETA |\n"
                         "| :--- | :---: | ---: | ---: |\n"
                         "| **SEA8** | 3,000 ft | 98% | 18:05z |\n"
                         "| **JAWBN** | 12,000 ft | 92% | 18:18z |\n"
                         "| **Q29** | FL240 | 85% | 18:34z |\n"
                         "---\n"
                         "```cpp\n"
                         "void ExecuteClimb() {\n"
                         "    Autopilot::SetTargetAlt(14000);\n"
                         "}\n"
                         "```";
    }
    // Latch initial window geometry as 100% baseline snapshot
    GetCurrentWindowGeometry(_baselineRect.l, _baselineRect.t, _baselineRect.r, _baselineRect.b);
    _baselineRect.valid = true;
}

ImGeeShowcaseWidget::~ImGeeShowcaseWidget()
{
    if (flId) {
        XPLMDestroyFlightLoop(flId);
        flId = nullptr;
    }
}

float ImGeeShowcaseWidget::cbFlightLoop(float, float, int, void* inRefcon)
{
    ImGeeShowcaseWidget& wnd = *reinterpret_cast<ImGeeShowcaseWidget*>(inRefcon);

    if (wnd.requestSwapDecoration) {
        wnd.requestSwapDecoration = false;
        int left, top, right, bottom;
        wnd.GetCurrentWindowGeometry(left, top, right, bottom);
        bool is_vis = wnd.GetVisible();
        XPLMWindowDecoration cur_dec = wnd.GetWindowDecoration();
        XPLMWindowDecoration new_dec = (cur_dec == xplm_WindowDecorationRoundRectangle)
            ? xplm_WindowDecorationSelfDecoratedResizable
            : xplm_WindowDecorationRoundRectangle;

        int width = std::max(565, right - left);
        int height = std::max(330, top - bottom);

        auto new_wnd = std::make_shared<ImGeeShowcaseWidget>(width, height, new_dec, xplm_WindowLayerFloatingWindows);
        new_wnd->SetWindowGeometry(left, top, right, bottom);
        new_wnd->mWindowOpacity = wnd.mWindowOpacity;
        new_wnd->_fontAdjust = wnd._fontAdjust;
        new_wnd->_lastZoom = wnd._lastZoom;
        new_wnd->_baselineRect = wnd._baselineRect;
        new_wnd->_priorZoomRect = wnd._priorZoomRect;

        // Hide old window immediately so it stops rendering underneath!
        wnd.SetVisible(false);

        // Keep old window alive in static graveyard to prevent destruction while inside callback
        static ImgWindowSPtrTy s_DeferredShowcaseGraveyard = nullptr;
        s_DeferredShowcaseGraveyard = s_pShowcaseWidget;

        for (auto& p : gWndList) {
            if (p.get() == &wnd) {
                p = new_wnd;
                break;
            }
        }
        s_pShowcaseWidget = new_wnd;
        new_wnd->SetVisible(is_vis);
        return 0.0f;
    }
    return 0.0f;
}

void ImGeeShowcaseWidget::RestoreWindowRect(const WindowRectSnapshot& rect)
{
    if (!rect.valid || !GetWindowId()) return;

    if (IsInVR()) {
        int w = std::abs(rect.r - rect.l);
        int h = std::abs(rect.t - rect.b);
        if (w > 0 && h > 0) {
            w = ImClamp(w, 565, 2560);
            h = ImClamp(h, 330, 1440);
            SetWindowGeometryVR(w, h);
        }
        return;
    }

    if (IsPoppedOut()) {
        SetWindowGeometryOS(rect.l, rect.t, rect.r, rect.b);
        return;
    }

    // Standard in-simulator 2D window:
    int l = rect.l;
    int t = rect.t;
    int r = rect.r;
    int b = rect.b;

    ClampGeometryToScreen(l, t, r, b, 565, 330, 2560, 1440, GetEffectiveTopMargin());

    SetWindowGeometry(l, t, r, b);
}

void ShowImGeeShowcaseWindow()
{
    ImGuiContext* prevContext = ImGui::GetCurrentContext();
    if (!s_pShowcaseWidget) {
        s_pShowcaseWidget = std::make_shared<ImGeeShowcaseWidget>(680, 568);
        gWndList.push_back(s_pShowcaseWidget);
    }
    s_pShowcaseWidget->SetVisible(true);
    s_pShowcaseWidget->BringWindowToFront();
    if (prevContext) {
        ImGui::SetCurrentContext(prevContext);
    }
}

void ImGeeShowcaseWidget::buildInterface()
{
    // Modern Avionics Glass Top Grip Handle
    if (HasWindowDragArea()) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        float avail_w = ImGui::GetWindowWidth();
        float center_x = win_pos.x + avail_w * 0.5f;
        float grip_w = 48.0f;
        float grip_y = win_pos.y + 6.0f;
        draw_list->AddRectFilled(ImVec2(center_x - grip_w * 0.5f, grip_y),
                                 ImVec2(center_x + grip_w * 0.5f, grip_y + 4.0f),
                                 IM_COL32(255, 255, 255, 100), 2.0f);
    }

    // Update transient notification timer
    float dt = ImGui::GetIO().DeltaTime;
    if (noteStatusTimer > 0.0f) {
        noteStatusTimer -= dt;
        if (noteStatusTimer <= 0.0f) {
            noteStatusMsg.clear();
        }
    }

    // Initialize default theme (ImGee Charcoal Slate) once on startup
    static bool s_themeInitialized = false;
    static int  s_currentTheme = 0; // 0 = Charcoal (Default), 1 = Rusty, 2 = Stock Dark, 3 = Classic, 4 = Light
    if (!s_themeInitialized) {
        ImGui::ApplyDefaultTheme();
        s_themeInitialized = true;
    }

    // 1. Container / Window Chrome: Baseline 1.0x scale (rock-solid, non-twitching controls)
    ImGui::SetWindowFontScale(1.0f);

    // Title on left
    ImGui::TextMD("# ImGee UX & Modern Cockpit Suite");

    // Steppers + Slider + Reset + Fit + Cursor suite (responsive width in chrome)
    const float title_w = Font9 ? Font9->CalcTextSizeA(Font9->LegacySize, FLT_MAX, -1.0f, "ImGee UX & Modern Cockpit Suite").x : 315.0f;
    const float min_offset = ImGui::GetStyle().WindowPadding.x + title_w + ImGui::GetStyle().ItemSpacing.x;
    const float fixed_controls_w = 139.0f;
    ImGuiWindow* cur_win = ImGui::GetCurrentWindow();
    const float scrollbar_pad = (cur_win && cur_win->ScrollbarY) ? cur_win->ScrollbarSizes.x : 0.0f;
    const float right_margin = ImGui::GetStyle().WindowPadding.x + scrollbar_pad;
    const float avail_for_suite = ImMax(fixed_controls_w + 90.0f, ImGui::GetWindowWidth() - min_offset - right_margin);
    const float slider_w = ImClamp(avail_for_suite - fixed_controls_w, 90.0f, 150.0f);
    const float zoom_suite_w = fixed_controls_w + slider_w;
    const float target_offset = ImGui::GetWindowWidth() - zoom_suite_w - right_margin;
    ImGui::SameLine(ImMax(min_offset, target_offset));

    // [-] Stepper: repeating button with (n % 4 == 0) gating (hold to step smoothly)
    ImGui::PushButtonRepeat(true);
    bool dec_clicked = ImGui::Button("-##zoom_dec", ImVec2(22.0f, 0.0f));
    ImGui::PopButtonRepeat();
    bool dec_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (!dec_clicked && dec_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        dec_clicked = true;
    }

    if (dec_clicked) {
        bool is_initial = ImGui::IsItemActivated() || ImGui::IsMouseClicked(0);
        bool allow_step = false;
        if (is_initial) {
            _decRepeatCount = 0;
            allow_step = true;
        } else {
            _decRepeatCount++;
            if ((_decRepeatCount % 4) == 0) {
                allow_step = true;
            }
        }
        if (allow_step) {
            float old_z = _fontAdjust;
            _fontAdjust = ImMax(0.50f, _fontAdjust - 0.05f);
            if (std::abs(_fontAdjust - old_z) > 0.001f) {
                if (std::abs(_fontAdjust - 1.0f) < 0.001f && _baselineRect.valid) {
                    RestoreWindowRect(_baselineRect);
                } else {
                    ScaleWindowGeometry(old_z, _fontAdjust);
                }
            }
        }
    }
    if (dec_hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), dec_hovered, "Zoom Out (-5%%)\n\n*Click or hold down to step smoothly*");
    }

    ImGui::SameLine(0.0f, 3.0f);

    // Track slider: Range 50% to 200%, 1.67x wider (150px) for spacious text and thumb breathing room
    float pre_slider_zoom = _fontAdjust;
    float pct = _fontAdjust * 100.0f;
    ImGui::SetNextItemWidth(slider_w);
    if (ImGui::SliderFloat("##zoom_slider", &pct, 50.0f, 200.0f, "Zoom %.0f%%")) {
        _fontAdjust = pct / 100.0f;
    }
    if (ImGui::IsItemActivated()) {
        _sliderStartZoom = pre_slider_zoom;
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (std::abs(_fontAdjust - _sliderStartZoom) > 0.001f) {
            if (std::abs(_fontAdjust - 1.0f) < 0.001f && _baselineRect.valid) {
                RestoreWindowRect(_baselineRect);
            } else {
                ScaleWindowGeometry(_sliderStartZoom, _fontAdjust);
            }
            _sliderStartZoom = _fontAdjust;
        }
    }
    // Sensitive mouse wheel (2% per click on macOS instead of chunky 5%+)
    float wheel_old = _fontAdjust;
    if (ImGui::AdjustItemOnMouseWheel(&_fontAdjust, 0.50f, 2.00f, 0.02f)) {
        if (std::abs(_fontAdjust - wheel_old) > 0.001f) {
            if (std::abs(_fontAdjust - 1.0f) < 0.001f && _baselineRect.valid) {
                RestoreWindowRect(_baselineRect);
            } else {
                ScaleWindowGeometry(wheel_old, _fontAdjust);
            }
        }
    }
    ImGui::TimedTooltip::MD::Text("Interactive UI Zoom: roll mouse wheel (2%%) or drag thumb.\n\n*Window resizes proportionally to match.*");

    ImGui::SameLine(0.0f, 3.0f);

    // [+] Stepper: repeating button with (n % 4 == 0) gating (hold to step smoothly)
    ImGui::PushButtonRepeat(true);
    bool inc_clicked = ImGui::Button("+##zoom_inc", ImVec2(22.0f, 0.0f));
    ImGui::PopButtonRepeat();
    bool inc_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (!inc_clicked && inc_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        inc_clicked = true;
    }

    if (inc_clicked) {
        bool is_initial = ImGui::IsItemActivated() || ImGui::IsMouseClicked(0);
        bool allow_step = false;
        if (is_initial) {
            _incRepeatCount = 0;
            allow_step = true;
        } else {
            _incRepeatCount++;
            if ((_incRepeatCount % 4) == 0) {
                allow_step = true;
            }
        }
        if (allow_step) {
            float old_z = _fontAdjust;
            _fontAdjust = ImMin(2.00f, _fontAdjust + 0.05f);
            if (std::abs(_fontAdjust - old_z) > 0.001f) {
                if (std::abs(_fontAdjust - 1.0f) < 0.001f && _baselineRect.valid) {
                    RestoreWindowRect(_baselineRect);
                } else {
                    ScaleWindowGeometry(old_z, _fontAdjust);
                }
            }
        }
    }
    if (inc_hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), inc_hovered, "Zoom In (+5%%)\n\n*Click or hold down to step smoothly*");
    }

    ImGui::SameLine(0.0f, 3.0f);

    // Vector Reset button with 1-click A/B toggle memory to prior zoom and geometry
    bool is_modified = std::abs(_fontAdjust - 1.0f) > 0.001f;
    bool can_restore = !is_modified && std::abs(_lastZoom - 1.0f) > 0.001f;
    bool rst_clicked = ImGui::ResetButton("##zoom_reset", is_modified, 0.0f, false, nullptr, can_restore);
    bool rst_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (!rst_clicked && rst_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        rst_clicked = true;
    }
    if (rst_clicked) {
        float old_z = _fontAdjust;
        if (is_modified) {
            // State A -> Reset to 100% baseline:
            // Snapshot active zoomed geometry so 1-click toggle can restore it with 100% fidelity
            GetCurrentWindowGeometry(_priorZoomRect.l, _priorZoomRect.t, _priorZoomRect.r, _priorZoomRect.b);
            _priorZoomRect.valid = true;
            _lastZoom = _fontAdjust;
            _fontAdjust = 1.0f;
            if (_baselineRect.valid) {
                RestoreWindowRect(_baselineRect);
            } else {
                ScaleWindowGeometry(old_z, 1.0f);
            }
        } else if (can_restore) {
            // State B -> 1-click Toggle back to prior zoom:
            _fontAdjust = _lastZoom;
            if (_priorZoomRect.valid) {
                RestoreWindowRect(_priorZoomRect);
            } else {
                ScaleWindowGeometry(1.0f, _fontAdjust);
            }
        }
    }
    if (rst_hovered) {
        if (is_modified) {
            ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), rst_hovered, "**Reset (↺ 100%%):** Reset zoom from **%d%%** back to **100%% baseline**.", (int)std::round(_fontAdjust * 100.0f));
        } else if (can_restore) {
            ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), rst_hovered, "**Toggle (↺ prior):** 1-click toggle back to prior **%d%% zoom**.", (int)std::round(_lastZoom * 100.0f));
        } else {
            ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), rst_hovered, "**Reset (↺ 100%%):** Zoom is at **100%% baseline**.");
        }
    }

    // Proportional Window Scaling: Fit window dimensions to baseline at current zoom level
    ImGui::SameLine(0.0f, 3.0f);
    if (ToolbarButton("Fit##fit_window", nullptr, ImVec2(32.0f, 0.0f))) {
        ResizeToScale(_fontAdjust);
    }
    bool fit_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (fit_hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), fit_hovered, "**Fit Window to Baseline**\n\nResets window to default 680x568 baseline proportions scaled to current zoom (%d%%) anchored to top-left.", (int)std::round(_fontAdjust * 100.0f));
    }
    // Cursor Handling Toggle Button (separated by 8px gap)
    ImGui::SameLine(0.0f, 8.0f);
    bool imgui_cursors = GetImgCursorHandling();
    if (ToolbarButton(imgui_cursors ? ICON_FA_MOUSE_POINTER "##cursor_toggle" : ICON_FA_MOUSE "##cursor_toggle", nullptr, ImVec2(24.0f, 0.0f))) {
        SetImgCursorHandling(!imgui_cursors);
    }
    bool cur_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    if (cur_hovered) {
        ImGui::TimedTooltip::TextMDProxy(ImGui::GetItemID(), cur_hovered, imgui_cursors
            ? "**ImGui Dynamic Cursors:** <pill color=#2ecc71>ACTIVE</pill>\n\nClick to switch to standard X-Plane hardware cursor."
            : "**ImGui Dynamic Cursors:** <pill color=#7f8c8d>DISABLED</pill>\n\nClick to enable ImGui contextual mouse cursors (hand, resize, text).");
    }

    if (IsSelfDecorated()) {
        ImGui::SameLine(0.0f, 6.0f);
        if (ToolbarButton(ICON_FA_WINDOW_CLOSE "##sc_close", "Close showcase window", ImVec2(24.0f, 0.0f))) {
            SetVisible(false);
        }
    }

    // Modern Backdrop Opacity & Mode Controller
    ImGuiStyle& style = ImGui::GetStyle();
    if (IsSelfDecorated()) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextMD("**Glass Opacity:**");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120.0f);
        if (ImGui::PercentSliderWithReset("##ShowcaseOpacity", &mWindowOpacity, 0.80f, 0.10f, 1.00f, "%.0f%%")) {
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = std::min(1.0f, mWindowOpacity * 0.8f + 0.2f);
        }
        char opac_tip[256];
        std::snprintf(opac_tip, sizeof(opac_tip),
            "**Continuous Backdrop Opacity**:\n"
            "Drag or mouse-wheel to seamlessly tune glass transparency over the 3D cockpit.\n"
            "Current: **%.0f%%** | Click [Reset] to restore factory 80%% glass.",
            mWindowOpacity * 100.0f);
        ImGui::TimedTooltip::MD::Text("%s", opac_tip);

        ImGui::SameLine(0.0f, 8.0f);
        if (ImGui::SmallButton("100% Solid##sc")) {
            mWindowOpacity = 1.0f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 1.0f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "100% Opaque (max contrast for high-glare daytime flight).");

        ImGui::SameLine();
        if (ImGui::SmallButton("80% Glass##sc")) {
            mWindowOpacity = 0.80f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.70f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "80% Smoked Glass (ideal balance of contrast & runway visibility).");

        ImGui::SameLine();
        if (ImGui::SmallButton("45% HUD##sc")) {
            mWindowOpacity = 0.45f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.45f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "45% Semi-Transparent HUD overlay.");

        ImGui::SameLine();
        if (ImGui::SmallButton("15% Ghost##sc")) {
            mWindowOpacity = 0.15f;
            style.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            style.Colors[ImGuiCol_Border].w = 0.25f;
        }
        ImGui::TimedTooltip::MD::Text("%s", "15% Ultra-light see-through HUD mode.");

        ImGui::SameLine(0.0f, 10.0f);
        if (ImGui::SmallButton("Switch to Native Bezel...##sc")) {
            requestSwapDecoration = true;
            XPLMScheduleFlightLoop(flId, -1.0, 1);
        }
        ImGui::TimedTooltip::MD::Text("%s",
            "**X-Plane Native Bezel**:\n"
            "Re-creates showcase window with standard X-Plane rounded rectangle bezel.");
    } else {
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Backdrop: X-Plane Native Bezel (Solid Maps Style)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Switch to Modern Glass HUD Mode...##sc")) {
            requestSwapDecoration = true;
            XPLMScheduleFlightLoop(flId, -1.0, 1);
        }
        ImGui::TimedTooltip::MD::Text("%s",
            "**Modern Cockpit Glass Mode** (SelfDecoratedResizable):\n"
            "Converts showcase window to a frameless, resizable glass HUD panel with continuous opacity controls.");
    }
    ImGui::Spacing();

    // 2. Inner Content Canvas: Apply dynamic _fontAdjust to all tabs and interactive controls
    ImGui::SetWindowFontScale(_fontAdjust);

    if (ImGui::BeginTabBar("ImGeeShowcaseTabBar", ImGuiTabBarFlags_NoTooltip)) {
        bool tab_knobs = ImGui::BeginTabItem("Avionics Knobs & Sliders");
        ImGui::TimedTooltip::MD::Text("Precision autopilot dials, sliders, procedural gauges, and copy button.");
        if (tab_knobs) {
            ImGui::Spacing();
            ImGui::TextMD("#### Precision Knobs with 1-Click Restore");
            ImGui::TextMD("Hover and roll mouse wheel to adjust values. Click **[Reset]** to restore calibrated defaults:");
            ImGui::Spacing();

            ImGui::SliderWithReset("Heading Bug", &headingBug, 180.0f, 0.0f, 360.0f, "%.0f deg", 0, 1.0f, "Click to reset heading to runway track (180 deg)");
            ImGui::TimedTooltip::MD::Text("Autopilot heading select. Wheel steps 1 deg; click button to reset.");

            ImGui::SliderWithReset("Altitude Target", &altitudeTarget, 5500.0f, 0.0f, 45000.0f, "%.0f ft", 0, 100.0f, "Click to reset to cruise altitude (5,500 ft)");
            ImGui::TimedTooltip::MD::Text("Selected altitude. Wheel steps 100 ft; click button to reset.");

            ImGui::SliderWithReset("Vertical Speed", &verticalSpeed, 0.0f, -4000.0f, 4000.0f, "%+.0f fpm", 0, 50.0f, "Click to level off (0 fpm)");
            ImGui::TimedTooltip::MD::Text("VS target rate. Wheel steps 50 fpm; click button to level off.");

            ImGui::Spacing();
            ImGui::PercentSlider("Throttle Lever", &throttlePct, 0.0f, 1.0f);
            ImGui::TimedTooltip::MD::Text("Throttle lever: **%.0f%%** (range 0%% to 100%%).\nRoll mouse wheel (1%% steps) or drag track to adjust.", throttlePct * 100.0f);

            ImGui::Spacing();
            static float s_InstrumentDimmer = 0.85f;
            static float s_RadioVolume = 0.70f;
            ImGui::DimmerKnob("Instrument Backlight", &s_InstrumentDimmer, 0.05f, 1.0f, 1.0f, "%.0f%%");
            ImGui::TimedTooltip::MD::Text("Potentiometer dimmer with **2-step OFF detente**.\nRoll mouse wheel or drag vertically. Scroll past 5%% to click into **OFF**!");

            ImGui::SameLine(0.0f, 20.0f);
            ImGui::KnobWithReset("Radio Volume", &s_RadioVolume, 0.70f, 0.0f, 1.0f, "%.0f%%", 0.0f, 0.02f, "Reset radio volume to 70%");
            ImGui::TimedTooltip::MD::Text("Standard 270 deg avionics rotary knob with vector reset button.");

            ImGui::Spacing();
            static const char* navSources[] = { "NAV1 (VOR/LOC)", "NAV2 (VOR)", "GPS / FMS", "ADF / NDB" };
            static const char* navTips[] = {
                "VHF Omnidirectional Range / Localizer receiver 1",
                "VHF Omnidirectional Range receiver 2",
                "GPS flight management computer navigation source",
                "Automatic Direction Finder receiver"
            };
            ImGui::MouseWheelCombo("Nav Source", &navSourceIdx, navSources, 4, -1, false, navTips);
            ImGui::TimedTooltip::MD::Text("Roll mouse wheel directly over combo to cycle nav sources without opening dropdown.");

            ImGui::Spacing();
            static int s_CamMode = 1;
            static const char* const camItems[] = { "Circle View", "Spot View", "Tower View", "Cockpit View" };
            static const char* const camTips[]  = { "Follow aircraft dynamically", "Fixed distance chase view", "Airport control tower view", "Pilot eye 3D cockpit" };
            ImGui::TextUnformatted("Camera Mode:");
            ImGui::SameLine();
            if (ImGui::SelectPulldown("##CamMode", &s_CamMode, camItems, 4, camTips)) {
                // Camera mode selection changed
            }
            ImGui::TimedTooltip::MD::Text("**SelectPulldown**: Closed mouse-wheel passes through (scrolls parent);\nOpen mouse-wheel cycles options dynamically.");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextMD("#### Procedural Indicators & Copy Button (Phase 7)");
            ImGui::TextMD("Zero-texture vector instruments, timers, and 1-click clipboard integration:");
            ImGui::Spacing();

            // ActivitySpinner
            ImGui::ActivitySpinner("##fms_spin", 11.0f);
            ImGui::TimedTooltip::MD::Text("Procedural **ActivitySpinner**: FMS route calculation & gyro synchronization.");

            ImGui::SameLine(0.0f, 15.0f);
            ImGui::TextUnformatted("FMS Sync");

            // ProgressDial
            ImGui::SameLine(0.0f, 25.0f);
            ImGui::ProgressDial("##fuel_dial", fuelRemaining, 14.0f);
            ImGui::TimedTooltip::MD::Text("Procedural **ProgressDial**: Fuel quantity remaining (%.0f%%).", fuelRemaining * 100.0f);

            ImGui::SameLine(0.0f, 12.0f);
            ImGui::Text("Fuel: %.0f%%", fuelRemaining * 100.0f);

            // CountdownDial
            ImGui::SameLine(0.0f, 25.0f);
            ImGui::CountdownDial("##hold_timer", holdTimerSec, holdTimerMaxSec, 14.0f);
            ImGui::TimedTooltip::MD::Text("Procedural **CountdownDial**: Holding pattern leg timer (%.0fs / %.0fs).", holdTimerSec, holdTimerMaxSec);

            ImGui::SameLine(0.0f, 12.0f);
            ImGui::Text("Hold Leg: %.0fs", holdTimerSec);

            ImGui::Spacing();
            // Procedural 4-digit octal transponder code (0000-7777) with synchronized Copy button
            const float xpndr_w = ImGui::CalcTextSize("7777").x + ImGui::GetStyle().FramePadding.x * 2.0f + 6.0f;
            ImGui::SetNextItemWidth(xpndr_w);
            auto OctalFilter = [](ImGuiInputTextCallbackData* data) -> int {
                if (data->EventChar >= '0' && data->EventChar <= '7')
                    return 0; // Allowed octal squawk digit
                return 1;     // Reject '8', '9', and non-numeric chars
            };
            ImGuiInputTextFlags xpndr_flags = ImGuiInputTextFlags_CharsDecimal |
                                              ImGuiInputTextFlags_AutoSelectAll |
                                              ImGuiInputTextFlags_CallbackCharFilter;
            ImGui::InputText("##xpndr_code", transponderCode, sizeof(transponderCode), xpndr_flags, OctalFilter);
            ImGui::TimedTooltip::MD::Text("Standard 4-digit octal avionics transponder (**0000**-**7777**). Click to select all and replace.");

            ImGui::SameLine(0.0f, 6.0f);
            ImGui::TextUnformatted("Transponder Code");

            ImGui::SameLine(0.0f, 12.0f);
            // Synchronized copy: if actively editing, copy live buffer from InputTextState; otherwise user buffer
            const char* text_to_copy = transponderCode;
            ImGuiID xpndr_id = ImGui::GetID("##xpndr_code");
            if (ImGuiInputTextState* state = ImGui::GetInputTextState(xpndr_id)) {
                if (state->TextA.Data && state->TextA.Data[0] != '\0') {
                    text_to_copy = state->TextA.Data;
                }
            }
            ImGui::CopyButton("##copy_xpndr", text_to_copy);
            ImGui::TimedTooltip::MD::Text("Copy active squawk code (**%s**) to OS clipboard.", text_to_copy);

            ImGui::EndTabItem();
        }

        bool tab_switches = ImGui::BeginTabItem("Switches & Toggles");
        ImGui::TimedTooltip::MD::Text("Animated tactile cockpit rocker switches and pushbutton annunciators.");
        if (tab_switches) {
            ImGui::Spacing();
            ImGui::TextMD("#### Tactile Cockpit Rocker Switches");
            ImGui::TextMD("Animated toggle switches with native hover cursor and state-aware tooltips:");
            ImGui::Spacing();

            // Pattern A: Korry 389 Split-Lens Pushbutton
            ImGui::ToggleButton("Avionics Master (Korry Split-Lens)", &avionicsMaster,
                                "Avionics: <pill color=#7f8c8d>OFFLINE</pill> <pill color=#1a252f fg=#555555>ONLINE</pill>",
                                "Avionics: <pill color=#1a252f fg=#555555>OFFLINE</pill> <pill color=#2ecc71>ONLINE</pill>");
            ImGui::TimedTooltip::MD::Text("Main avionics bus breaker and power distribution (dual-lamp stencil).");

            // Pattern B: Status + Action Cue (interactive guidance)
            ImGui::ToggleButton("Flight Director (Status + Action Cue)", &flightDirector,
                                "Flight Director: <pill color=#7f8c8d>STANDBY</pill>  *(Click to arm command bars)*",
                                "Flight Director: <pill color=#3498db>ACTIVE</pill>  *(Click to disarm)*");
            ImGui::TimedTooltip::MD::Text("Autopilot steering cues on Primary Flight Display with dynamic click guidance.");

            // Pattern C: Modern Glass Annunciator (single dynamic status pill)
            ImGui::ToggleButton("Pitot Heat (Modern Annunciator)", &pitotHeat,
                                "Pitot Heat: <pill color=#7f8c8d>OFF</pill>",
                                "Pitot Heat: <pill color=#e67e22>HEATING</pill>");
            ImGui::TimedTooltip::MD::Text("Airspeed probe anti-ice heating element.");

            ImGui::ToggleButton("Yaw Damper", &yawDamper,
                                "Yaw Damper: <pill color=#7f8c8d>DISARMED</pill>",
                                "Yaw Damper: <pill color=#2ecc71>ENGAGED</pill>");
            ImGui::TimedTooltip::MD::Text("Yaw stability augmentation and turn coordination.");

            ImGui::EndTabItem();
        }

        bool tab_markdown = ImGui::BeginTabItem("Typography & Markdown");
        ImGui::TimedTooltip::MD::Text("Interactive CommonMark elements, status pills, release badges, and clickable links.");
        if (tab_markdown) {
            ImGui::Spacing();
            ImGui::TextMD("#### Interactive Semantic Markdown & CommonMark Features");
            ImGui::TextMD("Inline pills, badges, formatting, and CommonMark 4.1 thematic breaks:");
            ImGui::Spacing();

            ImGui::TextMD("**Status:** Engine 1: <pill color=#2ecc71>NOMINAL</pill>   Fuel Pump: <pill color=#f39c12>LOW PRESS</pill>   Gen 1: <pill color=#e74c3c>OFFLINE</pill>");
            ImGui::TextMD("**Inline Styles:** **bold**, *italic*, ***bold-italic***, ~~strikethrough~~, ++underline++, and `monospace`.");
            ImGui::TextMD("**Nested Tags:** <b><i>Bold Italic</i></b>   |   <style bold italic>Style Tag</style>   |   <style red><b>Red Bold</b></style>");
            ImGui::TextMD("**Custom Style Tags:** Camera offsets: <cam>pitch</cam> and <cam>yaw</cam> (auto-styled as green bold mono).");
            ImGui::TextMD("**Template Aliases:** <tip>Verify altimeter before descent</tip>   |   Press <hotkey>Shift+C</hotkey> to recenter.");
            ImGui::TextMD("**Hotkeys:** <kbd>AP DISC</kbd> or <kbd>Z</kbd>   |   <kdb>Ctrl</kdb>+<kdb>Shift</kdb>+<kdb>F12</kdb>");
            ImGui::TextMD("**Release:** Core: <badge>v2.3.0</badge>   Platform: <badge>macOS Universal</badge>");
            ImGui::TextMD("**Tags & Icons Inside Emphasis:** **Reset (↺ 100%)**   |   *Caution: ⚠ Engine Master*   |   *Status: <badge red>CRITICAL</badge>*   |   **Dual Icons: ↺ and ↻**");

            ImGui::Spacing();
            ImGui::TextMD("#### GitHub Flavored Markdown (GFM) Tables");
            ImGui::TextMD("Turnkey tables with column alignment (`:---`, `:---:`, `---:`), headers, and rich inline styles:");
            ImGui::TextMD("| Instrument | Status | Target | Delta |\n"
                          "| :--- | :---: | ---: | ---: |\n"
                          "| **Heading Bug** | <pill color=#2ecc71>LOCKED</pill> | 180 deg | +0 deg |\n"
                          "| **Altitude Target** | <pill color=#3498db>CLIMBING</pill> | 5,500 ft | +1,200 ft |\n"
                          "| **Vertical Speed** | <pill color=#f39c12>ADVISORY</pill> | +500 fpm | 0 fpm |\n"
                          "| **Transponder** | <pill color=#2ecc71>ACTIVE</pill> | 4721 | Mode C |");

            ImGui::Spacing();
            ImGui::TextMD("---");

            ImGui::TextMD("#### Fenced Code Block with Syntax Highlighting & Copy Button");
            ImGui::TextMD("```cpp\n"
                          "void ConfigureCockpit() {\n"
                          "    // 1-click restore slider with mouse-wheel adjustments\n"
                          "    ImGui::SliderWithReset(\"Heading\", &hdg, 180.0f, 0.0f, 360.0f);\n"
                          "}\n"
                          "```");

            ImGui::Spacing();
            ImGui::TextMD("#### Live Markdown Block with Raw / Preview Toggle");
            ImGui::TextMD("````markdown\n"
                          "### Embedded Quick Reference\n"
                          "* Pitot Heat: <pill color=#e67e22>ARMED</pill>\n"
                          "* Avionics Bus: **14.2 V** (Nominal)\n"
                          "````");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextMD("#### In-Memory Image Service (C++, Markdown & StyledText)");
            ImGui::TextMD("Render memory-buffered textures (e.g. static PNG byte arrays) with zero disk/network I/O:");
            ImGui::Spacing();

            // Option A: Inline StyledText with automatic aspect ratio & icons
            ImGui::StyledText("Option A (StyledText): Transponder: <img name=avionics_beacon width=16 height=16 /> <b color=#2ecc71>ACTIVE</b>");
            ImGui::StyledText("Icon Capsule: <pill icon=avionics_beacon color=#2ecc71>ACTIVE</pill>   <badge icon=avionics_beacon>v1.14</badge>");
            ImGui::StyledText("Unary Annunciator: <pill icon=avionics_beacon color=#e74c3c /> (Alert)   <badge icon=avionics_beacon /> (Standby)");

            ImGui::Spacing();
            // Option B: Markdown image link & tags
            ImGui::TextMD("Option B (Markdown): ![Beacon](<avionics_beacon =20x20>) Status: **ONLINE**");
            ImGui::TextMD("Markdown Tags: <pill icon=avionics_beacon color=#2ecc71>ONLINE</pill>   <badge icon=avionics_beacon>Release</badge>   Unary: <pill icon=avionics_beacon color=#f39c12 />");

            ImGui::Spacing();
            ImGui::TextMD("Vector Icons & Unicode: <icon caret-up /> <icon caret-down /> <icon undo /> <icon redo /> <icon check /> <icon gear color=#f39c12 /> :plane: :star: (Unicode rescue: ↺ ↻ ⚙ ✓)");
            ImGui::StyledText("StyledText Icons: <icon caret-up /> Up   <icon caret-down /> Down   <icon success check /> Passed   <icon danger cross /> Failed");

            ImGui::Spacing();
            // Option C: Direct C++ helpers
            ImGui::Text("Option C (Direct C++):");
            ImGui::SameLine();
            ImGui::ImageNamed("avionics_beacon", ImVec2(20, 0));
            ImGui::SameLine();
            ImGui::PillNamed("avionics_beacon", "ACTIVE", 0xFF2ECC71);
            ImGui::SameLine();
            ImGui::BadgeNamed("avionics_beacon", "v1.14");
            ImGui::SameLine();
            ImGui::PillNamed("avionics_beacon", 0xFFE74C3C);
            ImGui::SameLine();
            ImGui::BadgeNamed("avionics_beacon", 0xFF34495E);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextMD("#### Typography & Inline Sized Text (StyledText, Markdown & Semantic C++)");
            ImGui::TextMD("Dynamic font roles, inline text size tags (`<tiny>`, `<small>`, `<regular>`, `<big>`, `<huge>`), and direct C++ helpers:");
            ImGui::Spacing();

            // 1. Inline Sized Text Tags in StyledText
            ImGui::StyledText("StyledText Sizes: <tiny color=#888888>Tiny (10px)</tiny>  <small color=#3498db>Small (12px)</small>  <regular>Regular (14px)</regular>  <big color=#f39c12>Big (16px)</big>  <huge color=#2ecc71>Huge (20px)</huge>");

            // 2. Inline Sized Text Tags in Markdown
            ImGui::TextMD("Markdown Sizes: <tiny color=#888888>Tiny Tag</tiny>  <small color=#3498db>Small Tag</small>  <regular>Regular Body</regular>  <large color=#e67e22>Large Tag</large>  <huge color=#e74c3c>Huge Tag</huge>");

            // 3. Direct Semantic C++ Helpers
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Semantic C++ Helpers:");
            ImGui::SameLine();
            ImGui::TinyText("<tiny> 10px");
            ImGui::SameLine();
            ImGui::SmallText("<small> 12px");
            ImGui::SameLine();
            ImGui::TitleText("<title> 16px");
            ImGui::SameLine();
            ImGui::MonoText("<mono> 14px");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextMD("#### Clickable Links (Hand Cursor & TimedTooltips)");
            if (ImGui::ButtonLink("Official ImgWindow Docs")) {
                if (ImGui::GetPlatformIO().Platform_OpenInShellFn) {
                    ImGui::GetPlatformIO().Platform_OpenInShellFn(ImGui::GetCurrentContext(), "https://github.com/slgoldberg/ImgWindow");
                }
            }
            ImGui::TimedTooltip::MD::Text("Visit slgoldberg/ImgWindow for complete reference documentation.");

            ImGui::SameLine();
            if (ImGui::ButtonLink("FlyWithLua Project")) {
                if (ImGui::GetPlatformIO().Platform_OpenInShellFn) {
                    ImGui::GetPlatformIO().Platform_OpenInShellFn(ImGui::GetCurrentContext(), "https://github.com/X-Plane/FlyWithLua");
                }
            }
            ImGui::TimedTooltip::MD::Text("Bill Good's FlyWithLua repository.");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextMD("#### Canonical Markdown Buttons & Rich Button Faces (`ImGui::MD::Button`)");
            ImGui::TextMD("Mirrors native `ImGui::Button(label, size)` with full Markdown parsing on button faces, standalone pills, and vector icons:");
            ImGui::Spacing();

            // 1. Standalone Pill & Badge buttons (renders styled oval directly without ugly gray rectangle box!)
            static int s_pillClickCount = 0;
            if (ImGui::MD::Button("<pill orange> :retweet: Refresh </pill>##pill_btn")) {
                s_pillClickCount++;
            }
            ImGui::TimedTooltip::MD::Text("**Standalone Pill Button**:\nRenders pure rounded pill with zero background box.\nClick count: %d", s_pillClickCount);

            ImGui::SameLine();
            static bool s_pillState = true;
            if (ImGui::MD::Button(s_pillState ? "<pill green> ONLINE </pill>##pill_state" : "<pill red> OFFLINE </pill>##pill_state")) {
                s_pillState = !s_pillState;
            }
            ImGui::TimedTooltip::MD::Text("**Dynamic State Pill Button**:\nClick to toggle status pill.");

            ImGui::SameLine();
            if (ImGui::MD::Button("<badge blue>v2.4.0 Release</badge>##badge_btn")) {
                // Clicked
            }
            ImGui::TimedTooltip::MD::Text("**Standalone Badge Button**:\nRenders direct clickable release badge.");

            // 2. Rich Markdown Text & Vector Icon Buttons
            ImGui::SameLine();
            if (ImGui::MD::Button("**Bold** & *Italic*##btn_rich", ImVec2(120.0f, 0.0f))) {
                // Clicked
            }
            ImGui::TimedTooltip::MD::Text("Standard button chrome with rich formatted Markdown text centered inside.");

            ImGui::SameLine();
            if (ImGui::ButtonMD("<icon undo color=#e74c3c /> **Reset**##btn_undo", "**Warning Tooltip:**\nReverts all test settings.")) {
                // Clicked
            }

            ImGui::EndTabItem();
        }

        bool tab_editor = ImGui::BeginTabItem("Live Editor & Scratchpad");
        ImGui::TimedTooltip::MD::Text("Interactive split-pane CommonMark editor with soft wrapping and 1-click toolbar.");
        if (tab_editor) {
            ImGui::Spacing();
            ImGui::TextMD("#### Live Markdown Scratchpad (Smart Formatting & 1-Click Toolbar)");
            ImGui::TextMD("Type on the left to see live CommonMark on the right. Select text and click any toolbar item to format selection in-place:");
            ImGui::Spacing();

            // Track active selection from previous frame's InputText before toolbar buttons deactivate it
            ImGuiContext& g = *GImGui;
            if (g.InputTextState.ID != 0 && g.InputTextState.Stb) {
                int s = g.InputTextState.GetSelectionStart();
                int e = g.InputTextState.GetSelectionEnd();
                if (s > e) std::swap(s, e);
                int cur = g.InputTextState.GetCursorPos();
                if (s >= 0) {
                    if (e > s) {
                        has_active_selection = true;
                        cached_sel_start = s;
                        cached_sel_end = e;
                    } else {
                        has_active_selection = false;
                        cached_sel_start = -1;
                        cached_sel_end = -1;
                    }
                    if (cur >= 0) {
                        cached_cur_pos = cur;
                    }
                }
            }

            auto wrap_or_insert = [&](const std::string& prefix, const std::string& suffix, const std::string& default_text, bool is_line_prefix) {
                int sel_start = -1, sel_end = -1, cur_pos = -1;
                if (g.InputTextState.ID != 0 && g.InputTextState.Stb) {
                    sel_start = g.InputTextState.GetSelectionStart();
                    sel_end = g.InputTextState.GetSelectionEnd();
                    if (sel_start > sel_end) std::swap(sel_start, sel_end);
                    cur_pos = g.InputTextState.GetCursorPos();
                }

                bool is_selected = (sel_start >= 0 && sel_end > sel_start);
                if (!is_selected && has_active_selection && cached_sel_start >= 0 && cached_sel_end > cached_sel_start) {
                    sel_start = cached_sel_start;
                    sel_end = cached_sel_end;
                    cur_pos = cached_cur_pos;
                    is_selected = true;
                }

                int new_start = -1;
                int new_end = -1;

                if (is_selected && sel_end <= (int)scratchpadText.length()) {
                    if (is_line_prefix) {
                        size_t prev_nl = scratchpadText.rfind('\n', sel_start > 0 ? sel_start - 1 : 0);
                        size_t line_start = (prev_nl == std::string::npos) ? 0 : prev_nl + 1;
                        scratchpadText.insert(line_start, prefix);
                        new_start = sel_start + (int)prefix.length();
                        new_end = sel_end + (int)prefix.length();
                    } else {
                        std::string before = scratchpadText.substr(0, sel_start);
                        std::string selected = scratchpadText.substr(sel_start, sel_end - sel_start);
                        std::string after = scratchpadText.substr(sel_end);

                        size_t lead_ws_len = 0;
                        while (lead_ws_len < selected.length() && (selected[lead_ws_len] == ' ' || selected[lead_ws_len] == '\t' || selected[lead_ws_len] == '\n' || selected[lead_ws_len] == '\r')) {
                            lead_ws_len++;
                        }
                        size_t trail_ws_len = 0;
                        while (trail_ws_len < (selected.length() - lead_ws_len) && (selected[selected.length() - 1 - trail_ws_len] == ' ' || selected[selected.length() - 1 - trail_ws_len] == '\t' || selected[selected.length() - 1 - trail_ws_len] == '\n' || selected[selected.length() - 1 - trail_ws_len] == '\r')) {
                            trail_ws_len++;
                        }

                        std::string lead_ws = selected.substr(0, lead_ws_len);
                        std::string core_text = selected.substr(lead_ws_len, selected.length() - lead_ws_len - trail_ws_len);
                        std::string trail_ws = selected.substr(selected.length() - trail_ws_len);

                        scratchpadText = before + lead_ws + prefix + core_text + suffix + trail_ws + after;
                        new_start = sel_start + (int)lead_ws.length() + (int)prefix.length();
                        new_end = new_start + (int)core_text.length();
                    }
                } else {
                    int ins_pos = cur_pos;
                    if (ins_pos < 0 || ins_pos > (int)scratchpadText.length()) {
                        if (cached_cur_pos >= 0 && cached_cur_pos <= (int)scratchpadText.length()) {
                            ins_pos = cached_cur_pos;
                        } else {
                            ins_pos = (int)scratchpadText.length();
                        }
                    }

                    if (is_line_prefix) {
                        size_t prev_nl = scratchpadText.rfind('\n', ins_pos > 0 ? ins_pos - 1 : 0);
                        size_t line_start = (prev_nl == std::string::npos) ? 0 : prev_nl + 1;
                        size_t next_nl = scratchpadText.find('\n', line_start);
                        if (next_nl == std::string::npos) next_nl = scratchpadText.length();

                        std::string line_content = scratchpadText.substr(line_start, next_nl - line_start);
                        bool is_empty_line = true;
                        for (char ch : line_content) {
                            if (ch != ' ' && ch != '\t' && ch != '\r') {
                                is_empty_line = false;
                                break;
                            }
                        }

                        if (is_empty_line) {
                            std::string before = scratchpadText.substr(0, line_start);
                            std::string after = scratchpadText.substr(next_nl);
                            scratchpadText = before + prefix + default_text + after;
                            new_start = (int)before.length() + (int)prefix.length();
                            new_end = new_start + (int)default_text.length();
                        } else {
                            scratchpadText.insert(line_start, prefix);
                            new_start = ins_pos + (int)prefix.length();
                            new_end = new_start;
                        }
                    } else {
                        std::string before = scratchpadText.substr(0, ins_pos);
                        std::string after = scratchpadText.substr(ins_pos);
                        scratchpadText = before + prefix + default_text + suffix + after;
                        new_start = ins_pos + (int)prefix.length();
                        new_end = new_start + (int)default_text.length();
                    }
                }

                ImGuiID target_id = (g.InputTextState.ID != 0) ? g.InputTextState.ID : g.InputTextDeactivatedState.ID;
                if (target_id != 0) {
                    g.InputTextState.ReloadSelectionStart = new_start;
                    g.InputTextState.ReloadSelectionEnd = new_end;
                    g.InputTextState.WantReloadUserBuf = true;
                    g.InputTextReactivateId = target_id;
                    g.InputTextDeactivatedState.ID = 0;
                }

                has_active_selection = false;
                cached_sel_start = -1;
                cached_sel_end = -1;
            };

            static ImGuiID s_scratchpadEditorId = 0;

            // 1-Click Formatting Toolbar with Smart Selection Wrapping (Lance's Pattern):
            if (ToolbarButton("H1", "Prepend `# ` heading to current line or selection")) {
                wrap_or_insert("# ", "", "Heading 1", true);
            }

            ImGui::SameLine();
            if (ToolbarButton("H2", "Prepend `## ` heading to current line or selection")) {
                wrap_or_insert("## ", "", "Heading 2", true);
            }

            ImGui::SameLine();
            if (ToolbarButton(" B ", "Wrap selection in `**bold**` markup")) {
                wrap_or_insert("**", "**", "bold text", false);
            }

            ImGui::SameLine();
            if (ToolbarButton(" I ", "Wrap selection in `*italic*` markup")) {
                wrap_or_insert("*", "*", "italic text", false);
            }

            ImGui::SameLine();
            if (ToolbarButton(" U ", "Wrap selection in `<u>underline</u>` tags")) {
                wrap_or_insert("<u>", "</u>", "underlined text", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("* List", "Prepend bullet `* ` to current line or selection")) {
                wrap_or_insert("* ", "", "Item description", true);
            }

            ImGui::SameLine();
            if (ToolbarButton("Pill", "Wrap selection in `<pill color=...>` status capsule")) {
                wrap_or_insert("<pill color=#2ecc71>", "</pill>", "ONLINE", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("Badge", "Wrap selection in `<badge>` release chip")) {
                wrap_or_insert("<badge>", "</badge>", "v2.3.0", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("Shield...", "Open **Shields.io & Badge Generator** to create custom badges & status pills")) {
                showShieldModal = true;
            }

            ImGui::SameLine();
            if (ToolbarButton("Key", "Wrap selection in `<kbd>` keycap")) {
                wrap_or_insert("<kbd>", "</kbd>", "Alt", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("Code", "Wrap selection in syntax-highlighted code block with [Copy] button")) {
                wrap_or_insert("\n```cpp\n", "\n```\n", "void FlightPlan() {\n}\n", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("---", "Insert CommonMark 4.1 thematic break divider (---)")) {
                wrap_or_insert("\n---\n", "", "", false);
            }

            ImGui::SameLine();
            if (ToolbarButton("Icon", "Insert in-memory registered beacon icon `![Beacon](avionics_beacon)`")) {
                wrap_or_insert("![Beacon](avionics_beacon)", "", "", false);
            }

            ImGui::SameLine();
            ImGui::TextDisabled("|");

            ImGui::SameLine();
            if (ToolbarButton("Clear", "Clears the scratchpad editor buffer")) {
                scratchpadText.clear();
                cached_cur_pos = 0;
                has_active_selection = false;
                cached_sel_start = cached_sel_end = -1;
                ImGuiID target_id = (g.InputTextState.ID != 0) ? g.InputTextState.ID : (g.InputTextDeactivatedState.ID != 0 ? g.InputTextDeactivatedState.ID : s_scratchpadEditorId);
                if (target_id != 0) {
                    g.InputTextState.ReloadSelectionStart = 0;
                    g.InputTextState.ReloadSelectionEnd = 0;
                    g.InputTextState.WantReloadUserBuf = true;
                    g.InputTextReactivateId = target_id;
                    g.InputTextDeactivatedState.ID = 0;
                }
            }

            ImGui::SameLine();
            if (ToolbarButton("Reset Sample", "Restores the flight briefing sample")) {
                scratchpadText = "### Live Flight Briefing\n"
                                 "* Status: <pill color=#2ecc71>CLEARED</pill>\n"
                                 "* Departure: **KSEA** Rwy 16L\n"
                                 "* Route: `SEA8 JAWBN Q29`\n"
                                 "* Clearance: <big color=#3498db>FL180</big> <tiny color=#888888>SUB-BUS 2A</tiny> <small color=#27ae60>14.2V</small> <huge color=#e74c3c>ALERT</huge>\n"
                                 "---\n"
                                 "| Waypoint | Target Alt | Fuel | ETA |\n"
                                 "| :--- | :---: | ---: | ---: |\n"
                                 "| **SEA8** | 3,000 ft | 98% | 18:05z |\n"
                                 "| **JAWBN** | 12,000 ft | 92% | 18:18z |\n"
                                 "| **Q29** | FL240 | 85% | 18:34z |\n"
                                 "<!-- table-preset: width=420 sort=\"Waypoint\" asc -->\n"
                                 "---\n"
                                 "```cpp\n"
                                 "void ExecuteClimb() {\n"
                                 "    Autopilot::SetTargetAlt(14000);\n"
                                 "}\n"
                                 "```";
            }
            ImGui::TimedTooltip::MD::Text("Restores the flight briefing sample");

            // --- File Persistence Toolbar (imgui_file_dialog.h integration) ---
            ImGui::Spacing();
            const char* notes_dir = "./Resources/plugins/imgui4xp/notes";
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Note:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(185.0f);
            if (ImGui::FilePickerCombo("##note_picker", &currentNoteFilename, notes_dir, ".md;.txt;.markdown")) {
                LoadNote(std::string(notes_dir) + "/" + currentNoteFilename);
            }
            ImGui::TimedTooltip::MD::Text(std::string("**Active Note Selector** (`FilePickerCombo`)\n\nScan and switch notes in `") + notes_dir + "` using native C++11 filesystem scanning.");

            ImGui::SameLine();
            if (ToolbarButton("Save", "**Save Note**\n\nPersist buffer directly to active note on disk.")) {
                if (!currentNoteFilename.empty() && currentNoteFilename != "(Untitled)") {
                    SaveNote(std::string(notes_dir) + "/" + currentNoteFilename);
                } else {
                    showSaveDialog = true;
                }
            }

            ImGui::SameLine();
            if (ToolbarButton("Open...", "**Open Note Dialog** (`OpenFileDialog`)\n\nBrowse X-Plane directories in an in-cockpit modal with live search & file sizes.")) {
                showOpenDialog = true;
            }

            ImGui::SameLine();
            if (ToolbarButton("Save As...", "**Save Note As Dialog** (`SaveFileDialog`)\n\nSave note under a new filename with built-in overwrite protection.")) {
                showSaveDialog = true;
            }

            ImGui::SameLine();
            if (ToolbarButton("+ New", "Start a blank new note buffer")) {
                scratchpadText.clear();
                currentNoteFilename = "(Untitled)";
                noteStatusMsg = "New note created";
                noteStatusTimer = 3.0f;
                cached_cur_pos = 0;
                has_active_selection = false;
                cached_sel_start = cached_sel_end = -1;
                ImGuiID target_id = (g.InputTextState.ID != 0) ? g.InputTextState.ID : (g.InputTextDeactivatedState.ID != 0 ? g.InputTextDeactivatedState.ID : s_scratchpadEditorId);
                if (target_id != 0) {
                    g.InputTextState.ReloadSelectionStart = 0;
                    g.InputTextState.ReloadSelectionEnd = 0;
                    g.InputTextState.WantReloadUserBuf = true;
                    g.InputTextReactivateId = target_id;
                    g.InputTextDeactivatedState.ID = 0;
                }
            }

            if (!noteStatusMsg.empty() && noteStatusTimer > 0.0f) {
                ImGui::SameLine();
                std::string pill_str = "<pill color=#2ecc71>" + noteStatusMsg + "</pill>";
                ImGui::TextMD(pill_str);
            }

            ImGui::Spacing();

            // Side-by-side split panels with 0-padded interactive draggable splitter:
            float avail_w = ImGui::GetContentRegionAvail().x;
            static float s_scratchpadSplitRatio = 0.50f;
            const float splitter_w = ImGui::GetStyle().ItemSpacing.x; // Exact standard item spacing gap (0 extra padding)
            const float total_pane_w = ImMax(200.0f, avail_w - splitter_w);

            // Compute minimum required widths based on pane header controls (so title & toggle never overstrike)
            const float left_title_w = ImGui::CalcTextSize("Raw Input (Markdown):").x;
            const float left_wrap_w = CalcToggleButtonWidth("Word Wrap");
            const float min_left_w = left_title_w + ImGui::GetStyle().ItemSpacing.x + left_wrap_w + ImGui::GetStyle().WindowPadding.x * 2.0f + 6.0f;

            const float right_title_w = ImGui::CalcTextSize("Live Rendered Output:").x;
            const float right_sync_w = CalcToggleButtonWidth("Sync Scroll");
            const float min_right_w = right_title_w + ImGui::GetStyle().ItemSpacing.x + right_sync_w + ImGui::GetStyle().WindowPadding.x * 2.0f + 6.0f;

            float pane_left_w = total_pane_w * s_scratchpadSplitRatio;
            if (total_pane_w >= (min_left_w + min_right_w)) {
                pane_left_w = ImClamp(pane_left_w, min_left_w, total_pane_w - min_right_w);
            } else {
                pane_left_w = total_pane_w * 0.50f;
            }
            float pane_right_w = total_pane_w - pane_left_w;

            float avail_h = ImGui::GetContentRegionAvail().y;
            // Reserve space for Close button footer (rendered at 1.0x unscaled chrome scale, flush against child borders)
            float unscaled_frame_h = ImGui::GetStyle().FontSizeBase + ImGui::GetStyle().FramePadding.y * 2.0f;
            float footer_reserve = unscaled_frame_h + ImGui::GetStyle().ItemSpacing.y;
            float pane_h = ImMax(120.0f, avail_h - footer_reserve);

            // Seamless 1-click active retention for Word Wrap & Sync Scroll toggles:
            static ImRect s_wordWrapRect(0, 0, 0, 0);
            static ImRect s_syncScrollRect(0, 0, 0, 0);
            ImGuiIO& io = ImGui::GetIO();
            if (io.MouseClicked[0] && (s_wordWrapRect.Contains(io.MousePos) || s_syncScrollRect.Contains(io.MousePos))) {
                if (g.InputTextState.ID != 0 && g.ActiveId == g.InputTextState.ID) {
                    g.InputTextReactivateId = g.ActiveId;
                }
            }

            // Left Pane: Raw Editor (with soft word-wrap toggle and tab input!)
            ImGui::BeginChild("ScratchpadInputPane", ImVec2(pane_left_w, pane_h), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Raw Input (Markdown):");

            // Right-aligned Word Wrap toggle (disabled by default for code/tables horizontal scroll)
            float wrap_w = CalcToggleButtonWidth("Word Wrap");
            float min_wrap_x = ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x;
            float target_wrap_x = ImGui::GetWindowWidth() - wrap_w - ImGui::GetStyle().WindowPadding.x;
            ImGui::SameLine(ImMax(min_wrap_x, target_wrap_x));
            static bool editorWordWrap = true;
            ToolbarToggleButton("Word Wrap##editor_wrap", &editorWordWrap, "**Word Wrap**\n\nToggle soft word-wrapping in raw editor. When unchecked, long lines and tables scroll horizontally.");
            s_wordWrapRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

            ImGui::Separator();

            ImGuiInputTextFlags editor_flags = ImGuiInputTextFlags_AllowTabInput;
            if (editorWordWrap)
                editor_flags |= ImGuiInputTextFlags_WordWrap;

            s_scratchpadEditorId = ImGui::GetID("##editor");
            ImGui::InputTextMultiline("##editor", &scratchpadText, ImVec2(-FLT_MIN, -FLT_MIN), editor_flags);
            ImGuiWindow* input_child_win = ImGui::GetCurrentWindow();
            ImGuiWindow* editor_win = input_child_win->DC.ChildWindows.empty() ? nullptr : input_child_win->DC.ChildWindows.back();
            ImGui::EndChild();

            // Interactive Splitter Grip between Left & Right Panes (flush 0-padded against child borders)
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::InvisibleButton("##scratchpad_splitter", ImVec2(splitter_w, pane_h));
            bool splitter_hovered = ImGui::IsItemHovered();
            bool splitter_active = ImGui::IsItemActive();
            if (splitter_hovered || splitter_active) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
            if (splitter_active) {
                float delta_x = ImGui::GetIO().MouseDelta.x;
                if (delta_x != 0.0f && total_pane_w > 0.0f) {
                    s_scratchpadSplitRatio = ImClamp(s_scratchpadSplitRatio + delta_x / total_pane_w, 0.05f, 0.95f);
                }
            }
            if (splitter_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                s_scratchpadSplitRatio = 0.50f; // 1-click restore balanced 50/50 split
            }
            // Draw subtle splitter grip line centered in the standard item spacing gap
            ImVec2 split_p0 = ImGui::GetItemRectMin();
            ImVec2 split_p1 = ImGui::GetItemRectMax();
            float split_mid_x = (split_p0.x + split_p1.x) * 0.5f;
            ImU32 split_col = splitter_active ? ImGui::GetColorU32(ImGuiCol_SeparatorActive)
                            : (splitter_hovered ? ImGui::GetColorU32(ImGuiCol_SeparatorHovered)
                                                : ImGui::GetColorU32(ImGuiCol_Separator, 0.4f));
            ImGui::GetWindowDrawList()->AddLine(ImVec2(split_mid_x, split_p0.y + 4.0f), ImVec2(split_mid_x, split_p1.y - 4.0f), split_col, 1.0f);

            ImGui::SameLine(0.0f, 0.0f);

            // Right Pane: Live Rendered Output (with auto soft-wrapping & horizontal scrolling!)
            ImGui::BeginChild("ScratchpadPreviewPane", ImVec2(pane_right_w, pane_h), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Live Rendered Output:");

            // Right-aligned Sync Scroll toggle (disabled by default)
            float sync_w = CalcToggleButtonWidth("Sync Scroll");
            float min_sync_x = ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x;
            float target_sync_x = ImGui::GetWindowWidth() - sync_w - ImGui::GetStyle().WindowPadding.x;
            ImGui::SameLine(ImMax(min_sync_x, target_sync_x));
            ToolbarToggleButton("Sync Scroll", &syncScroll, "**Synchronized Scrolling** (Experimental)\n\nProportionally syncs scroll positions between raw editor and rendered markdown.");
            s_syncScrollRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

            ImGui::Separator();
            ImGuiWindow* preview_win = nullptr;
            if (ImGui::BeginChild("ScratchpadPreviewScroll", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
                preview_win = ImGui::GetCurrentWindow();
                if (scratchpadText.empty()) {
                    ImGui::TextDisabled("(Editor buffer is empty. Type on the left or click Reset Sample...)");
                } else {
                    ImGui::MarkdownConfig scratchpadConfig = *ImGui::GetDefaultMarkdownConfig();
                    scratchpadConfig.userData = this;
                    scratchpadConfig.tableStateCallback = [](const ImGui::MarkdownTableState& state, void* userData) {
                        if (!userData || !state.isCommitted) return;
                        auto* widget = static_cast<ImGeeShowcaseWidget*>(userData);
                        widget->pendingTableStates.push_back(state);
                    };
                    ImGui::TextWrappedMD(scratchpadText.c_str(), &scratchpadConfig);

                    if (!pendingTableStates.empty()) {
                        for (const auto& st : pendingTableStates) {
                            scratchpadText = ImGui::ApplyTableStateToMarkdown(scratchpadText, st);
                        }
                        pendingTableStates.clear();
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndChild();

            static bool s_lastBothScrollable = false;
            bool both_scrollable = (syncScroll && editor_win && preview_win && editor_win->ScrollMax.y > 0.0f && preview_win->ScrollMax.y > 0.0f);
            bool sync_resumed = (both_scrollable && !s_lastBothScrollable);
            s_lastBothScrollable = both_scrollable;

            // Two-way synchronized scrolling arbitration (only active when BOTH panes have scrollable content)
            if (both_scrollable) {
                if (sync_resumed) {
                    float r = ImClamp(editor_win->Scroll.y / editor_win->ScrollMax.y, 0.0f, 1.0f);
                    float target_y = r * preview_win->ScrollMax.y;
                    ImGui::SetScrollY(preview_win, target_y);
                    preview_win->Scroll.y = target_y;
                }
                bool editor_hovered = ImGui::IsMouseHoveringRect(editor_win->Rect().Min, editor_win->Rect().Max);
                bool preview_hovered = ImGui::IsMouseHoveringRect(preview_win->Rect().Min, preview_win->Rect().Max);
                ImGuiID sb_editor = ImGui::GetWindowScrollbarID(editor_win, ImGuiAxis_Y);
                ImGuiID sb_preview = ImGui::GetWindowScrollbarID(preview_win, ImGuiAxis_Y);

                bool editor_focused = (g.InputTextState.ID != 0 && (g.ActiveId == s_scratchpadEditorId || g.InputTextState.ID == s_scratchpadEditorId));
                bool editor_scrolling_wheel = (editor_hovered && ImGui::GetIO().MouseWheel != 0.0f);
                bool preview_scrolling_wheel = (preview_hovered && ImGui::GetIO().MouseWheel != 0.0f);
                bool sb_editor_active = (g.ActiveId == sb_editor);
                bool sb_preview_active = (g.ActiveId == sb_preview);

                bool preview_driving = sb_preview_active || preview_scrolling_wheel;
                bool editor_driving = sb_editor_active || editor_scrolling_wheel || (editor_focused && !preview_driving);

                if (editor_driving && !preview_driving) {
                    float target_y = 0.0f;
                    // Pin to bottom if editor is at/near bottom, or user is typing near the end of the note
                    bool at_bottom = (editor_win->Scroll.y >= editor_win->ScrollMax.y - 12.0f);
                    if (!at_bottom && editor_focused && cached_cur_pos >= 0 && !scratchpadText.empty()) {
                        size_t last_nl = scratchpadText.rfind('\n');
                        if (last_nl == std::string::npos || cached_cur_pos >= (int)last_nl) {
                            at_bottom = true;
                        }
                    }

                    if (at_bottom) {
                        target_y = preview_win->ScrollMax.y;
                    } else if (editor_win->Scroll.y <= 4.0f) {
                        target_y = 0.0f;
                    } else {
                        float r = ImClamp(editor_win->Scroll.y / editor_win->ScrollMax.y, 0.0f, 1.0f);
                        target_y = r * preview_win->ScrollMax.y;
                    }
                    ImGui::SetScrollY(preview_win, target_y);
                    preview_win->Scroll.y = target_y;
                } else if (preview_driving) {
                    float target_y = 0.0f;
                    if (preview_win->Scroll.y >= preview_win->ScrollMax.y - 4.0f) {
                        target_y = editor_win->ScrollMax.y;
                    } else if (preview_win->Scroll.y <= 4.0f) {
                        target_y = 0.0f;
                    } else {
                        float r = ImClamp(preview_win->Scroll.y / preview_win->ScrollMax.y, 0.0f, 1.0f);
                        target_y = r * editor_win->ScrollMax.y;
                    }
                    ImGui::SetScrollY(editor_win, target_y);
                    editor_win->Scroll.y = target_y;
                }
            }

            // --- In-cockpit OpenFileDialog modal ---
            if (showOpenDialog) {
                if (ImGui::OpenFileDialog("Open Markdown Note", &showOpenDialog, &dialogFilePath, notes_dir, ".md;.txt;.markdown")) {
                    LoadNote(dialogFilePath);
                }
            }

            // --- In-cockpit SaveFileDialog modal (with Confirm Overwrite protection) ---
            if (showSaveDialog) {
                std::string default_name = (currentNoteFilename.empty() || currentNoteFilename == "(Untitled)") ? "flight_notes.md" : currentNoteFilename;
                if (ImGui::SaveFileDialog("Save Markdown Note As", &showSaveDialog, &dialogFilePath, notes_dir, default_name.c_str(), ".md;.txt;.markdown")) {
                    SaveNote(dialogFilePath);
                }
            }

            // --- Shields.io & Custom Badge Generator Modal ---
            if (showShieldModal) {
                ImGui::OpenPopup("Shield / Badge Generator");
            }
            if (ImGui::BeginPopupModal("Shield / Badge Generator", &showShieldModal, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Spacing();
                ImGui::TextMD("#### Generate Custom Badge or Shields.io Image");
                ImGui::TextDisabled("Configure label, message, and color to generate badges:");
                ImGui::Spacing();

                ImGui::InputText("Label##shield_lbl", shieldLabel, sizeof(shieldLabel));
                ImGui::InputText("Message##shield_msg", shieldMessage, sizeof(shieldMessage));

                const char* color_names[] = { "Blue (#3498db)", "Green (#2ecc71)", "Orange (#e67e22)", "Red (#e74c3c)", "Purple (#9b59b6)" };
                const char* color_hex[]   = { "3498db", "2ecc71", "e67e22", "e74c3c", "9b59b6" };
                const char* shield_colors[] = { "blue", "success", "orange", "red", "purple" };
                ImGui::Combo("Color##shield_col", &shieldColorIdx, color_names, IM_ARRAYSIZE(color_names));

                const char* style_names[] = { "Shields.io Online Image (![...](...))", "Native ImGee Vector Badge (<badge>)", "Native ImGee Status Pill (<pill>)" };
                ImGui::Combo("Format##shield_fmt", &shieldStyleIdx, style_names, IM_ARRAYSIZE(style_names));

                ImGui::Spacing();
                ImGui::TextDisabled("Preview Output:");
                std::string generated_text;
                if (shieldStyleIdx == 0) {
                    std::string url = FormatShieldsIoUrl(shieldLabel, shieldMessage, shield_colors[shieldColorIdx]);
                    generated_text = "![" + std::string(shieldLabel) + " - " + std::string(shieldMessage) + "](" + url + ")";
                } else if (shieldStyleIdx == 1) {
                    std::string lbl(shieldLabel);
                    std::string body = lbl.empty() ? std::string(shieldMessage) : (lbl + ": " + std::string(shieldMessage));
                    generated_text = "<badge color=#" + std::string(color_hex[shieldColorIdx]) + ">" + body + "</badge>";
                } else {
                    generated_text = "<pill color=#" + std::string(color_hex[shieldColorIdx]) + ">" + std::string(shieldMessage) + "</pill>";
                }
                ImGui::TextWrapped("%s", generated_text.c_str());

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                if (ImGui::Button("Insert into Note", ImVec2(140.0f, 0.0f))) {
                    wrap_or_insert(generated_text, "", "", false);
                    showShieldModal = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(90.0f, 0.0f)) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    showShieldModal = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // 3. Restore window chrome scale for footer
    ImGui::SetWindowFontScale(1.0f);
    if (ImGui::Button("Close")) {
        SetVisible(false);
    }

    ImGui::SameLine();
    if (ImGui::MD::Button("Quick Status ▲")) {
        ImGui::OpenPopupAtItem("##QuickStatusPopup", ImGui::ImGuiPopupAnchor_AboveItem, ImGui::ImGuiPopupAlign_Left);
    }
    ImGui::TimedTooltip::MD::Text("**Anchored Popup (AboveItem)**:\nClick to toggle status popup blossoming upward above button.\nMuscle-memory safe: click button again while open to close.");

    if (ImGui::BeginAnchoredPopup("##QuickStatusPopup")) {
        ImGui::TextMD("### Cockpit Quick Status");
        ImGui::Separator();
        ImGui::StyledText("Transponder: <pill icon=avionics_beacon color=#2ecc71>4721 ACTIVE</pill>");
        ImGui::StyledText("GPS Nav: <pill icon=avionics_beacon color=#3498db>LOCKED</pill>   Pitot: <pill icon=avionics_beacon color=#e67e22 />");
        ImGui::Spacing();
        static int s_PopupCamMode = 0;
        static const char* const popupCamItems[] = { "Cockpit 3D", "Chase 30m", "Tower View" };
        static const char* const popupCamTips[] = {
            "**Cockpit 3D**\nPilot eye 3D perspective with 6DOF head movement.",
            "**Chase 30m**\nExternal chase camera trailing 30m behind aircraft.",
            "**Tower View**\nStationary airport control tower vantage point."
        };
        ImGui::TextUnformatted("View:");
        ImGui::SameLine();
        ImGui::SelectPulldown("##PopupCamMode", &s_PopupCamMode, popupCamItems, 3, popupCamTips, 130.0f, ImGui::ImGuiPopupAnchor_BelowItem, ImGui::ImGuiPopupAlign_Left, true /* wrap */);
        ImGui::TimedTooltip::MD::Text(
            s_PopupCamMode == 0 ? "**Cockpit 3D View**\nDirect pilot perspective inside virtual cockpit." :
            s_PopupCamMode == 1 ? "**Chase 30m View**\nExternal chase camera trailing 30m behind aircraft." :
                                  "**Tower View**\nStationary airport control tower vantage point.");
        ImGui::Spacing();
        float last_h = ImGui::GetLastPopupHeight("##QuickStatusPopup");
        ImGui::TextDisabled("Measured Popup Height: %.1f px", last_h);
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    static int s_DockCamMode = 0;
    static const char* const dockCamItems[] = { "Cockpit 3D", "Chase 30m", "Tower View" };
    static const char* const dockCamTips[] = {
        "**Cockpit 3D**\nPilot eye 3D perspective with 6DOF head movement.",
        "**Chase 30m**\nExternal chase camera trailing 30m behind aircraft.",
        "**Tower View**\nStationary airport control tower vantage point."
    };
    if (ImGui::SelectPulldownMD("##DockCamMode", &s_DockCamMode, dockCamItems, 3, dockCamTips, 130.0f, ImGui::ImGuiPopupAnchor_AboveItem, ImGui::ImGuiPopupAlign_Left, true /* wrap */)) {
        // Selection changed
    }
    ImGui::TimedTooltip::MD::Text(
        "**Upward Blossoming Pulldown** (`ImGuiPopupAnchor_AboveItem`):\n"
        "Active Camera: **%s**\n"
        "Pops up above status bar with inverted procedural vector caret.",
        dockCamItems[s_DockCamMode]);

    // ImGee Theme Selector Combo (Charcoal, Rusty, ImGui Stock)
    const char* const theme_names[] = {
        "Charcoal (Default)",
        "Rusty (Aviation)",
        "ImGui Dark",
        "ImGui Classic",
        "ImGui Light"
    };
    const float theme_combo_w = 145.0f;
    const float theme_label_w = ImGui::CalcTextSize("Theme:").x;
    const float theme_suite_w = theme_label_w + ImGui::GetStyle().ItemSpacing.x + theme_combo_w;
    const float footer_margin = ImGui::GetStyle().WindowPadding.x + ((cur_win && cur_win->ScrollbarY) ? cur_win->ScrollbarSizes.x : 0.0f);
    const float target_theme_x = ImGui::GetWindowWidth() - theme_suite_w - footer_margin;

    if (target_theme_x > ImGui::GetCursorPosX() + 10.0f) {
        ImGui::SameLine(target_theme_x);
    } else {
        ImGui::SameLine(0.0f, 10.0f);
    }

    static const char* const theme_tips[] = {
        "**Charcoal (Default)**\nSleek modern slate/graphite flight deck palette.",
        "**Rusty (Aviation)**\nWarm aviation bronze & terracotta palette (ABC classic).",
        "**ImGui Dark**\nStock Dear ImGui dark gray color scheme.",
        "**ImGui Classic**\nLegacy Dear ImGui purplish-blue color scheme.",
        "**ImGui Light**\nHigh-contrast light daytime color scheme."
    };
    ImGui::TextUnformatted("Theme:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(theme_combo_w);
    bool theme_changed = ImGui::MouseWheelCombo("##ThemePicker", &s_currentTheme, theme_names, IM_ARRAYSIZE(theme_names), -1, true /* wrap */, theme_tips);
    if (theme_changed) {
        switch (s_currentTheme) {
            case 0: ImGui::ApplyDefaultTheme(); break;
            case 1: ImGui::StyleColorsRusty(); break;
            case 2: ImGui::StyleColorsDark(); break;
            case 3: ImGui::StyleColorsClassic(); break;
            case 4: ImGui::StyleColorsLight(); break;
            default: break;
        }
        if (mIsGlassHUD || IsSelfDecorated()) {
            ImGuiStyle& st = ImGui::GetStyle();
            st.Colors[ImGuiCol_WindowBg].w = mWindowOpacity;
            st.Colors[ImGuiCol_Border].w = std::min(1.0f, mWindowOpacity * 0.8f + 0.2f);
        }
    }
    ImGui::TimedTooltip::MD::Text(
        "**ImGee Theme Presets** (Click dropdown or roll mouse wheel):\n"
        "• **Charcoal**: Sleek modern slate/graphite flight deck palette (default).\n"
        "• **Rusty**: Warm aviation bronze & terracotta palette (ABC classic).\n"
        "• **ImGui Dark / Classic / Light**: Stock Dear ImGui fallback schemes."
    );

    // Continuously preserve user's customized 100% baseline window layout (position & dimensions)
    if (std::abs(_fontAdjust - 1.0f) < 0.001f) {
        GetCurrentWindowGeometry(_baselineRect.l, _baselineRect.t, _baselineRect.r, _baselineRect.b);
        _baselineRect.valid = true;
    }
}

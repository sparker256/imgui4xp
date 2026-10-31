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

}

//
// MARK: Global data and functions
//

// The raw TTF data of OpenFontIcons has been generated into the following file
#include "fa-solid-900.inc"

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
                  ICON_FA_MOUSE_POINTER ICON_FA_MOUSE ICON_FA_HAND_POINTER);
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

  // 3. Monospace Font - Prefer RobotoMono-Regular, fallback to B612Mono / Menlo / DejaVuSansMono
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

  // Configure default Markdown font associations & link callback
  static ImGui::MarkdownConfig s_MarkdownConfig;
  s_MarkdownConfig.h1Font = Font9;      // Roboto-Bold 20
  s_MarkdownConfig.h2Font = Font8;      // Roboto-Bold 15
  s_MarkdownConfig.h3Font = Font8;      // Roboto-Bold 15
  s_MarkdownConfig.boldFont = FontBold; // Roboto-Bold 15
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
    
    // We take the parameter combination "SelfDecorateResizeable" + "LayerFlightOverlay"
    // to mean: simulate HUD
    if (decoration  == xplm_WindowDecorationSelfDecoratedResizable &&
        layer       == xplm_WindowLayerFloatingWindows)
    {
        // let's set a fairly transparent, barely visible background
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0, 0x10);
        // There's no window decoration, so to move the window we need to
        // activate a "drag area", here a small strip (roughly double text height)
        // at the top of the window, ie. the window can be moved by
        // dragging a spot near the window's top
        SetWindowDragArea(0, 5, INT_MAX, 5 + 2*int(FONT_SIZE));
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
    
    // If we are a transparent HUD-like window then we draw 3 lines that look
    // a bit like a head...so people know where to drag the window to move it
    if (HasWindowDragArea()) {
        ImGui::SameLine();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos_start = ImGui::GetCursorPos();
        float x_end = ImGui::GetContentRegionAvail().x - 75;
        for (int i = 0; i < 3; i++) {
            draw_list->AddLine(pos_start, {x_end, pos_start.y}, IM_COL32(0xa0, 0xa0, 0xa0, 255), 1.0f);
            pos_start.y += 5;
        }
    }
    
    // Button with fixed width 30 and standard height
    // to pop out the window in an OS window
    static float btnWidth = ImGui::CalcTextSize(ICON_FA_WINDOW_MAXIMIZE).x + 5;
    const bool bBtnPopOut = !IsPoppedOut();
    const bool bBtnPopIn  = IsPoppedOut() || IsInVR();
    const bool bBtnVR     = vr_is_enabled && !IsInVR();
    int numBtn = bBtnPopOut + bBtnPopIn + bBtnVR;
    if (numBtn > 0) {
        // Setup colors for window sizing buttons
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_ScrollbarGrabActive)); // dark gray
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);                           // transparent
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_ScrollbarGrab)); // lighter gray

        if (bBtnVR) {
            // Same line, but right-alinged
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_EXTERNAL_LINK_SQUARE_ALT, "Move into VR"))
                nextWinPosMode = xplm_WindowVR;
            --numBtn;
        }
        if (bBtnPopIn) {
            // Same line, but right-alinged
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - (numBtn * btnWidth));
            if (ImGui::ButtonTooltip(ICON_FA_WINDOW_RESTORE, "Move back into X-Plane"))
                nextWinPosMode = xplm_WindowPositionFree;
            --numBtn;
        }
        if (bBtnPopOut) {
            // Same line, but right-alinged
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

    if (ImGui::TreeNode("Contrib UX & Modern Cockpit Suite")) {
        ImGui::TextMD("# Contrib UX Showcase");
        ImGui::TextMD("Turnkey extensions from `ImgWindow/contrib/` for modern cockpit plugins:");
        
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
                            "AP Master: <pill color=#1a252f fg=#555555>ENGAGED</pill> <pill color=#e67e22>DISARMED</pill>",
                            "AP Master: <pill color=#2ecc71>ENGAGED</pill> <pill color=#1a252f fg=#555555>DISARMED</pill>");

        ImGui::SameLine();
        ImGui::ButtonLink("Official ImgWindow Docs");
        ImGui::TimedTooltip::MD::Text("Clickable button that demonstrates the native OS hand cursor and stationary tooltip.");

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
        ImGui::TextMD("### Markdown & Styling Tags (Contrib)");
        ImGui::TextMD("Direct color tag: <color #2b65ec>Ocean Blue via `<color #2b65ec>` tag</color>");
        ImGui::TextMD("Status pills: <pill color=#2ecc71>ONLINE</pill>  <pill color=#e74c3c>FAIL</pill>  <pill color=#f39c12>ARMED</pill>");
        ImGui::TextMD("Keybinding badges: <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>F12</kbd>");
        ImGui::TextMD("<highlight>Highlight tags replace manual PushStyleColor / PopStyleColor boilerplate!</highlight>");
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
        ImGui::TextMD("#### Modern Cockpit Switch (Contrib):");
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

ImGeeShowcaseWidget::ImGeeShowcaseWidget(int width, int height,
                                         XPLMWindowDecoration decoration,
                                         XPLMWindowLayer layer) :
    ImgWindow(WindowGeometry::Centered(width, height), decoration, layer, true /* cursors */)
{
    SetWindowTitle("ImGee Cockpit Showcase");
    SetWindowResizingLimits(520, 420, 2560, 1440);
    scratchpadText = "### Live Flight Briefing\n"
                     "* Status: <pill color=#2ecc71>CLEARED</pill>\n"
                     "* Departure: **KSEA** Rwy 16L\n"
                     "* Route: `SEA8 JAWBN Q29`\n"
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

ImGeeShowcaseWidget::~ImGeeShowcaseWidget() = default;

void ShowImGeeShowcaseWindow()
{
    ImGuiContext* prevContext = ImGui::GetCurrentContext();
    if (!s_pShowcaseWidget) {
        s_pShowcaseWidget = std::make_shared<ImGeeShowcaseWidget>(680, 580);
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
    // 1. Container / Window Chrome: Baseline 1.0x scale (rock-solid, non-twitching controls)
    ImGui::SetWindowFontScale(1.0f);

    // Title on left
    ImGui::TextMD("# ImGee UX & Modern Cockpit Suite");

    // Steppers + Slider (1.67x wider) + ResetButton + CursorToggle suite (fixed width in chrome)
    const float zoom_suite_w = 260.0f;
    const float min_offset = ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x;
    const float target_offset = ImGui::GetWindowWidth() - zoom_suite_w - ImGui::GetStyle().WindowPadding.x;
    ImGui::SameLine(ImMax(min_offset, target_offset));

    // [-] Stepper: repeating button with (n % 4 == 0) gating (hold to step smoothly)
    ImGui::PushButtonRepeat(true);
    bool dec_clicked = ImGui::Button("-##zoom_dec", ImVec2(22.0f, 0.0f));
    ImGui::PopButtonRepeat();

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
            _fontAdjust = ImMax(0.50f, _fontAdjust - 0.05f);
        }
    }
    ImGui::TimedTooltip::MD::Text("Zoom Out (-5%%)\n\n*Click or hold down to step smoothly*");

    ImGui::SameLine(0.0f, 3.0f);

    // Track slider: Range 50% to 200%, 1.67x wider (150px) for spacious text and thumb breathing room
    float pct = _fontAdjust * 100.0f;
    ImGui::SetNextItemWidth(150.0f);
    if (ImGui::SliderFloat("##zoom_slider", &pct, 50.0f, 200.0f, "Zoom %.0f%%")) {
        _fontAdjust = pct / 100.0f;
    }
    // Sensitive mouse wheel (2% per click on macOS instead of chunky 5%+)
    if (ImGui::AdjustItemOnMouseWheel(&_fontAdjust, 0.50f, 2.00f, 0.02f)) {
        // wheel adjusted
    }
    ImGui::TimedTooltip::MD::Text("Interactive UI Zoom: roll mouse wheel (2%%) or drag thumb.");

    ImGui::SameLine(0.0f, 3.0f);

    // [+] Stepper: repeating button with (n % 4 == 0) gating (hold to step smoothly)
    ImGui::PushButtonRepeat(true);
    bool inc_clicked = ImGui::Button("+##zoom_inc", ImVec2(22.0f, 0.0f));
    ImGui::PopButtonRepeat();

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
            _fontAdjust = ImMin(2.00f, _fontAdjust + 0.05f);
        }
    }
    ImGui::TimedTooltip::MD::Text("Zoom In (+5%%)\n\n*Click or hold down to step smoothly*");

    ImGui::SameLine(0.0f, 3.0f);

    // Vector Reset button with 1-click A/B toggle memory to prior zoom
    bool is_modified = std::abs(_fontAdjust - 1.0f) > 0.001f;
    bool can_restore = !is_modified && std::abs(_lastZoom - 1.0f) > 0.001f;
    if (ImGui::ResetButton("##zoom_reset", is_modified, 0.0f, false, nullptr, can_restore)) {
        if (is_modified) {
            _lastZoom = _fontAdjust;
            _fontAdjust = 1.0f;
        } else if (can_restore) {
            _fontAdjust = _lastZoom;
        }
    }
    if (is_modified) {
        ImGui::TimedTooltip::MD::Text("Reset zoom back to **100%% baseline**.");
    } else if (can_restore) {
        ImGui::TimedTooltip::MD::Text("1-click toggle back to prior **%.0f%% zoom**.", _lastZoom * 100.0f);
    } else {
        ImGui::TimedTooltip::MD::Text("Zoom is at **100%% baseline**.");
    }

    // Cursor Handling Toggle Button (separated by 8px gap)
    ImGui::SameLine(0.0f, 8.0f);
    bool imgui_cursors = GetImgCursorHandling();
    if (ImGui::Button(imgui_cursors ? ICON_FA_MOUSE_POINTER "##cursor_toggle" : ICON_FA_MOUSE "##cursor_toggle", ImVec2(24.0f, 0.0f))) {
        SetImgCursorHandling(!imgui_cursors);
    }
    ImGui::TimedTooltip::MD::Text(imgui_cursors
        ? "**ImGui Dynamic Cursors:** <pill color=#2ecc71>ACTIVE</pill>\n\nClick to switch to standard X-Plane hardware cursor."
        : "**ImGui Dynamic Cursors:** <pill color=#7f8c8d>DISABLED</pill>\n\nClick to enable ImGui contextual mouse cursors (hand, resize, text).");

    ImGui::TextDisabled("Dedicated, non-scrollable reference showcase for avionics controls, dials, and CommonMark.");
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
                                "Avionics: <pill color=#1a252f fg=#555555>ONLINE</pill> <pill color=#7f8c8d>OFFLINE</pill>",
                                "Avionics: <pill color=#2ecc71>ONLINE</pill> <pill color=#1a252f fg=#555555>OFFLINE</pill>");
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
            ImGui::TextMD("**Inline Styles:** **bold**, *italic*, ~~strikethrough~~, ++underline++, and `monospace`.");
            ImGui::TextMD("**Hotkeys:** <kbd>AP DISC</kbd> or <kbd>Z</kbd>   |   <kdb>Ctrl</kdb>+<kdb>Shift</kdb>+<kdb>F12</kdb>");
            ImGui::TextMD("**Release:** Core: <badge>v2.3.0</badge>   Platform: <badge>macOS Universal</badge>");

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

            // 1-Click Formatting Toolbar with Smart Selection Wrapping:
            if (ImGui::Button("H1")) {
                wrap_or_insert("# ", "", "Heading 1", true);
            }
            ImGui::TimedTooltip::MD::Text("Prepend `# ` heading to current line or selection");

            ImGui::SameLine();
            if (ImGui::Button("H2")) {
                wrap_or_insert("## ", "", "Heading 2", true);
            }
            ImGui::TimedTooltip::MD::Text("Prepend `## ` heading to current line or selection");

            ImGui::SameLine();
            if (ImGui::Button(" B ")) {
                wrap_or_insert("**", "**", "bold text", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `**bold**` markup");

            ImGui::SameLine();
            if (ImGui::Button(" I ")) {
                wrap_or_insert("*", "*", "italic text", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `*italic*` markup");

            ImGui::SameLine();
            if (ImGui::Button(" U ")) {
                wrap_or_insert("<u>", "</u>", "underlined text", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `<u>underline</u>` tags");

            ImGui::SameLine();
            if (ImGui::Button("* List")) {
                wrap_or_insert("* ", "", "Item description", true);
            }
            ImGui::TimedTooltip::MD::Text("Prepend bullet `* ` to current line or selection");

            ImGui::SameLine();
            if (ImGui::Button("Pill")) {
                wrap_or_insert("<pill color=#2ecc71>", "</pill>", "ONLINE", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `<pill color=...>` status capsule");

            ImGui::SameLine();
            if (ImGui::Button("Badge")) {
                wrap_or_insert("<badge>", "</badge>", "v2.3.0", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `<badge>` release chip");

            ImGui::SameLine();
            if (ImGui::Button("Key")) {
                wrap_or_insert("<kbd>", "</kbd>", "Alt", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in `<kbd>` keycap");

            ImGui::SameLine();
            if (ImGui::Button("Code")) {
                wrap_or_insert("\n```cpp\n", "\n```\n", "void FlightPlan() {\n}\n", false);
            }
            ImGui::TimedTooltip::MD::Text("Wrap selection in syntax-highlighted code block with [Copy] button");

            ImGui::SameLine();
            if (ImGui::Button("---")) {
                wrap_or_insert("\n---\n", "", "", false);
            }
            ImGui::TimedTooltip::MD::Text("Insert CommonMark 4.1 thematic break divider (---)");

            ImGui::SameLine();
            ImGui::TextDisabled("|");

            ImGui::SameLine();
            if (ImGui::Button("Clear")) {
                scratchpadText.clear();
            }
            ImGui::TimedTooltip::MD::Text("Clears the scratchpad editor buffer");

            ImGui::SameLine();
            if (ImGui::Button("Reset Sample")) {
                scratchpadText = "### Live Flight Briefing\n"
                                 "* Status: <pill color=#2ecc71>CLEARED</pill>\n"
                                 "* Departure: **KSEA** Rwy 16L\n"
                                 "* Route: `SEA8 JAWBN Q29`\n"
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
            ImGui::TimedTooltip::MD::Text("Restores the flight briefing sample");

            ImGui::Spacing();

            // Side-by-side split panels:
            float avail_w = ImGui::GetContentRegionAvail().x;
            float pane_w = ImMax(100.0f, (avail_w - ImGui::GetStyle().ItemSpacing.x) * 0.5f);
            float avail_h = ImGui::GetContentRegionAvail().y;
            float pane_h = ImMax(200.0f, avail_h - 10.0f);

            // Left Pane: Raw Editor (with soft word-wrap and tab input!)
            ImGui::BeginChild("ScratchpadInputPane", ImVec2(pane_w, pane_h), true);
            ImGui::TextDisabled("Raw Input (Markdown):");
            ImGui::InputTextMultiline("##editor", &scratchpadText, ImVec2(-FLT_MIN, pane_h - 35.0f),
                                      ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_AllowTabInput);
            ImGui::EndChild();

            ImGui::SameLine();

            // Right Pane: Live Rendered Output (with auto soft-wrapping!)
            ImGui::BeginChild("ScratchpadPreviewPane", ImVec2(pane_w, pane_h), true);
            ImGui::TextDisabled("Live Rendered Output:");
            ImGui::Separator();
            if (scratchpadText.empty()) {
                ImGui::TextDisabled("(Editor buffer is empty. Type on the left or click Reset Sample...)");
            } else {
                ImGui::TextWrappedMD(scratchpadText.c_str());
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // 3. Restore window chrome scale for footer
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Close")) {
        SetVisible(false);
    }
}

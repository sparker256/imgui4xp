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
#include "contrib.h"
#include <vector>

// Configure one-time setup like fonts
void configureImgWindow();
// Cleanup one-time setup
void cleanupAfterImgWindow();

inline ImFont* Font1 = nullptr;
inline ImFont* Font2 = nullptr;
inline ImFont* Font3 = nullptr;
inline ImFont* Font4 = nullptr;
inline ImFont* Font5 = nullptr;
inline ImFont* Font6 = nullptr;
inline ImFont* Font7 = nullptr;
inline ImFont* Font8 = nullptr;
inline ImFont* Font9 = nullptr;
inline ImFont* FontBold = nullptr;
inline ImFont* FontMono = nullptr;

// Dedicated fixed-size, non-scrollable window showcasing ImGee / contrib UX suite
class ImGeeShowcaseWidget: public ImgWindow {
public:
    ImGeeShowcaseWidget(int width = 680, int height = 580,
                        XPLMWindowDecoration decoration = xplm_WindowDecorationRoundRectangle,
                        XPLMWindowLayer layer = xplm_WindowLayerFloatingWindows);
    ~ImGeeShowcaseWidget() override;

protected:
    ImGuiWindowFlags_ beforeBegin() override {
        return ImGuiWindowFlags_None;
    }

    // Dynamic UI Zoom multiplier (Responsive UI Scaling Math)
    float GetExtensionScaleFactor() const override { return _fontAdjust; }

    void buildInterface() override;

private:
    // Tab 1 state: Knobs, Sliders & Procedural Dials
    float       headingBug      = 180.0f;
    float       altitudeTarget  = 5500.0f;
    float       verticalSpeed   = 0.0f;
    float       throttlePct     = 0.65f;
    int         navSourceIdx    = 0;
    float       fuelRemaining   = 0.74f;
    float       holdTimerSec    = 42.0f;
    const float holdTimerMaxSec = 60.0f;
    char        transponderCode[5] = "4721";

    // Tab 2 state: Switches & Toggles
    bool        avionicsMaster  = true;
    bool        flightDirector  = true;
    bool        pitotHeat       = false;
    bool        yawDamper       = false;

    // Tab 3 state: Typography & Markdown
    std::string pilotCallsign   = "N172SP";

    // Tab 4 state: Live Scratchpad & Toolbar Demo
    std::string scratchpadText;
    bool        has_active_selection = false;
    int         cached_sel_start     = -1;
    int         cached_sel_end       = -1;
    int         cached_cur_pos       = -1;
    bool        syncScroll           = false;

    // File Dialog & Picker State (imgui_file_dialog.h integration)
    std::string currentNoteFilename  = "flight_briefing.md";
    std::string noteStatusMsg;
    float       noteStatusTimer      = 0.0f;
    bool        showOpenDialog       = false;
    bool        showSaveDialog       = false;
    std::string dialogFilePath;

    // Shields.io & Badge Generator Modal State
    bool        showShieldModal      = false;
    char        shieldLabel[64]      = "X-Plane";
    char        shieldMessage[64]    = "12.00+";
    int         shieldColorIdx       = 0; // 0=blue, 1=green, 2=orange, 3=red, 4=purple
    int         shieldStyleIdx       = 0; // 0=Shields.io Image, 1=Native ImGee Badge, 2=Native ImGee Pill

    void LoadNote(const std::string& path);
    void SaveNote(const std::string& path);

    // Window-level UI zoom factor (Responsive UI Scaling Math)
    float       _fontAdjust     = 1.0f;
    float       _lastZoom       = 1.0f;
    int         _decRepeatCount = 0;
    int         _incRepeatCount = 0;
    float       _sliderStartZoom = 1.0f;
    std::vector<ImGui::MarkdownTableState> pendingTableStates;

    struct WindowRectSnapshot {
        int  l = 0;
        int  t = 0;
        int  r = 0;
        int  b = 0;
        bool valid = false;
    };
    WindowRectSnapshot _baselineRect;
    WindowRectSnapshot _priorZoomRect;

    void RestoreWindowRect(const WindowRectSnapshot& rect);
};

// Backwards-compatible alias for showcase window
typedef ImGeeShowcaseWidget ContribShowcaseWidget;

// Opens or focuses the dedicated ImGee Showcase Window
void ShowImGeeShowcaseWindow();
inline void ShowContribShowcaseWindow() { ShowImGeeShowcaseWindow(); }
inline std::shared_ptr<ImGeeShowcaseWidget> s_pShowcaseWidget = nullptr;

// Our own class defining our own UI
class ImguiWidget: public ImgWindow {
public:
    // texture ID and size of the image we want to show
    static ImTextureID  image_id;
    static ImVec2       image_size;
protected:
    // Counter for the number of windows opened
    static int      num_win;
    // I am window number...
    const int       myWinNum;
    // Note to myself that a change of window mode is requested
    XPLMWindowPositioningMode nextWinPosMode = -1;
    // Our flight loop callback in case we need one
    XPLMFlightLoopID flId = nullptr;
    // Values in node "Buttons" / "Checkboxes"
    bool makeRed = false;
    int         radioChoice = 1;
    // Values in node "Sliders"
    float       sliderVal1  = 250.0f;
    float       sliderVal2  = 0.0f;
    int         sliderVal3  = 50;
    float       sliderAngle = 0.0f;
    // Values in node "ComboBox"
    int         choice1     = 1;
    int         choice2     = 1;
    // Values in node "Input"
    std::string userText;
    int         userI1 = 123;
    int         userI2 = 1234;
    // Values in node "List"
    std::vector<const char*> listContent;
    int         listSelItem = 0;
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
    // Auto-centers on primary display using WindowGeometry::Centered(width, height)
    ImguiWidget(int width, int height,
                XPLMWindowDecoration decoration = xplm_WindowDecorationRoundRectangle,
                XPLMWindowLayer layer = xplm_WindowLayerFloatingWindows);
    // Modern WindowGeometry constructor
    ImguiWidget(const WindowGeometry& geometry,
                XPLMWindowDecoration decoration = xplm_WindowDecorationRoundRectangle,
                XPLMWindowLayer layer = xplm_WindowLayerFloatingWindows);
    // Legacy constructor (retained for backward compatibility)
    ImguiWidget(int left, int top, int right, int bot,
                XPLMWindowDecoration decoration = xplm_WindowDecorationRoundRectangle,
                XPLMWindowLayer layer = xplm_WindowLayerFloatingWindows);
    ~ImguiWidget() override;
protected:
    // Main function: creates the window's UI
    void buildInterface() override;

    // flight loop callback for stuff we cannot do during drawing callback
    static float cbFlightLoop(
        float                inElapsedSinceLastCall,
        float                inElapsedTimeSinceLastFlightLoop,
        int                  inCounter,
        void*                inRefcon);
};

//

#endif /* SRC_IMGUIWIDGET_H_ */

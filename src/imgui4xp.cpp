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

#define VERSION_NUMBER "1.15 build " __DATE__ " " __TIME__

// All our headers combined
#include "imgui4xp.h"

/// Our "standard" window size
constexpr int WIN_WIDTH     = 800;      ///< window width
constexpr int WIN_HEIGHT    = 450;      ///< window height
constexpr int WIN_PAD       =  75;      ///< distance from left and top border
constexpr int WIN_COLL_OFS  =  30;      ///< offset of collated windows

// --- Global Variables ---

// Is VR enabled?
bool vr_is_enabled = false;

// the ImGui windows we are going to create
// managed in the form of smart pointers.
ImgWindowSPtrVecTy gWndList;

/// Calculate window's standard coordinates on X-Plane's Main Monitor
void CalcWinCoords (int& left, int& top, int& right, int& bottom)
{
    const WindowGeometry geom = WindowGeometry::FromMainMonitorTopLeft(
        WIN_PAD, WIN_PAD, WIN_WIDTH, WIN_HEIGHT);
    left   = geom.left;
    top    = geom.top;
    right  = geom.right;
    bottom = geom.bottom;
}

// Command references
XPLMCommandRef s_cmdToggleDemo = nullptr;
#if defined(IMGEE_VERSION)
XPLMCommandRef s_cmdToggleShowcase = nullptr;
#endif
XPLMCommandRef s_cmdGatherWindows = nullptr;

// Plugin menu and command item indices
XPLMMenuID g_hPluginMenu = nullptr;
int g_menuItemDemo = -1;
int g_menuItemShowcase = -1;
int g_menuItemGather = -1;

// Lightweight flight loop to keep Plugins menu checkmarks synced when the user
// closes a window via X-Plane's native close box or a popped-out OS window's
// close button (which hide the XPLMWindowID without notifying the plugin).
static XPLMFlightLoopID s_flMenuSync = nullptr;
static bool s_flMenuSyncActive = false;
static int s_lastDemoCheck = -1;
static int s_lastShowcaseCheck = -1;

static float MenuSyncFlightLoopCB(float /*inElapsedSinceLastCall*/,
                                  float /*inElapsedTimeSinceLastFlightLoop*/,
                                  int   /*inCounter*/,
                                  void* /*inRefcon*/)
{
    UpdateMenuCheckmarks();
    const bool demoVis = (s_pStarterWidget && s_pStarterWidget->GetVisible());
#if defined(IMGEE_VERSION)
    const bool showcaseVis = (s_pShowcaseWidget && s_pShowcaseWidget->GetVisible());
#else
    const bool showcaseVis = false;
#endif
    if (demoVis || showcaseVis) {
        return 0.15f;
    }
    s_flMenuSyncActive = false;
    return 0.0f;
}

/// Creates another window right inside the list
void AddWindow (XPLMWindowDecoration decoration,
                XPLMWindowLayer layer)
{
    // Create the window using ImgWindow's modern WindowGeometry constructor,
    // anchored relative to X-Plane's Main Monitor usable top-left corner.
    auto w = std::make_shared<ImguiWidget>(
        WindowGeometry::FromMainMonitorTopLeft(WIN_PAD, WIN_PAD, WIN_WIDTH, WIN_HEIGHT, decoration),
        decoration,
        layer);
    gWndList.emplace_back(w);
    s_pStarterWidget = w;
    UpdateMenuCheckmarks();
}

void UpdateMenuCheckmarks()
{
    const bool demoVis = (s_pStarterWidget && s_pStarterWidget->GetVisible());
#if defined(IMGEE_VERSION)
    const bool showcaseVis = (s_pShowcaseWidget && s_pShowcaseWidget->GetVisible());
#else
    const bool showcaseVis = false;
#endif

    if (g_hPluginMenu) {
        if (g_menuItemDemo >= 0) {
            const int check = demoVis ? xplm_Menu_Checked : xplm_Menu_Unchecked;
            if (check != s_lastDemoCheck) {
                XPLMCheckMenuItem(g_hPluginMenu, g_menuItemDemo, check);
                s_lastDemoCheck = check;
            }
        }
#if defined(IMGEE_VERSION)
        if (g_menuItemShowcase >= 0) {
            const int check = showcaseVis ? xplm_Menu_Checked : xplm_Menu_Unchecked;
            if (check != s_lastShowcaseCheck) {
                XPLMCheckMenuItem(g_hPluginMenu, g_menuItemShowcase, check);
                s_lastShowcaseCheck = check;
            }
        }
#endif
    }

    if ((demoVis || showcaseVis) && s_flMenuSync && !s_flMenuSyncActive) {
        XPLMScheduleFlightLoop(s_flMenuSync, 0.15f, 1);
        s_flMenuSyncActive = true;
    }
}

void ToggleStarterWindow()
{
    ImGuiContext* prevContext = ImGui::GetCurrentContext();
    if (!s_pStarterWidget) {
        AddWindow();
        if (s_pStarterWidget) {
            s_pStarterWidget->SetVisible(true);
            s_pStarterWidget->BringWindowToFront();
        }
    } else {
        bool is_vis = s_pStarterWidget->GetVisible();
        s_pStarterWidget->SetVisible(!is_vis);
        if (!is_vis && !s_pStarterWidget->IsPoppedOut()) {
            s_pStarterWidget->BringWindowToFront();
        }
    }
    UpdateMenuCheckmarks();
    if (prevContext) {
        ImGui::SetCurrentContext(prevContext);
    }
}

void ToggleImGeeShowcaseWindow()
{
#if defined(IMGEE_VERSION)
    ImGuiContext* prevContext = ImGui::GetCurrentContext();
    if (!s_pShowcaseWidget) {
        ShowImGeeShowcaseWindow();
    } else {
        bool is_vis = s_pShowcaseWidget->GetVisible();
        s_pShowcaseWidget->SetVisible(!is_vis);
        if (!is_vis && !s_pShowcaseWidget->IsPoppedOut()) {
            s_pShowcaseWidget->BringWindowToFront();
        }
    }
    UpdateMenuCheckmarks();
    if (prevContext) {
        ImGui::SetCurrentContext(prevContext);
    }
#endif
}

void GatherAllWindows()
{
    ImGuiContext* prevContext = ImGui::GetCurrentContext();
    int left, top, right, bottom;
    CalcWinCoords(left, top, right, bottom);
    for (ImgWindowSPtrTy& pWnd : gWndList) {
        if (!pWnd) continue;
        pWnd->SetVisible(true);
        pWnd->SetWindowPositioningMode(xplm_WindowPositionFree);
        int curL, curT, curR, curB;
        pWnd->GetWindowGeometry(curL, curT, curR, curB);
        int w = curR - curL;
        int h = curT - curB;
        if (w <= 0 || h <= 0) {
            w = WIN_WIDTH;
            h = WIN_HEIGHT;
        }
        pWnd->SetWindowGeometry(left, top, left + w, top - h);
        pWnd->BringWindowToFront();
        left += WIN_COLL_OFS;
        top  -= WIN_COLL_OFS;
    }
    UpdateMenuCheckmarks();
    if (prevContext) {
        ImGui::SetCurrentContext(prevContext);
    }
}

static int CommandHandlerCB(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* /*inRefcon*/)
{
    if (inPhase == xplm_CommandBegin) {
        if (inCommand == s_cmdToggleDemo) {
            ToggleStarterWindow();
        }
#if defined(IMGEE_VERSION)
        else if (inCommand == s_cmdToggleShowcase) {
            ToggleImGeeShowcaseWindow();
        }
#endif
        else if (inCommand == s_cmdGatherWindows) {
            GatherAllWindows();
        }
    }
    return 0;
}

PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
    // Plugin details
    XPLMDebugString("imgui4xp: ver " VERSION_NUMBER  "\n");
    strcpy(outName, "imgui4xp: ver " VERSION_NUMBER);
    strcpy(outSig, "sparker.example.imgui4xp");
    strcpy(outDesc, "Imgui For X-Plane");

    // You probably want this on
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    
    // Register first-class X-Plane commands
    s_cmdToggleDemo = XPLMCreateCommand("imgui4xp/toggle_demo_window", "Toggle imgui4xp Demo Window (open/closed)");
    XPLMRegisterCommandHandler(s_cmdToggleDemo, CommandHandlerCB, 1, nullptr);

#if defined(IMGEE_VERSION)
    s_cmdToggleShowcase = XPLMCreateCommand("imgui4xp/toggle_showcase_window", "Toggle ImGee Showcase Window (open/closed)");
    XPLMRegisterCommandHandler(s_cmdToggleShowcase, CommandHandlerCB, 1, nullptr);
#endif

    s_cmdGatherWindows = XPLMCreateCommand("imgui4xp/gather_windows", "Gather All Windows to Screen");
    XPLMRegisterCommandHandler(s_cmdGatherWindows, CommandHandlerCB, 1, nullptr);

    // Create the menu for the plugin with first-class commands
    int my_slot = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "imgui4xp", nullptr, 0);
    g_hPluginMenu = XPLMCreateMenu("imgui4xp", XPLMFindPluginsMenu(), my_slot, nullptr, nullptr);
    g_menuItemDemo = XPLMAppendMenuItemWithCommand(g_hPluginMenu, "imgui4xp Demo Window", s_cmdToggleDemo);
#if defined(IMGEE_VERSION)
    g_menuItemShowcase = XPLMAppendMenuItemWithCommand(g_hPluginMenu, "ImGee Showcase Window", s_cmdToggleShowcase);
#endif
    g_menuItemGather = XPLMAppendMenuItemWithCommand(g_hPluginMenu, "Gather All Windows", s_cmdGatherWindows);

    // Initialize random number generator
    std::srand((unsigned)std::time(nullptr));

    return 1;
}

PLUGIN_API void XPluginStop(void) {
    if (s_flMenuSync) {
        XPLMDestroyFlightLoop(s_flMenuSync);
        s_flMenuSync = nullptr;
        s_flMenuSyncActive = false;
    }
    if (s_cmdToggleDemo) {
        XPLMUnregisterCommandHandler(s_cmdToggleDemo, CommandHandlerCB, 1, nullptr);
        s_cmdToggleDemo = nullptr;
    }
#if defined(IMGEE_VERSION)
    if (s_cmdToggleShowcase) {
        XPLMUnregisterCommandHandler(s_cmdToggleShowcase, CommandHandlerCB, 1, nullptr);
        s_cmdToggleShowcase = nullptr;
    }
#endif
    if (s_cmdGatherWindows) {
        XPLMUnregisterCommandHandler(s_cmdGatherWindows, CommandHandlerCB, 1, nullptr);
        s_cmdGatherWindows = nullptr;
    }
    if (g_hPluginMenu) {
        XPLMDestroyMenu(g_hPluginMenu);
        g_hPluginMenu = nullptr;
    }
    s_lastDemoCheck = -1;
    s_lastShowcaseCheck = -1;
    cleanupAfterImgWindow();
}

PLUGIN_API void XPluginDisable(void) {
    if (s_flMenuSync) {
        XPLMDestroyFlightLoop(s_flMenuSync);
        s_flMenuSync = nullptr;
        s_flMenuSyncActive = false;
    }
    // Destroy all window object in order to properly clean up.
    // (Can't use ImgWindow::SafeDelete here as that would wait for a
    //  flight loop callback, which won't be delivered any longer.
    //  Delete should be safe here as no rendering is taking place and will no longer.)
    gWndList.clear();
    s_pStarterWidget = nullptr;
#if defined(IMGEE_VERSION)
    s_pShowcaseWidget = nullptr;
#endif
    UpdateMenuCheckmarks();
    
    // Cleanup the general stuff
    cleanupAfterImgWindow();
}

PLUGIN_API int XPluginEnable(void) {
    // Some general ImGui setup
    configureImgWindow();

    if (!s_flMenuSync) {
        XPLMCreateFlightLoop_t flDef = {
            sizeof(flDef),
            xplm_FlightLoop_Phase_BeforeFlightModel,
            MenuSyncFlightLoopCB,
            nullptr,
        };
        s_flMenuSync = XPLMCreateFlightLoop(&flDef);
        s_flMenuSyncActive = false;
    }
    s_lastDemoCheck = -1;
    s_lastShowcaseCheck = -1;
    
    // Create a first window
    if (!s_pStarterWidget) {
        AddWindow();
    } else {
        s_pStarterWidget->SetVisible(true);
    }

#if defined(IMGEE_VERSION)
    // Pre-create showcase window in hidden state so it's ready on-demand without mid-frame allocations
    if (!s_pShowcaseWidget) {
        ImGuiContext* prevContext = ImGui::GetCurrentContext();
        s_pShowcaseWidget = std::make_shared<ImGeeShowcaseWidget>(680, 596);
        s_pShowcaseWidget->SetVisible(false);
        gWndList.push_back(s_pShowcaseWidget);
        if (prevContext) {
            ImGui::SetCurrentContext(prevContext);
        }
    }
#endif

    UpdateMenuCheckmarks();
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int inMessage, void * /*inParam*/) {
    
    switch (inMessage) {
        // Move all windows in or out of VR when VR mode changes
        case XPLM_MSG_ENTERED_VR:
            vr_is_enabled = true;
            // We don't move popped out windows into VR because we think
            // the user didn't want it inside the sim
            for (ImgWindowSPtrTy& pWnd: gWndList) {
                if (!pWnd->IsPoppedOut())
                    pWnd->SetWindowPositioningMode(xplm_WindowVR);
            }
            break;
            
        case XPLM_MSG_EXITING_VR:
            vr_is_enabled = false;
            // Geometry gets lost when moving into VR, so we need to re-position the window now
            int left, top, right, bottom;
            CalcWinCoords(left, top, right, bottom);
            for (ImgWindowSPtrTy& pWnd: gWndList) {
                // If we don't move popped out windows then we need to make
                // sure that we only move VR windows back into the sim,
                // so that a popped out window stays popped out
                if (pWnd->IsInVR()) {
                    pWnd->SetWindowPositioningMode(xplm_WindowPositionFree);
                    pWnd->SetWindowGeometry(left, top, right, bottom);
                    // move the next window a bit to the side and down to see all of them arranged
                    left += WIN_COLL_OFS; right  += WIN_COLL_OFS;
                    top  -= WIN_COLL_OFS; bottom -= WIN_COLL_OFS;
                }
            }
            break;
    }
}

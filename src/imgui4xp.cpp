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

#define VERSION_NUMBER "1.04 build " __DATE__ " " __TIME__


#include "XPLMDisplay.h"    // for window creation and manipulation
#include "XPLMGraphics.h"   // for window drawing
#include "XPLMDataAccess.h" // for the VR dataref
#include "XPLMPlugin.h"     // for XPLM_MSG_SCENERY_LOADED message
#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "../src/ImgWindow/ImgWindow.h"
#include "../src/ImgWindow/ImgFontAtlas.h"

#include "imgui4xp.h"
#include "imgui_starter_window.h"
#include <cstring>

/// Our "standard" window size
constexpr int WIN_WIDTH     = 800;      ///< window width
constexpr int WIN_HEIGHT    = 450;      ///< window height
constexpr int WIN_PAD       =  75;      ///< distance from left and top border

// --- Global Variables ---

// Is VR enabled?
bool vr_is_enabled = false;

// Pointer to the ImGui window we are going to create
ImguiWidget* imguiPtr = nullptr;



/// Calculate window's standard coordinates
void CalcWinCoords (int& left, int& top, int& right, int& bottom)
{
    // Screen coordinates
    int screenLeft, screenTop;
    XPLMGetScreenBoundsGlobal(&screenLeft, &screenTop, nullptr, nullptr);

    // Coordinates of our window
    left    = screenLeft    + WIN_PAD;
    right   = left          + WIN_WIDTH;
    top     = screenTop     - WIN_PAD;
    bottom  = top           - WIN_HEIGHT;
}

/// Callback function for menu
void CBMenu (void* /*inMenuRef*/, void* inItemRef)
{
    // Show window?
    if (inItemRef == (void*)1)
    {
        if (imguiPtr) {
            imguiPtr->SetVisible(true);
            // Reset the size to standard, makes sure it will show in a defined way
            int left, top, right, bottom;
            CalcWinCoords(left, top, right, bottom);
            imguiPtr->SetWindowGeometry(left, top, right, bottom);
        }
    }
}


PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
    // Plugin details
    XPLMDebugString("imgui4xp: ver " VERSION_NUMBER  "\n");
    strcpy(outName, "imgui4xp: ver " VERSION_NUMBER);
    strcpy(outSig, "sparker.example.imgui4xp");
    strcpy(outDesc, "Imgui For X-Plane");

    // You probably want this on
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
    
    // Create the menu for the plugin
    int my_slot = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "imgui4xp", NULL, 0);
    XPLMMenuID hMenu = XPLMCreateMenu("imgui4xp", XPLMFindPluginsMenu(), my_slot, CBMenu, NULL);
    XPLMAppendMenuItem(hMenu, "Show Window", (void*)1, 0);

	return 1;
}

PLUGIN_API void	XPluginStop(void) {
}

PLUGIN_API void XPluginDisable(void) {
    // Destroy the window object in order to properly clean up.
    // (Can't use ImgWindow::SafeDelete here as that would wait for a
    //  flight loop callback, which won't be delivered any longer.
    //  Delete should be safe here as no rendering is taking place and will no longer.)
    if (imguiPtr)
        delete imguiPtr;
    imguiPtr = nullptr;
}

PLUGIN_API int XPluginEnable(void) {
    // Some general ImGui setup
    configureImgWindow();
    
    // Create our window with standard coordinates
    if (!imguiPtr) {
        int left, top, right, bottom;
        CalcWinCoords(left, top, right, bottom);
        imguiPtr = new ImguiWidget(left, top, right, bottom, xplm_WindowDecorationRoundRectangle);
    }

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int inMessage, void * /*inParam*/) {
    
    switch (inMessage) {
        // Move the window in or out of VR when VR mode changes
        case XPLM_MSG_ENTERED_VR:
            vr_is_enabled = true;
            // We don't move popped out windows into VR because we think
            // the user didn't want it inside the sim
            if (imguiPtr &&
                !imguiPtr->IsPoppedOut())
                imguiPtr->SetWindowPositioningMode(xplm_WindowVR);
            break;
            
        case XPLM_MSG_EXITING_VR:
            vr_is_enabled = false;
            // If we don't move popped out windows then we need to make
            // sure that we only move VR windows back into the sim,
            // so that a popped out window stays popped out
            if (imguiPtr &&
                imguiPtr->IsInVR())
                imguiPtr->SetWindowPositioningMode(xplm_WindowPositionFree);
            break;
    }
}

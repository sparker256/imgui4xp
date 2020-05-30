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

int vr_is_enabled = 0;

std::shared_ptr<ImguiWidget> imguiPtr;

/// Callback function for menu
void CBMenu (void* /*inMenuRef*/, void* inItemRef)
{
    // Show window?
    if (inItemRef == (void*)1)
    {
        if (imguiPtr)
            imguiPtr->SetVisible(true);
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
    imguiPtr.reset();
}

PLUGIN_API int XPluginEnable(void) {
    int left, top, right, bot;
    XPLMGetScreenBoundsGlobal(&left, &top, &right, &bot);

    int width = 800;
    int height = 450;
    int pad = 75;
    int x = left + pad;
    int y = top - pad;
    // WindowDecoration decorate
    // WindowDecorationNone = 0
    // WindowDecorationRoundRectangle = 1
    // WindowDecorationSelfDecorated = 2
    // WindowDecorationSelfDecoratedResizable = 3
    int decorate = 1;
    configureImgWindow();
    imguiPtr = std::make_shared<ImguiWidget>(x, y, x + width, y - height, decorate);

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int inMessage, void * /*inParam*/) {
    
    switch (inMessage) {
        // Move the window in or out of VR when VR mode changes
        case XPLM_MSG_ENTERED_VR:
        case XPLM_MSG_EXITING_VR:
            vr_is_enabled = inMessage == XPLM_MSG_ENTERED_VR;
            if (imguiPtr)
                // ImguiWindow::moveForVR is what we need, but it is protected.
                // SetVisible in turn calls ImguiWindow::moveForVr,
                // even if the visibility status doesn't change
                imguiPtr->SetVisible(imguiPtr->GetVisible());
            break;
    }
}

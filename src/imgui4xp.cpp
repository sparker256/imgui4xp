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

#define VERSION_NUMBER "1.03 build " __DATE__ " " __TIME__


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

PLUGIN_API int XPluginStart(char * outName, char * outSig, char * outDesc) {
    // Plugin details
    XPLMDebugString("imgui4xp: ver " VERSION_NUMBER  "\n");
    strcpy(outName, "imgui4xp: ver " VERSION_NUMBER);
    strcpy(outSig, "sparker.example.imgui4xp");
    strcpy(outDesc, "Imgui For X-Plane");

    // You probably want this on
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

	return 1;
}

PLUGIN_API void	XPluginStop(void) {
}

PLUGIN_API void XPluginDisable(void) {
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

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, intptr_t inMessage, void * inParam) {
}
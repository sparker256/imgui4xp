/*
 * ImgWindow.h
 *
 * Integration for dear imgui into X-Plane.
 *
 * Copyright (C) 2018, Christopher Collins
*/

#ifndef IMGWINDOW_H
#define IMGWINDOW_H

#include "SystemGL.h"
#include "XOGLUtils.h"

#include <string>
#include <memory>

#include <XPLMDisplay.h>
#include <XPCProcessing.h>
#include <imgui.h>

#include "ImgFontAtlas.h"

/** ImgWindow is a Window for creating dear imgui widgets within.
 *
 * There's a few traps to be aware of when using dear imgui with X-Plane:
 *
 * 1) The Dear ImGUI coordinate scheme is inverted in the Y axis vs the X-Plane 
 *    (and OpenGL default) scheme. You must be careful if you're trying to 
 *    directly manipulate positioning of widgets rather than letting imgui 
 *    self-layout.  There are (private) functions in ImgWindow to do the 
 *    coordinate mapping.
 *
 * 2) The Dear ImGUI rendering space is only as big as the window - this means
 *    popup elements cannot be larger than the parent window.  This was
 *    unavoidable on XP11 because of how popup windows work and the possibility
 *    for negative coordinates (which imgui doesn't like).
 *
 * 3) There is no way to detect if the window is hidden without a per-frame
 *    processing loop or similar.
 *
 * @note It should be possible to map globally on XP9 & XP10 letting you run
 * popups as large as you need, or to use the ImGUI native titlebars instead of
 * the XP10 ones - source for this may be provided later, but could also be
 * trivially adapted from this one by adjusting the way the space is translated
 * and mapped in the DrawWindowCB and constructor.
 */
class
ImgWindow {
public:
    /** sFontAtlas is the global shared font-atlas.
     *
     * If you want to share fonts between windows, this needs to be set before
     * any dialogs are actually instantiated.  It will be automatically handed
     * over to the contexts as they're created.
     */
    static std::shared_ptr<ImgFontAtlas> sFontAtlas;

    virtual ~ImgWindow();

    /** SetVisible() makes the window visible after making the onShow() call.
     * It is also at this time that the window will be relocated onto the VR
     * display if the VR headset is in use.
     *
     * @param inIsVisible true to be displayed, false if the window is to be
     * hidden.
     */
    virtual void SetVisible(bool inIsVisible);

    /** GetVisible() returns the current window visibility.
     * @return true if the window is visible, false otherwise.
    */
    bool GetVisible() const;

protected:
    /** mFirstRender can be checked during buildInterface() to see if we're
     * being rendered for the first time or not.  This is particularly
     * important for windows that use Columns as SetColumnWidth() should only
     * be called once.
     *
     * There may be other times where it's advantageous to make specific ImGui
     * calls once and once only.
     */
    bool mFirstRender;

    /** Construct a window with the specified bounds
     *
     * @param left Left edge of the window's contents in global boxels.
     * @param top Top edge of the window's contents in global boxels.
     * @param right Right edge of the window's contents in global boxels.
     * @param bottom Bottom edge of the window's contents in global boxels.
     * @param decoration The decoration style to use (see notes)
     * @param layer the preferred layer to present this window in (see notes)
     *
     * @note The decoration should generally be one presented/rendered by XP -
     * the ImGui window decorations are very intentionally supressed by
     * ImgWindow to allow them to fit in with the rest of the simulator.
     *
     * @note The only layers that really make sense are Floating and Modal.  Do
     * not set VR layer here however unless the window is ONLY to be rendered
     * in VR.
     */
    ImgWindow(
        int left,
        int top,
        int right,
        int bottom,
        XPLMWindowDecoration decoration = xplm_WindowDecorationRoundRectangle,
        XPLMWindowLayer layer = xplm_WindowLayerFloatingWindows);

    /** SetWindowTitle sets the title of the window both in the ImGui layer and
     * at the XPLM layer.
     *
     * @param title the title to set.
     */
    void SetWindowTitle(const std::string &title);

    /** moveForVR() is an internal helper for moving the window to either it's
     * preferred layer or the VR layer depending on if the headset is in use.
     */
    void moveForVR();

    /** buildInterface() is the method where you can define your ImGui interface
     * and handle events.  It is called every frame the window is drawn.
     *
     * @note You must NOT delete the window object inside buildInterface() -
     * use SafeDelete() for that.
     */
    virtual void buildInterface() = 0;

    /** onShow() is called before making the Window visible.  It provides an
     * opportunity to prevent the window being shown.
     *
     * @note the implementation in the base-class is a null handler.  You can
     * safely override this without chaining.
     *
     * @return true if the Window should be shown, false if the attempt to show
     * should be suppressed.
     */
    virtual bool onShow();

    /** SafeDelete() can be used within buildInterface() to get the object to
     * self-delete once it's finished rendering this frame.
     */
    void SafeDelete();

private:
    static void DrawWindowCB(XPLMWindowID inWindowID, void *inRefcon);

    static int HandleMouseClickCB(
        XPLMWindowID inWindowID,
        int x, int y,
        XPLMMouseStatus inMouse,
        void *inRefcon);

    static void HandleKeyFuncCB(
        XPLMWindowID inWindowID,
        char inKey,
        XPLMKeyFlags inFlags,
        char inVirtualKey,
        void *inRefcon,
        int losingFocus);

    static XPLMCursorStatus HandleCursorFuncCB(
        XPLMWindowID inWindowID,
        int x, int y,
        void *inRefcon);

    static int HandleMouseWheelFuncCB(
        XPLMWindowID inWindowID,
        int x, int y,
        int wheel,
        int clicks,
        void *inRefcon);

    static int HandleRightClickFuncCB(
        XPLMWindowID inWindowID,
        int x, int y,
        XPLMMouseStatus inMouse,
        void *inRefcon);

    int HandleMouseClickGeneric(
        int x, int y,
        XPLMMouseStatus inMouse,
        int button = 0);

    void RenderImGui(ImDrawData *draw_data);

    void updateImgui();

    void updateMatrices();

    void boxelsToNative(int x, int y, int &outX, int &outY);

    void translateImguiToBoxel(float inX, float inY, int &outX, int &outY);

    void translateToImguiSpace(int inX, int inY, float &outX, float &outY);

    float mModelView[16], mProjection[16];
    int mViewport[4];
    bool mSelfDestruct;

    std::string mWindowTitle;

    XPLMWindowID mWindowID;
    ImGuiContext *mImGuiContext;
    GLuint mFontTexture;
    bool mIsInVR;

    int mTop;
    int mBottom;
    int mLeft;
    int mRight;

    XPLMWindowLayer mPreferredLayer;


};

#endif // #ifndef IMGWINDOW_H

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

#ifndef IMGUI4XP_H
#define IMGUI4XP_H


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

// Standard C/C++ header
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

// X-Plane SDK header
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMMenus.h"

// Dear ImGui and ImgWindow
#include "../src/imgui/imgui.h"
#include "../src/ImgWindow/ImgWindow.h"

// Our Window definition
#include "imgui_starter_window.h"

#endif // #ifndef IMGUI4XP_H

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

// (Open)GL
#include "SystemGL.h"

// Standard C/C++ header
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
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
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ImgWindow.h"

// Our Window definition
#include "imgui_starter_window.h"

/// Calculate window's standard coordinates
void CalcWinCoords (int& left, int& top, int& right, int& bottom);

#endif // #ifndef IMGUI4XP_H

I made changes to these two file to get it to build corectly on my system.
William R. Good (sparker)

***************************************************************

In ImgWindow.h I changed from.

#include "SysOpenGL.h"

to

// #include "SysOpenGL.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif


and also in ImgWindow.h

from

#include <XPCProcessing.h>
#include <imgui.h>

to

#include <XPLMProcessing.h>
#ifdef LINUX
#include "../src/imgui/imgui.h"
#else
#include "../../src/imgui/imgui.h"
#endif


*********************************************************************

In ImgWindow.cpp I changed from.

#include "SysOpenGL.h"
#include "XOGLUtils.h"

to

//#include "SysOpenGL.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
//#include "XOGLUtils.h"

*************************************************************************

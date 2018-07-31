# imgui4xp
Imgui Template For X-Plane 11 Also Showing How To Support Multi Platform From Linux

I am documenting the process I took to get a multiplatform stand alone plugin that supports imgui on X-Plane built on Ubuntu 16.04 LTS. My goal it to make it easier for the next developer to amcomplish the same task. How I plan to use this is to develop an idea on FlyWithLua beta using its new support for imgui and when developed far enough then convert it to a stand alone plugin with no need for lua. I hope someone will find this usfull. 

First clone or download this site and put the resulting imgui4xp folder wherever you like. 

Next you will need to download or clone the imgui GitHub site found at https://github.com/ocornut/imgui.

After you have un zipped or cloned copy following files

    imgui.cpp
    imgui.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imconfig.h (empty by default, user-editable)
    stb_rect_pack.h
    stb_textedit.h
    stb_truetype.h

To the imgui4xp/src/imgui folder.

Next you will need to download or clone the xsb_public GitHub site found at https://github.com/kuroneko/xsb_public.

After you have un zipped or cloned copy following files

	ImgWindow.cpp	
 	ImgWindow.h

To the imgui4xp/src/ImgWindow folder.

You will next need the current X-Plane SDK which can be found at https://developer.x-plane.com/sdk/plugin-sdk-downloads/.

On my system I have renamed the main SDK folder to XSDK and it resides in /opt so the full path is /opt/XSDK

Since I develop on Linux my IDE of choice is QT Creator which is installed from my package manager.

I have scripts and make files that alow me to click on one button on my desktop and automaticly build for all three platforms.

You will need a compiler to build the linux files and the following command will get that for you.

sudo apt-get install build-essential

To build for Windows on Ubuntu you will need to use this command. sudo apt-get install mingw-w64

To build the Mac files on Ubuntu you need to go to the osxcross GitHub site found here.

https://github.com/tpoechtrager/osxcross

And follow there instructions and then you will need to install the clang compiler using this command.

sudo apt-get install clang-3.8

With all this correctly installed you to should be able to build for all three platforms on Ubuntu.

# imgui4xp
Imgui Template For X-Plane 11 Also Showing How To Support Multi Platform From Linux

I am documenting the process I took to get a multiplatform stand alone plugin that supports imgui on X-Plane built on Ubuntu 18.04 LTS. My goal it to make it easier for the next developer to accomplish the same task. With this template you should be able to develop an idea on FlyWithLua NG using its new support for imgui and when developed far enough then convert it to a stand alone plugin with no need for lua. I hope someone will find this useful.  

Thanks to kuroneko on x-plane.org and his repository at https://github.com/xsquawkbox/xsb_public we have added the ability to select the font you want to use and also use the ImGui::SetWindowFontScale(1.00) function to set the size you want your fonts to be. You first pick the font you want for the project then using the ImGui::SetWindowFontScale(1.00) function you can change the size of the font on any line you chose.

First clone imgui4xp using this command "git clone https://github.com/sparker256/imgui4xp.git" and put the resulting imgui4xp folder wherever you like. I normally put myself in the folder where I want the repository folder to reside first. 

Since I develop on Linux my IDE of choice is QT Creator which is installed from my package manager.

I have scripts and make files that allow me to click on one button on my desktop and automatically build for all three platforms.

You will need a compiler to build the Linux files and the following command will get that for you.

sudo apt-get install build-essential

To build for Windows on Ubuntu you will need to use this command. sudo apt-get install mingw-w64

To build the Mac files on Ubuntu you need to go to the osxcross GitHub site found here.

https://github.com/tpoechtrager/osxcross

And follow there instructions and then you will need to install the clang compiler using this command.

sudo apt-get install clang-3.8

With all this correctly installed you to should be able to build for all three platforms on Ubuntu using Cmake.

I have now also added the ability of using Docker to build for all three platforms.

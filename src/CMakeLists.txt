set(CMAKE_CXX_STANDARD 14)

include_directories("/opt/XSDK/CHeaders/XPLM")
include_directories("/opt/XSDK/CHeaders/Wrappers")
include_directories("/opt/XSDK/CHeaders/Widgets")
include_directories("/opt/XSDK/CHeaders")

include_directories("../src")
include_directories("../src/ImgWindow")
include_directories("../src/imgui")


if(WIN32)
	link_directories("/opt/XSDK/Libraries/Win")
	link_directories("/usr/x86_64-w64-mingw32/lib")
elseif(APPLE)
	link_directories("/opt/XSDK/Libraries/Mac")
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1")


if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=0 -DIBM=1 -DLIN=0 -DCREATECOMPLETEEDITION -DWIN64 -DNDEBUG -D_WINDOWS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_MBCS -D_USRDLL -DCustomCommands_EXPORTS -DSIMDATA_EXPORTS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_EXTERNAL_ALLOCATOR -std=c++11 -fexceptions -fpermissive")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wshadow -Wfloat-equal -pedantic -Wall -Wno-unused -Wno-terminate -Wno-missing-field-initializers")
elseif(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=1 -DIBM=0 -DLIN=0 -DCREATECOMPLETEEDITION -mmacosx-version-min=10.10")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused -Wno-missing-field-initializers -fPIC -O3 -DMACOSX")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot /home/bill/X-Plane_OsxCrossBuild/osxcross/target/SDK/MacOSX10.11.sdk")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -std=c++11 -stdlib=libc++ -fexceptions -fpermissive -Wshadow -Wfloat-equal -pedantic")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=0 -DIBM=0 -DLIN=1 -DCREATECOMPLETEEDITION -D_REENTRANT -std=gnu++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fpermissive -O3 -Wall -Wno-unused -Wno-terminate -Wno-missing-field-initializers -W -fPIC")
endif()


# X-Plane plugin
add_library(imgui4xp SHARED imgui4xp.cpp imgui_starter_window.cpp imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp ImgWindow/ImgWindow.cpp)


if(WIN32)
    target_link_libraries(imgui4xp -static -static-libgcc -static-libstdc++ XPLM_64 opengl32)
elseif(APPLE)
	target_link_libraries(imgui4xp "-framework XPLM -mmacosx-version-min=10.10 -isysroot /home/bill/X-Plane_OsxCrossBuild/osxcross/target/SDK/MacOSX10.11.sdk -flat_namespace -undefined suppress -shared -static -static-libgcc -static-libstdc++ -undefined dynamic_lookup -single_module -framework IOKit -framework CoreFoundation -framework GLUT -framework OpenGL -framework Cocoa -ldl")
elseif(UNIX)
target_link_libraries(imgui4xp -static-libgcc -static-libstdc++ -fPIC)
endif()


set_target_properties(imgui4xp PROPERTIES PREFIX "")
set_target_properties(imgui4xp PROPERTIES OUTPUT_NAME "imgui4xp")
set_target_properties(imgui4xp PROPERTIES SUFFIX ".xpl")

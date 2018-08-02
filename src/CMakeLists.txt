set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fPIC -Wno-reorder")


include_directories("/opt/XSDK/CHeaders/XPLM")
include_directories("/opt/XSDK/CHeaders/Wrappers")
include_directories("/opt/XSDK/CHeaders/Widgets")
include_directories("/opt/XSDK/CHeaders")

include_directories("../src")
include_directories("../src/ImgWindow")
include_directories("../src/imgui")



# X-Plane plugin
add_library(imgui4xp SHARED imgui4xp.cpp
	imgui_starter_window.cpp
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	ImgWindow/ImgWindow.cpp
	
)
if(WIN32)
    target_link_libraries(imgui4xp
        -static
        -static-libgcc
        -static-libstdc++
        XPLM_64

    )
elseif(APPLE)
target_link_libraries(imgui4xp
#    ${CommonLibs}
#    avitab_env_xplane
    "-framework XPLM"
)
elseif(UNIX)
target_link_libraries(imgui4xp
    -static-libgcc
    -static-libstdc++
    -fPIC
#    ${CommonLibs}
)
endif()

set_target_properties(imgui4xp PROPERTIES PREFIX "")
set_target_properties(imgui4xp PROPERTIES OUTPUT_NAME "imgui4xp")
set_target_properties(imgui4xp PROPERTIES SUFFIX ".xpl")

# X-Plane stuff
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1")
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=1 -DIBM=0 -DLIN=0")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=0 -DIBM=0 -DLIN=1")
elseif(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPL=0 -DIBM=1 -DLIN=0 -D_CRT_SECURE_NO_WARNINGS /bigobj")
endif()

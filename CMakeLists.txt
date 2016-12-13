cmake_minimum_required(VERSION 2.8.9)
project(relwarb)

if (UNIX)
    set(platform_sources src/linux_relwarb.cpp)
    
    set(NO_WARN "-Wno-unused-variable -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "-Wall -Werror -DLINUX ${NO_WARN}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -DRELWARB_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

    set(platform_flag)
    set(libs GL X11)
elseif (WIN32)
    set(platform_sources src/win32_relwarb.cpp)

    set(NO_WARN "/wd4100 /wd4189 /wd4065 /wd4505 /wd4101 /wd4244 /wd4201 /wd4577")
    set(OTHER_FLAGS "/WX /W4 /FC /GR- /EHa- /nologo /DWIN32_LEAN_AND_MEAN /DWIN32 /D_WINDOWS /DRELWARB_DEBUG")
    set(CMAKE_CXX_FLAGS "${NO_WARN} ${OTHER_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Z7 /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /Ox")

    set(libs user32.lib gdi32.lib opengl32.lib)
    set(platform_flag WIN32)
endif()

set(sources 
    ${platform_sources}
    src/relwarb.cpp
    src/relwarb_opengl.cpp
    src/relwarb_utils.cpp
    src/relwarb_world_sim.cpp
    src/relwarb_renderer.cpp
    src/relwarb_debug.cpp)

set(headers 
    src/relwarb.h
    src/relwarb_defines.h
    src/relwarb_utils.h
    src/relwarb_opengl.h
    src/relwarb_math.h
    src/relwarb_renderer.h
    src/relwarb_debug.h)


add_executable(relwarb ${platform_flag} ${sources} ${headers})
target_link_libraries(relwarb ${libs})

set_property(TARGET relwarb PROPERTY CXX_STANDARD 14)
set_property(TARGET relwarb PROPERTY CXX_STANDARD_REQUIRED True)
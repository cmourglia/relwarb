cmake_minimum_required(VERSION 2.8.9)
project(relwarb)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)
# file(COPY ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    # DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rd/glfw)

if (UNIX)
    set(platform_sources src/linux_relwarb.cpp)

    set(NO_WARN "-Wno-unused-variable -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror ${NO_WARN}")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

    set(platform_flag)
    set(libs GL X11)
elseif (WIN32)
    set(platform_sources src/win32_relwarb.cpp)

    #set(NO_WARN "/wd4100 /wd4189 /wd4065 /wd4505 /wd4101 /wd4244 /wd4201 /wd4577")
    set(OTHER_FLAGS "/WX /W4 /FC /GR- /EHsc-")
    set(CMAKE_CXX_FLAGS "${OTHER_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Z7 /Od /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /Ox /MD")

    set(libs user32.lib gdi32.lib opengl32.lib)
    set(platform_flag WIN32)

    add_definitions(-DWIN32 -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

set(sources
    # ${platform_sources}
    src/relwarb_glfw.cpp
    src/relwarb.cpp
    src/relwarb_opengl.cpp
    src/relwarb_utils.cpp
    src/relwarb_world_sim.cpp
    src/relwarb_renderer.cpp
    src/relwarb_debug.cpp
    src/relwarb_entity.cpp
    src/relwarb_editor.cpp
    src/relwarb_parser.cpp
    src/relwarb_game.cpp)

set(headers
    src/relwarb.h
    src/relwarb_defines.h
    src/relwarb_utils.h
    src/relwarb_opengl.h
    src/relwarb_math.h
    src/relwarb_renderer.h
    src/relwarb_debug.h
    src/relwarb_world_sim.h
    src/relwarb_entity.h
    src/relwarb_input.h
    src/relwarb_editor.h
    src/relwarb_parser.h
    src/relwarb_game.h)


add_executable(relwarb ${platform_flag} ${sources} ${headers})
target_link_libraries(relwarb ${libs} glfw)
target_include_directories(relwarb PRIVATE 3rd/glfw/include)

set_property(TARGET relwarb PROPERTY CXX_STANDARD 14)
set_property(TARGET relwarb PROPERTY CXX_STANDARD_REQUIRED True)

if (WIN32)
    set_target_properties(relwarb PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(relwarb PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()
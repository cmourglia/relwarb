cmake_minimum_required(VERSION 2.8.9)
project(relwarb)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

set(sources 
    src/linux_relwarb.cpp
    src/relwarb.cpp
    src/relwarb_opengl.cpp
    src/relwarb_utils.cpp
    src/relwarb_world_sim.cpp)

set(headers 
    src/relwarb.h
    src/relwarb_defines.h
    src/relwarb_utils.h
    src/relwarb_opengl.h
    src/relwarb_math.h)


add_executable(relwarb ${sources} ${headers})
target_link_libraries(relwarb GL X11)

set_property(TARGET relwarb PROPERTY CXX_STANDARD 14)
set_property(TARGET relwarb PROPERTY CXX_STANDARD_REQUIRED True)
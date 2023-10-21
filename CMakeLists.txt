cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(TermCraft VERSION 1.0)

add_library(libs_module
    src/engine.cpp
    src/render/render.cpp
    src/controller/controller.cpp
    src/controller/camera.cpp
    src/world/world.cpp
    src/world/chunk.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(libs_module pthread)
endif()

target_compile_options(libs_module BEFORE PUBLIC "-O3")

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} libs_module)
# target_compile_options(${PROJECT_NAME} BEFORE PUBLIC "-O3")

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(TermCraft VERSION 1.0)

option(TC_CURSOR_INVIS "Make the terminal cursor invisible" ON)

add_library(libs_module
    src/engine.cpp
    src/render/render.cpp
    src/controller/controller.cpp
    src/controller/camera.cpp
    src/world/world.cpp
)

target_compile_options(libs_module BEFORE PUBLIC -fopenmp -O3)
target_compile_definitions(libs_module PRIVATE TC_CURSOR_INVIS)
message(Invisible\ cursor:\ ${TC_CURSOR_INVIS})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(libs_module pthread)
endif()

target_link_libraries(libs_module -fopenmp)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} libs_module)

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(TermCraft VERSION 1.0)

set(BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR})
message("BUILD_PATH is ${BUILD_PATH}")
add_compile_definitions(BUILD_PATH="${BUILD_PATH}")

file(COPY res/ DESTINATION res)

add_library(libs_module
    src/engine.cpp
    src/render/render.cpp
    src/render/draw_util.cpp
    src/render/fragment.cpp
    src/render/mesh.cpp
    src/render/texture.cpp
    src/render/tri.cpp
    src/render/vertex.cpp
    src/controller/controller.cpp
    src/controller/camera.cpp
    src/world/world.cpp
    src/world/mesh_util.cpp
    src/world/raycast_util.cpp
    src/world/block.cpp
    src/world/chunk.cpp
    src/world/spline.cpp
    src/stb.cpp
)

# use this for valgrind: -pg
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    message("Darwin Clang detected")
    target_compile_options(libs_module BEFORE PUBLIC -Xpreprocessor -fopenmp -O3)
else()
    target_compile_options(libs_module BEFORE PUBLIC -fopenmp -O3)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(libs_module pthread)
endif()

target_link_libraries(libs_module -fopenmp)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} libs_module)

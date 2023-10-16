cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(TermCraft VERSION 1.0)

set(SOURCE_FILES src/main.cpp src/engine.cpp src/render/render.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

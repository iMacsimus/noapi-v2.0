cmake_minimum_required(VERSION 3.8)
project(minapi)

set(CMAKE_CXX_STANDART 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS src/cpuvsps.h
            src/vsps_shader.h
            src/common_structures.h)
set(SOURCE src/main.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

add_library(litemath STATIC external/litemath/Image2d.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC external/litemath)
target_link_libraries(${PROJECT_NAME} PUBLIC litemath)
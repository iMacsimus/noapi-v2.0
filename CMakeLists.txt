cmake_minimum_required(VERSION 3.8)
project(noapi)

set(CMAKE_CXX_STANDART 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADERS include/noapi/cpuvsps.h
            include/noapi/common_structures.h
            include/noapi/IShader.h
            include/noapi/integer_sequence.h
            include/noapi/tuple_repr.h
            include/noapi/attribute_checker.h
            include/noapi/vsps_auto_interpolate.h
            include/noapi/vsps_auto_fetch.h
            include/noapi/choose.h
    )
set(SOURCE src/dummy.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include/)

add_library(litemath STATIC external/litemath/Image2d.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC external/litemath)
target_link_libraries(${PROJECT_NAME} PUBLIC litemath)

add_library(obj_loader STATIC examples/utils/obj_loader/Mesh.cpp examples/utils/obj_loader/Mesh.h)
target_include_directories(obj_loader PUBLIC external/litemath examples/utils/obj_loader)

add_executable(simple_triangle examples/simple_triangle.cpp)
target_link_libraries(simple_triangle PUBLIC ${PROJECT_NAME})

add_executable(textured_cube examples/textured_cube.cpp)
target_link_libraries(textured_cube PUBLIC ${PROJECT_NAME} obj_loader)

add_executable(shadowed_scene examples/shadowed_scene.cpp)
target_link_libraries(shadowed_scene PUBLIC ${PROJECT_NAME} obj_loader)

add_executable(auto_features examples/auto_features.cpp)
target_link_libraries(auto_features PUBLIC ${PROJECT_NAME} obj_loader)



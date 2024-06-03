cmake_minimum_required(VERSION 3.8)
project(noapi)
set(CMAKE_CXX_STANDART 14)

set(HEADERS 
      include/noapi/cpuvsps.h
      include/noapi/common_structures.h
      include/noapi/IShader.h
      include/noapi/Lerper.h
      include/noapi/vsps_clipping.h
) # end HEADERS
set(SOURCE 
      src/dummy.cpp
) # end SOURCE

add_library(litemath STATIC external/litemath/Image2d.cpp)
add_library(${PROJECT_NAME} STATIC ${SOURCE} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_include_directories(${PROJECT_NAME} PUBLIC external/litemath)
target_link_libraries(${PROJECT_NAME} PUBLIC litemath)

# obj_loader for examples
add_library(obj_loader STATIC 
              examples/utils/obj_loader/Mesh.cpp 
              examples/utils/obj_loader/Mesh.h
) # end obj_loader
target_include_directories(obj_loader PUBLIC external/litemath examples/utils/obj_loader)

# examples
add_executable(simple_triangle  
                examples/simple_triangle/simple_triangle.cpp
                examples/simple_triangle/simple_shader.cpp 
                examples/simple_triangle/simple_shader.h
) # end simple_triangle
target_link_libraries(simple_triangle PUBLIC ${PROJECT_NAME})

add_executable(textured_cube 
                examples/textured_cube/textured_cube.cpp 
                examples/textured_cube/textured_cube_alg.cpp
) # end textured_cube
target_link_libraries(textured_cube PUBLIC ${PROJECT_NAME} obj_loader)

add_executable(shadowed_scene   
                examples/shadowed_scene/shadowed_scene.cpp
                examples/shadowed_scene/tex_shader.cpp
                examples/shadowed_scene/tex_shader.h
                examples/shadowed_scene/sm_shader.cpp 
                examples/shadowed_scene/sm_shader.h
) # end shadowed_scene
target_link_libraries(shadowed_scene PUBLIC ${PROJECT_NAME} obj_loader)



# cmake minimum version required
cmake_minimum_required (VERSION 2.8)
# project information

project (VecExtract)

set (CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build")

include_directories(include/freeglut-3.2.1/include include/glew-2.1.0/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(vec_extract main.cpp)
target_link_libraries(vec_extract glew32d glew32sd)
add_custom_command(TARGET vec_extract 
                   POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglutd.dll ${CMAKE_BINARY_DIR}/Debug/
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32d.dll ${CMAKE_BINARY_DIR}/Debug/)
# cmake minimum version required
cmake_minimum_required (VERSION 2.8)
# project information

project (TestSSB)

set (CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build")

include_directories(include/freeglut-3.2.1/include include/glew-2.1.0/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(ssb_align src/ssb_align.cpp)
target_link_libraries(ssb_align glew32d glew32sd)
add_custom_command(TARGET ssb_align 
                   POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglutd.dll ${CMAKE_BINARY_DIR}/Debug/
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32d.dll ${CMAKE_BINARY_DIR}/Debug/)
				   
add_executable(ssb_struct src/ssb_struct.cpp)
target_link_libraries(ssb_struct glew32d glew32sd)
add_custom_command(TARGET ssb_struct 
                   POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglutd.dll ${CMAKE_BINARY_DIR}/Debug/
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32d.dll ${CMAKE_BINARY_DIR}/Debug/)
				   
add_executable(ssb_array src/ssb_array.cpp)
target_link_libraries(ssb_array glew32d glew32sd)
add_custom_command(TARGET ssb_array 
                   POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/freeglutd.dll ${CMAKE_BINARY_DIR}/Debug/
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32d.dll ${CMAKE_BINARY_DIR}/Debug/)
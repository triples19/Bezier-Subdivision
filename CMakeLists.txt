cmake_minimum_required(VERSION 3.10)
project(OpenGL-Demo)

set(CMAKE_CXX_STANDARD 17)

# find_library(glfw3 glfw ${PROJECT_SOURCE_DIR}/thirdparty/libglfw3.a)

add_subdirectory(thirdparty)
add_subdirectory(src)

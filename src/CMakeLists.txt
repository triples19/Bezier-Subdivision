file(GLOB SRC *.hpp *.h *.cpp *.c)

add_executable(Bezier-Subdivition ${SRC})
target_include_directories(thirdparty PUBLIC ../thirdparty)

if (WIN32)
target_link_libraries(Bezier-Subdivition thirdparty ${PROJECT_SOURCE_DIR}/thirdparty/libglfw3.a -static-libgcc -static-libstdc++)
elseif (APPLE)
target_link_libraries(Bezier-Subdivition thirdparty ${PROJECT_SOURCE_DIR}/thirdparty/libglfw.3.dylib)
endif()

add_custom_command(TARGET Bezier-Subdivition PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/shaders/)
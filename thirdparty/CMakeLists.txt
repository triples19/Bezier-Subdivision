file(GLOB THIRD_PARTY imgui/*)

add_library(thirdparty ${THIRD_PARTY})
target_link_libraries(thirdparty ${PROJECT_SOURCE_DIR}/thirdparty/libglfw3.a)
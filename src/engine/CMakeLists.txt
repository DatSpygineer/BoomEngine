cmake_minimum_required(VERSION 3.25)

file(GLOB BOOM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(BoomEngine SHARED ${BOOM_SOURCES})
target_link_libraries(BoomEngine PUBLIC SDL2::SDL2 OpenGL::GL GLEW::GLEW)
target_include_directories(BoomEngine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_definitions(BoomEngine PRIVATE BOOM_DLL_EXPORT)
set_target_properties(BoomEngine PROPERTIES OUTPUT_NAME boom)
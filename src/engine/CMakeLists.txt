cmake_minimum_required(VERSION 3.25)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

file(GLOB BOOM_CLIENT_SRC ${CMAKE_CURRENT_LIST_DIR}/client/*.c)
file(GLOB BOOM_SERVER_SRC ${CMAKE_CURRENT_LIST_DIR}/server/*.c)
file(GLOB BOOM_SHARED_SRC ${CMAKE_CURRENT_LIST_DIR}/shared/*.c)

add_library(BoomClientLib SHARED ${BOOM_CLIENT_SRC} ${BOOM_SHARED_SRC})
target_compile_definitions(BoomClientLib PUBLIC CLIENT_DLL)
target_compile_definitions(BoomClientLib PRIVATE BOOM_EXPORTS)
target_include_directories(BoomClientLib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/client ${CMAKE_CURRENT_LIST_DIR}/shared)
target_link_libraries(BoomClientLib PUBLIC puli glfw OpenGL::GL GLEW::GLEW)

add_library(BoomServerLib SHARED ${BOOM_SERVER_SRC} ${BOOM_SHARED_SRC})
target_compile_definitions(BoomServerLib PUBLIC SERVER_DLL)
target_compile_definitions(BoomServerLib PRIVATE BOOM_EXPORTS)
target_include_directories(BoomServerLib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/server ${CMAKE_CURRENT_LIST_DIR}/shared)
target_link_libraries(BoomServerLib PUBLIC puli)

set_target_properties(BoomClientLib PROPERTIES OUTPUT_NAME boom_cl)
set_target_properties(BoomServerLib PROPERTIES OUTPUT_NAME boom_sv)

add_subdirectory(scripting)
add_subdirectory(bnet)
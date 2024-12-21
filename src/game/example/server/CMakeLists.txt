cmake_minimum_required(VERSION 3.25)

project(ExampleServer)

add_library(ExampleServer SHARED server.c)
target_link_libraries(ExampleServer PRIVATE BoomServerLib)
set_target_properties(ExampleServer PROPERTIES OUTPUT_NAME server LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../example/bin)
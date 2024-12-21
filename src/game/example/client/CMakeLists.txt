cmake_minimum_required(VERSION 3.25)

project(ExampleClient)

add_library(ExampleClient SHARED client.c)
target_link_libraries(ExampleClient PRIVATE BoomClientLib)
set_target_properties(ExampleClient PROPERTIES OUTPUT_NAME client LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../example/bin)
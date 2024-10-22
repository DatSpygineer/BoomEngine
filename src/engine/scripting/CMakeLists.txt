cmake_minimum_required(VERSION 3.25)

project(BoomScript)

file(GLOB BOOM_SCRIPT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)

add_library(BoomScript STATIC ${BOOM_SCRIPT_SOURCES})
target_include_directories(BoomScript PUBLIC ${CMAKE_CURRENT_LIST_DIR})
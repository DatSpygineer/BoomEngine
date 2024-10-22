cmake_minimum_required(VERSION 3.25)

project(ut)

add_library(ut STATIC ut.c)
target_include_directories(ut PUBLIC ${CMAKE_CURRENT_LIST_DIR})
cmake_minimum_required(VERSION 3.25)

project(BoomGame C)

add_library(BoomGame SHARED game.c)
target_link_libraries(BoomGame PRIVATE BoomEngine)
set_target_properties(BoomGame PROPERTIES OUTPUT_NAME game)
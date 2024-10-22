cmake_minimum_required(VERSION 3.25)

project(BoomClientLib)
project(BoomServerLib)

if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	add_compile_definitions(_CRT_INTERNAL_NONSTDC_NAMES) # Required for "sys/stat.h"

	include(vcpkg.cmake)
endif()

set(BOOM_GAME_DIR ${CMAKE_CURRENT_LIST_DIR}/game)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BOOM_GAME_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BOOM_GAME_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dev)

add_subdirectory(src)
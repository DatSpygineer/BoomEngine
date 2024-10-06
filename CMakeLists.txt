cmake_minimum_required(VERSION 3.25)

project(BoomEngine C)

set(CMAKE_C_STANDARD 11)

if (WIN32)
    add_compile_definitions(_CRT_INTERNAL_NONSTDC_NAMES) # Required for "sys/stat.h"
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    option(USE_VCPKG "Enable/Disable use of VCPKG for packages." ON)
else()
    option(USE_VCPKG "Enable/Disable use of VCPKG for packages." OFF)
endif()

if (USE_VCPKG)
    include(vcpkg.cmake)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/game/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/game/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/game/lib)

add_subdirectory(src)
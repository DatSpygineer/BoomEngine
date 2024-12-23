cmake_minimum_required(VERSION 3.25)

project(puli)

set(CMAKE_CXX_STANDARD 11)

if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	add_compile_definitions(_CRT_INTERNAL_NONSTDC_NAMES) # Required for "sys/stat.h"
else()
	find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
endif()

file(GLOB PULI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_library(puli STATIC ${PULI_SOURCES})
target_include_directories(puli PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if (NOT WIN32)
	target_link_libraries(puli PUBLIC Qt6::Core Qt6::Widgets)
endif()

add_subdirectory(test)
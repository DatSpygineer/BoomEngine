cmake_minimum_required(VERSION 3.25)

project(bnet)

if (WIN32)
	set(BNET_SOURCES bnet_win32.c)
else ()
	set(BNET_SOURCES bnet_posix.c)
endif ()
add_library(bnet STATIC ${BNET_SOURCES})
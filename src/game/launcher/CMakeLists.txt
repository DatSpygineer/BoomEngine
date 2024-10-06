cmake_minimum_required(VERSION 3.25)

project(BoomLauncher C)

if (WIN32)
	add_executable(BoomLauncher WIN32 main.c launcher.rc)
else()
	add_executable(BoomLauncher main.c)
endif()

target_link_libraries(BoomLauncher PRIVATE BoomEngine)
set_target_properties(BoomLauncher PROPERTIES OUTPUT_NAME boom_cl)
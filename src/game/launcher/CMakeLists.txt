cmake_minimum_required(VERSION 3.25)

project(BoomLauncher)
project(BoomServer)

if (WIN32)
	add_executable(BoomLauncher WIN32 client.c client.rc)
else()
	add_executable(BoomLauncher client.c)
endif()
set_target_properties(BoomLauncher PROPERTIES
	OUTPUT_NAME boom
	RUNTIME_OUTPUT_DIRECTORY ${BOOM_GAME_DIR}
)
target_link_libraries(BoomLauncher PRIVATE puli BoomClientLib)

add_executable(BoomServer server.c)
set_target_properties(BoomServer PROPERTIES
	OUTPUT_NAME boom_server
	RUNTIME_OUTPUT_DIRECTORY ${BOOM_GAME_DIR}
)
target_link_libraries(BoomServer PRIVATE puli BoomServerLib)

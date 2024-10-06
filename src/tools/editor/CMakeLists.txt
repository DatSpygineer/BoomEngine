cmake_minimum_required(VERSION 3.25)

project(BoomEd)

set(CMAKE_CXX_STANDARD 20)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)
include(${wxWidgets_USE_FILE})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Lua REQUIRED)

file(GLOB EDITOR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

if (WIN32)
	add_executable(BoomEd WIN32 ${EDITOR_SOURCES} editor.rc)
else()
	add_executable(BoomEd ${EDITOR_SOURCES})
endif()

target_link_libraries(BoomEd PRIVATE ${wxWidgets_LIBRARIES} OpenGL::GL GLEW::GLEW ${LUA_LIBRARIES})
target_include_directories(BoomEd PRIVATE ${LUA_INCLUDE_DIR})
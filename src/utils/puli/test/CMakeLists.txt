cmake_minimum_required(VERSION 3.25)

project(puli_test)

add_executable(puli_test main.c)
target_link_libraries(puli_test PUBLIC puli ut)
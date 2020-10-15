cmake_minimum_required(VERSION 3.10)
project(c_com_stack C)

set(CMAKE_C_STANDARD 99)

add_executable(
        c_com_stack
        src/tests/comStackTest.c
)
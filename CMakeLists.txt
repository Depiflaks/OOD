cmake_minimum_required(VERSION 3.28)
project(OOD)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

set(CMAKE_EXPORT_COMPILE_COMMANDS_OUTPUT_DIR ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(labs/lab1 ${CMAKE_BINARY_DIR}/labs/lab1)

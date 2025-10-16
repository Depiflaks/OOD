cmake_minimum_required(VERSION 3.28)
project(OOD)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

set(CMAKE_EXPORT_COMPILE_COMMANDS_OUTPUT_DIR ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
  include(GoogleTest)
endif()

find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

add_subdirectory(labs/lab1/object_ducks ${CMAKE_BINARY_DIR}/labs/lab1/object_ducks)
add_subdirectory(labs/lab1/functional_ducks ${CMAKE_BINARY_DIR}/labs/lab1/functional_ducks)
add_subdirectory(labs/lab1/shapes ${CMAKE_BINARY_DIR}/labs/lab1/shapes)
add_subdirectory(labs/lab2/WeatherStation ${CMAKE_BINARY_DIR}/labs/lab2/WeatherStation)
add_subdirectory(labs/lab3/coffee ${CMAKE_BINARY_DIR}/labs/lab3/coffee)
add_subdirectory(labs/lab3/streams ${CMAKE_BINARY_DIR}/labs/lab3/streams)
add_subdirectory(labs/lab4/shapes ${CMAKE_BINARY_DIR}/labs/lab4/shapes)
add_subdirectory(labs/lab5/editor ${CMAKE_BINARY_DIR}/labs/lab4/editor)

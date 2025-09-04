cmake_minimum_required(VERSION 3.10)
project(lab1_duck)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
    SimUDuck/*.cpp
    SimUDuck/*.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

cmake_minimum_required(VERSION 3.10)
project(lab10_mvc)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
        src/*.cpp
        src/*.h
        src/*.ui
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        tests/*.cpp
        tests/*.h
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(APPEND TEST_SOURCES ${PROJECT_SOURCES})

add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
target_include_directories(${PROJECT_NAME}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/tests)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
        gtest
        gmock
        gtest_main
        Qt6::Widgets
        Boost::boost
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Boost::boost
)

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME}_tests)

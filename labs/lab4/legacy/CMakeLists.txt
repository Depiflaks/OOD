cmake_minimum_required(VERSION 3.10)
project(lab4_shape)

set(CMAKE_CXX_STANDARD 20)

if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif ()
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GTest REQUIRED)

# ==== SOURCES ====

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
    "src/*/*/*/*.cpp"
)

list(FILTER ALL_SOURCES EXCLUDE REGEX ".*tests/.*")

# ==== SHAPE_APP ====

add_executable(shape_app
        main.cpp
    ${ALL_SOURCES}
        src/openGl/openGl.h
        src/openGl/openGl.cpp
)

target_link_libraries(shape_app
    PRIVATE
    OpenGL::GL
    glfw
)

target_include_directories(shape_app PRIVATE
        src
)

# ==== TESTS ====

list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/GlCanvas\\.cpp$")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/openGl\\.cpp$")
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")

if (TEST_SOURCES)
    add_executable(shape_tests
        ${TEST_SOURCES}
        ${ALL_SOURCES}
    )

    target_include_directories(shape_tests PRIVATE
            src
    )

    target_link_libraries(shape_tests
        PRIVATE
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
    )

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(shape_tests)
endif ()
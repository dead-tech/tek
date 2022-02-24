cmake_minimum_required(VERSION 3.18.4)
project(tek)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILER_WARNINGS
        -std=c++17
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion
        -Wpedantic
        -Wold-style-cast
        -Wunused
        -Wnull-dereference
        -fsanitize=undefined
        )

file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "src/*.hpp"
        )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS TARGETS)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::fmt)
target_link_options(${PROJECT_NAME} PRIVATE ${COMPILER_WARNINGS})

# Testing
option(ENABLE_TESTING "Enable the tests" ON)
if (ENABLE_TESTING)
    file(GLOB_RECURSE TESTS
            "tests/*.cpp"
            )

    list(REMOVE_ITEM TESTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp")

    add_library(tek_lib ${SOURCES})
    target_link_libraries(tek_lib CONAN_PKG::fmt)
    target_link_options(tek_lib PRIVATE ${COMPILER_WARNINGS})

    add_library(catch_main OBJECT "tests/main.cpp")
    target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)
    target_link_options(catch_main PRIVATE ${COMPILER_WARNINGS})

    add_executable(tests ${TESTS})
    target_link_libraries(tests PRIVATE catch_main tek_lib)
    target_link_options(tests PRIVATE -std=c++17)
endif ()

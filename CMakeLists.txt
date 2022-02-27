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

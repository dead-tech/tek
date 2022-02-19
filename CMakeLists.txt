cmake_minimum_required(VERSION 3.18.4)
project(tek)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.hpp"
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

cmake_minimum_required(VERSION 3.20)
project(ChipSharp VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(core)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

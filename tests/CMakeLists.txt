cmake_minimum_required(VERSION 3.20)
project(ChipSharpTests LANGUAGES C)

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(unity)

add_executable(test_chip8
    test_chip8.c
)

target_link_libraries(test_chip8 PRIVATE chip8 unity)

target_include_directories(test_chip8 PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

include(CTest)
add_test(NAME Chip8Tests COMMAND test_chip8)

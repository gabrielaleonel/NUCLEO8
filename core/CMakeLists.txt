cmake_minimum_required(VERSION 3.20)
project(ChipSharpCore VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CHIP8_SOURCES
    src/chip8.c
    src/cpu.c
    src/memory.c
    src/display.c
    src/timers.c
    src/input.c
    src/fontset.c
)

add_library(chip8 SHARED ${CHIP8_SOURCES})

target_include_directories(chip8
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(chip8 PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

if(WIN32)
    target_compile_definitions(chip8 PRIVATE CHIP8_SHARED_EXPORTS)
endif()

if(NOT WIN32)
    target_compile_options(chip8 PRIVATE -fvisibility=hidden)
endif()

install(TARGETS chip8
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/chip8 DESTINATION include)

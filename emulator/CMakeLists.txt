cmake_minimum_required(VERSION 3.29)
project(emulator C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/${CMAKE_BUILD_TYPE}/bin)

set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS}")

find_package(SDL2 REQUIRED PATHS ${CMAKE_SOURCE_DIR}/sdl)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(emulator main.c
        utils.c
        chip8.c
)

target_link_libraries(emulator ${SDL2_LIBRARIES})


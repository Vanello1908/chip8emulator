cmake_minimum_required(VERSION 3.29)
project(emulator C)

set(CMAKE_C_STANDARD 11)

add_executable(emulator main.c
        chip8.h
        chip8.c
        definitions.h
        opcodes.h
        opcodes.c)

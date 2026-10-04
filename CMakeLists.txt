cmake_minimum_required(VERSION 4.4.3 FATAL_ERROR)

project(cpu-emulator LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


add_executable(cpu-emulator
    src/main.c
    src/isa.c
    src/assembler.c
    src/cpu.c
    src/debugger.c
    src/ram.c
)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
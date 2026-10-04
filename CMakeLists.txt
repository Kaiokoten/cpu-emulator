cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

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

if(MSVC)                                                   
    target_compile_options(cpu-emulator PRIVATE /W4)   
    target_compile_definitions(cpu-emulator PRIVATE _CRT_SECURE_NO_WARNINGS)    
else()                                                     
    target_compile_options(cpu-emulator PRIVATE -Wall -Wextra)
endif()
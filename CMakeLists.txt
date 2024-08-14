cmake_minimum_required(VERSION 3.28)
project(Assembler C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c90 -Wall -ansi -pedantic")


add_executable(Assembler main.c
        MacroProcessing.c
        MacroProcessing.h
        AssemblyConstants.h
        structs.h
        helper.h
        helper.c
        passOne.c
        passOne.h
        passTwo.c
        passTwo.h
        AssemblyConstants.c
)
target_compile_options(Assembler PRIVATE -std=c90 -Wall -ansi -pedantic)

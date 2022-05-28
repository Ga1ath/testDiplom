cmake_minimum_required(VERSION 3.21)
project(Diplom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(.)

add_executable(
        Diplom
        Coordinate.h
        Defines.h
        Error.h
        FileHandler.h
        Lexer.h
        main.cpp
        Node.h
        Value.h
)

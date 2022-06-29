cmake_minimum_required(VERSION 3.16)
project(tex-preprocessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(.)

add_executable(
    tex-preprocessor
    main.cpp
    Error.cpp
    Defines.cpp
    Coordinate.cpp
    FileHandler.cpp
    Lexer.cpp
    Node.cpp
    Value.cpp
    basic_HM.cpp
)

cmake_minimum_required(VERSION 3.5)
project(Diplom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(.)

add_executable(
    Diplom
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

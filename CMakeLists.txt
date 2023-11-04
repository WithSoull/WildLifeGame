cmake_minimum_required(VERSION 3.26)
project(NcursesGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(NcursesGame main.cpp
        terminal.cpp
        logic.h
        logic.cpp
        terminal.cpp)

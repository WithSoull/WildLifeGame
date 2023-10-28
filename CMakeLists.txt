cmake_minimum_required(VERSION 3.26)
project(WildLifeGame)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(WildLifeGame
        main.cpp
        terminal.cpp
        terminal.h
        logic.cpp
        logic.h
        emoji.h)

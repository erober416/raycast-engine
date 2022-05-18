cmake_minimum_required(VERSION 3.22)
project(untitled)

find_package(SDL2 REQUIRED SDL2)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled Main.cpp Player.cpp Player.h Main.h Map.cpp Map.h Texture.cpp Texture.h)
target_link_libraries(untitled PRIVATE SDL2)
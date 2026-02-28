cmake_minimum_required(VERSION 4.1)
project(C__GameDEV)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(C__GameDEV main.cpp)

target_link_libraries( C__GameDEV
        SDL2::SDL2
        OpenGL::GL
        GLEW::GLEW
)
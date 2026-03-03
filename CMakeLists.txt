cmake_minimum_required(VERSION 4.1)
project(VoidShooter)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_executable(VoidShooter
        src/main.cpp
        src/Game.cpp
        src/Player.cpp
        src/Enemy.cpp
        src/Bullet.cpp
        src/TextRender.cpp
        src/TextRender.h
)

target_include_directories(VoidShooter PRIVATE src)

target_link_libraries(VoidShooter
        SDL2::SDL2
        OpenGL::GL
        GLEW::GLEW
        stb
        glm::glm
        SDL2_ttf
)
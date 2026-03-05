#pragma once
#include <GL/glew.h>
#include <string>

class TextureLoader {
public:
    static GLuint load(const std::string& path);
};
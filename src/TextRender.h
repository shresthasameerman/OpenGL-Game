#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class TextRender {
public:
    TextRender(const std::string& fontPath, int fontSize);
    ~TextRender();
    void drawText(const std::string& text, float x, float y, glm::vec3 color);

private:
    TTF_Font* font;
    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint createTextTexture(const std::string& text, glm::vec3 color, int& w, int& h);
};
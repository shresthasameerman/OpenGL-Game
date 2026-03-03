#include "TextRender.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const char* textVertSrc = R"(
#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
uniform mat4 projection;
out vec2 vertUV;
void main(){
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    vertUV = aUV;
}
)";

const char* textFragSrc = R"(
#version 440 core
in vec2 vertUV;
out vec4 outColor;
uniform sampler2D textTexture;
void main(){
    outColor = texture(textTexture, vertUV);
}
)";

static GLuint compileTextShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Text shader error: " << log << "\n";
    }
    return shader;
}

TextRender::TextRender(const std::string& fontPath, int fontSize) {
    TTF_Init();
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
    }

    GLuint v = compileTextShader(GL_VERTEX_SHADER, textVertSrc);
    GLuint f = compileTextShader(GL_FRAGMENT_SHADER, textFragSrc);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, v);
    glAttachShader(shaderProgram, f);
    glLinkProgram(shaderProgram);
    glDeleteShader(v);
    glDeleteShader(f);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

TextRender::~TextRender() {
    TTF_CloseFont(font);
    TTF_Quit();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

GLuint TextRender::createTextTexture(const std::string& text, glm::vec3 color, int& w, int& h) {
    SDL_Color sdlColor = {
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        255
    };

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
    if (!surface) {
        std::cerr << "TTF render error: " << TTF_GetError() << "\n";
        return 0;
    }

    // Convert surface to RGBA format to ensure correct pixel layout
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    if (!converted) return 0;

    w = converted->w;
    h = converted->h;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);
    SDL_FreeSurface(converted);
    return texture;
}

void TextRender::drawText(const std::string& text, float x, float y, glm::vec3 color) {
    int w, h;
    GLuint texture = createTextTexture(text, color, w, h);
    if (!texture) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    float x2 = x + w;
    float y2 = y + h;

    float verts[] = {
        x,  y,  0.0f, 0.0f,
        x2, y,  1.0f, 0.0f,
        x2, y2, 1.0f, 1.0f,
        x,  y,  0.0f, 0.0f,
        x2, y2, 1.0f, 1.0f,
        x,  y2, 0.0f, 1.0f,
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "textTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDeleteTextures(1, &texture);
    glDisable(GL_BLEND);
}
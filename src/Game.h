#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

class Game {
    public:
    Game();
    ~Game();

    void run();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();
    void spawnEnemy();

    SDL_Window *window;
    SDL_GLContext glContext;
    GLuint shaderProgram;
    GLuint quadVAO;
    GLuint quadVBO;
    bool running;
    float spawnTimer;
    float spawnInterval;
    int score;

    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
};
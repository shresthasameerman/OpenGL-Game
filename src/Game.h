#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "TextRender.h"
#include "AudioManager.h"

class Game {
    public:
    Game();
    ~Game();

    void run();

private:
    enum class GameState { MAINMENU, PLAYING, GAMEOVER };

    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();
    void spawnEnemy();
    void initStars();
    void updateStars(float deltaTime);
    void restartGame();

    SDL_Window *window;
    SDL_GLContext glContext;
    GLuint shaderProgram;
    GLuint quadVAO;
    GLuint quadVBO;
    TextRender* textRender;
    TextRender* titleFont;
    bool running;
    float spawnTimer;
    float spawnInterval;
    int score;
    AudioManager* audio;
    GameState state;
    float flashTimer;
    GLuint texPlayer;
    GLuint texBullet;
    GLuint texEnemyStraight;
    GLuint texEnemyZigzag;
    glm::vec2 mousePos;

    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    struct Star {
        glm::vec2 position;
        float speed;
        float size;
        float brightness;
    };
    std::vector<Star> stars;
};
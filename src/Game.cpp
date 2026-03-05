#include "Game.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

const char* vertSrc = R"(
#version 440 core
layout(location = 0) in vec3 aPos;
uniform mat4 transform;
void main() {
    gl_Position = transform * vec4(aPos, 1.0);
}
)";

const char* fragSrc = R"(
#version 440 core
uniform vec3 color;
out vec4 outColor;
void main() {
    outColor = vec4(color, 1.0);
}
)";

glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error: " << log << "\n";
    }
    return shader;
}

void drawRect(GLuint shader, GLuint VAO, glm::vec2 pos, glm::vec2 size, glm::vec3 color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    glm::mat4 mvp = projection * model;
    glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(glGetUniformLocation(shader, "color"), color.r, color.g, color.b);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Game::Game() {
    audio = new AudioManager();
    srand(time(nullptr));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("Void Shooter",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    textRender = new TextRender("font.ttf", 32);

    GLuint v = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, v);
    glAttachShader(shaderProgram, f);
    glLinkProgram(shaderProgram);
    glDeleteShader(v);
    glDeleteShader(f);

    running = true;
    spawnTimer = 0.0f;
    spawnInterval = 2.0f;
    score = 0;
    state = GameState::PLAYING;
    flashTimer = 0.0f;
    initStars();
}

Game::~Game() {
    delete audio;
    delete textRender;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::restartGame() {
    score = 0;
    spawnTimer = 0.0f;
    spawnInterval = 2.0f;
    flashTimer = 0.0f;
    bullets.clear();
    enemies.clear();
    player = Player();
    state = GameState::PLAYING;
}

void Game::spawnEnemy() {
    glm::vec2 pos;
    int edge = rand() % 4;
    if (edge == 0)      pos = glm::vec2(rand() % 1280, -50.0f);
    else if (edge == 1) pos = glm::vec2(rand() % 1280, 770.0f);
    else if (edge == 2) pos = glm::vec2(-50.0f, rand() % 720);
    else                pos = glm::vec2(1330.0f, rand() % 720);

    EnemyType type = (rand() % 2 == 0) ? EnemyType::STRAIGHT : EnemyType::ZIGZAG;
    enemies.emplace_back(pos, type);
}

void Game::processInput(float deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;

        if (state == GameState::GAMEOVER) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
                restartGame();
        }

        if (state == GameState::PLAYING) {
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                if (player.shoot(glm::vec2(mx, my), bullets))
                    audio->playShoot();
            }
        }
    }

    if (state == GameState::PLAYING) {
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            if (player.shoot(glm::vec2(mx, my), bullets))
                audio->playShoot();
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        player.update(deltaTime, keys);
    }
}

void Game::update(float deltaTime) {
    if (state != GameState::PLAYING) return;

    updateStars(deltaTime);

    if (flashTimer > 0.0f) flashTimer -= deltaTime;

    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnEnemy();
        spawnTimer = 0.0f;
        if (spawnInterval > 0.5f) spawnInterval -= 0.05f;
    }

    for (auto& b : bullets) b.update(deltaTime);
    for (auto& e : enemies) e.update(deltaTime, player.position + player.size * 0.5f);

    // Bullet vs enemy collision
    for (auto& b : bullets) {
        for (auto& e : enemies) {
            if (!b.alive || !e.alive) continue;
            if (b.position.x < e.position.x + e.size.x &&
                b.position.x + b.size.x > e.position.x &&
                b.position.y < e.position.y + e.size.y &&
                b.position.y + b.size.y > e.position.y) {
                b.alive = false;
                e.alive = false;
                score++;
                audio->playExplosion();
            }
        }
    }

    // Enemy vs player collision
    for (auto& e : enemies) {
        if (!e.alive) continue;
        if (player.invincibleTimer > 0.0f) continue;
        if (e.position.x < player.position.x + player.size.x &&
            e.position.x + e.size.x > player.position.x &&
            e.position.y < player.position.y + player.size.y &&
            e.position.y + e.size.y > player.position.y) {
            player.health--;
            player.invincibleTimer = 1.5f; // 1.5 seconds invincible
            flashTimer = 0.3f;             // screen flash
            e.alive = false;
            if (player.health <= 0) {
                player.alive = false;
                state = GameState::GAMEOVER;
            }
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.alive; }), bullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.alive; }), enemies.end());
}

void Game::render() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
}

void Game::initStars() {
    for (int i = 0; i < 100; i++) {
        Star s;
        float depth = (rand() % 100) / 100.0f;
        s.position = glm::vec2(rand() % 1280, rand() % 720);
        s.speed = 20.0f + depth * 80.0f;
        s.size = 1.0f + depth * 2.0f;
        s.brightness = 0.5f + depth * 0.5f;
        stars.push_back(s);
    }
}

void Game::updateStars(float deltaTime) {
    for (auto& s : stars) {
        s.position.y += s.speed * deltaTime;
        if (s.position.y > 720) {
            s.position.y = 0;
            s.position.x = rand() % 1280;
        }
    }
}

void Game::run() {
    float verts[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - lastTime) / 1000.0f;
        lastTime = now;

        processInput(deltaTime);
        update(deltaTime);

        // Background color — flash red when hit
        if (flashTimer > 0.0f)
            glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
        else
            glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Always draw stars
        for (auto& s : stars)
            drawRect(shaderProgram, VAO, s.position, glm::vec2(s.size), glm::vec3(s.brightness));

        if (state == GameState::PLAYING) {
            // Draw player — blink when invincible
            bool showPlayer = player.invincibleTimer <= 0.0f ||
                              (int)(player.invincibleTimer * 10) % 2 == 0;
            if (showPlayer)
                drawRect(shaderProgram, VAO, player.position, player.size,
                    glm::vec3(0.0f, 1.0f, 1.0f));

            // Draw bullets
            for (auto& b : bullets)
                drawRect(shaderProgram, VAO, b.position, b.size,
                    glm::vec3(1.0f, 1.0f, 0.0f));

            // Draw enemies
            for (auto& e : enemies) {
                glm::vec3 col = (e.type == EnemyType::STRAIGHT) ?
                    glm::vec3(1.0f, 0.2f, 0.2f) :
                    glm::vec3(1.0f, 0.6f, 0.0f);
                drawRect(shaderProgram, VAO, e.position, e.size, col);
            }

            // HUD
            textRender->drawText("Score: " + std::to_string(score), 20.0f, 20.0f,
                glm::vec3(1.0f, 1.0f, 1.0f));
            textRender->drawText("Lives: " + std::to_string(player.health), 20.0f, 60.0f,
                glm::vec3(1.0f, 0.3f, 0.3f));

        } else if (state == GameState::GAMEOVER) {
            textRender->drawText("GAME OVER", 460.0f, 280.0f,
                glm::vec3(1.0f, 0.2f, 0.2f));
            textRender->drawText("Score: " + std::to_string(score), 540.0f, 340.0f,
                glm::vec3(1.0f, 1.0f, 1.0f));
            textRender->drawText("Press R to Restart", 430.0f, 400.0f,
                glm::vec3(0.8f, 0.8f, 0.8f));
        }

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
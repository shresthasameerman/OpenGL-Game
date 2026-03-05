#pragma once
#include <SDL_stdinc.h>
#include <vector>
#include <glm/glm.hpp>
#include "Bullet.h"

class Bullet;

class Player {
    public:
        Player();

    void update(float deltaTime, const Uint8* keys);
    bool shoot(glm::vec2 mousePos, std::vector<Bullet>& bullets);

    glm::vec2 position;
    glm::vec2 size;
    float speed;
    float shootCooldown;
    float shootTimer;
    bool alive;
    int health;
    float invincibleTimer;
};

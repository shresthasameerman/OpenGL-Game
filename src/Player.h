#pragma once
#include <SDL_stdinc.h>
#include <vector>
#include <glm/glm.hpp>

#include "Bullet.h"

class Player {
    public:
        Player();

    void update(float deltaTime, const Uint8* keys);
    void shoot(glm::vec2 mousePos, std::vector<Bullet>& bullets);

    glm::vec2 position;
    glm::vec2 size;
    float speed;
    float shootCooldown;
    float shootTimer;
    bool alive;
};

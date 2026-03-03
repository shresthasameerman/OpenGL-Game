#pragma once
#include <glm/glm.hpp>

class Bullet {
    public:
        Bullet(glm::vec2 pos, glm::vec2 dir);

        void update(float deltaTime);
        glm::vec2 position;
    glm::vec2 direction;
    glm::vec2 size;
    float speed;
    bool alive;
};
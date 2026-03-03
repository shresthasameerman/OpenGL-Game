//
// Created by sameer on 3/2/26.
//

#include "Bullet.h"

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir) {
    position = pos;
    direction = glm::normalize(dir);
    size = glm::vec2(12.0f, 12.0f);
    speed = 600.0f;
    alive = true;
}

void Bullet::update(float deltaTime) {
    position += direction * speed * deltaTime;

    if (position.x < 0 || position.x > 1200 ||
        position.y < 0 || position.y > 720) {
        alive = false;
    }
}
//
// Created by sameer on 3/2/26.
//

#include "Enemy.h"
#include <cmath>

Enemy::Enemy(glm::vec2 spawnPos, EnemyType type) {
    position = spawnPos;
    size = glm::vec2(40.0f,40.0f);
    speed = 120.0f;
    this->type = type;
    zigzagTimer = 0.0f;
    alive = true;
}

void Enemy::update(float deltaTime, glm::vec2 playerPos) {
    glm::vec2 center = position + size * 0.5f;
    glm::vec2 dir = glm::normalize(playerPos - center);

    if (type == EnemyType::STRAIGHT) {
        position += dir * speed * deltaTime;
    }
    else if (type == EnemyType::ZIGZAG) {
        zigzagTimer += deltaTime;
        //Perpendicular zigzag offset
        glm::vec2 prep = glm::vec2(-dir.y,dir.x);
        float zigzag = std::sin(zigzagTimer * 4.0f)*80.0f;
        position += dir * speed * deltaTime;
        position += prep *zigzag * deltaTime;
    }
}

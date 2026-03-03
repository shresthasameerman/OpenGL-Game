#pragma once
#include <glm/glm.hpp>
enum class EnemyType { STRAIGHT, ZIGZAG};

class Enemy {
    public:
        Enemy(glm::vec2 spawnPos, EnemyType type);
    void update(float deltaTime, glm::vec2 playerPos);

    glm::vec2 position;
    glm::vec2 size;
    float speed;
    EnemyType type;
    float zigzagTimer;
    bool alive;
};
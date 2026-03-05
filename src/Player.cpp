#include "Player.h"
#include "Bullet.h"
#include <vector>
#include <SDL2/SDL.h>

Player::Player() {
    position = glm::vec2(640.0f, 360.0f);
    size = glm::vec2(48.0f, 48.0f);
    speed = 300.0f;
    shootCooldown = 0.2f;
    shootTimer = 0.0f;
    alive = true;
}

void Player::update(float deltaTime, const Uint8* keys) {
    if (keys[SDL_SCANCODE_W]) position.y -= speed * deltaTime;
    if (keys[SDL_SCANCODE_S]) position.y += speed * deltaTime;
    if (keys[SDL_SCANCODE_A]) position.x -= speed * deltaTime;
    if (keys[SDL_SCANCODE_D]) position.x += speed * deltaTime;

    position.x = glm::clamp(position.x, 0.0f, 1280.0f - size.x);
    position.y = glm::clamp(position.y, 0.0f, 720.0f - size.y);

    if (shootTimer > 0.0f) shootTimer -= deltaTime;
}

bool Player::shoot(glm::vec2 mousePos, std::vector<Bullet>& bullets) {
    if (shootTimer <= 0.0f) {
        glm::vec2 center = position + size * 0.5f;
        glm::vec2 direction = mousePos - center;
        bullets.emplace_back(center, direction);
        shootTimer = shootCooldown;
        return true;
    }
    return false;
}
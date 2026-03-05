#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void playShoot();
    void playExplosion();

private:
    Mix_Chunk* shootSound;
    Mix_Chunk* explosionSound;

    Mix_Chunk* generateBeep(int frequency, int duration, int volume);
};
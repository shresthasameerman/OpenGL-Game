#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <vector>

AudioManager::AudioManager() {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512);
    shootSound = generateBeep(880, 50, 200);
    explosionSound = generateBeep(120, 200, 128);
}

AudioManager::~AudioManager() {
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(explosionSound);
    Mix_CloseAudio();
}

Mix_Chunk* AudioManager::generateBeep(int frequency, int duration, int volume) {
    int sampleRate = 44100;
    int numSamples = (duration * sampleRate) / 1000;
    std::vector<Uint8> buffer(numSamples * 2);

    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / sampleRate;
        float fade = 1.0f - (float)i / numSamples;
        float sample = std::sin(2.0f * M_PI * frequency * t);
        Sint16 val = (Sint16)(sample * volume * fade * 100);
        buffer[i * 2] = val & 0xFF;
        buffer[i * 2 + 1] = val & 0xFF;
    }

    Mix_Chunk* chunk = new Mix_Chunk;
    chunk->allocated = 1;
    chunk->alen = buffer.size();
    chunk->abuf = new Uint8[buffer.size()];
    std::copy(buffer.begin(), buffer.end(), chunk->abuf);
    chunk->volume = MIX_MAX_VOLUME;
    return chunk;
}

void AudioManager::playShoot() {
    Mix_PlayChannel(-1, shootSound, 0);
}

void AudioManager::playExplosion() {
    Mix_PlayChannel(-1, explosionSound, 0);
}
#include "audio/SoundManager.hpp"
#include "core/ConfigLoader.hpp"
#include <algorithm>

SoundManager::SoundManager(sf::Music& bgmMusic, bool bgmLoaded) 
    : bgmMusic(bgmMusic), bgmLoaded(bgmLoaded) {
}

void SoundManager::update(float dt) {
    if (fadeTimer > 0.f && fadeDuration > 0.f) {
        fadeTimer -= dt;
        float progress = 1.0f - (fadeTimer / fadeDuration);
        currentBGMVolume = currentBGMVolume + (targetBGMVolume - currentBGMVolume) * progress;
        bgmMusic.setVolume(currentBGMVolume);
    }
}

void SoundManager::setBGMVolume(float volume) {
    currentBGMVolume = volume;
    if (bgmLoaded) bgmMusic.setVolume(currentBGMVolume);
}

void SoundManager::stopBGM() {
    bgmMusic.stop();
}

void SoundManager::fadeBGM(float targetVolume, float duration) {
    targetBGMVolume = targetVolume;
    fadeTimer = duration;
    fadeDuration = duration;
}

void SoundManager::playJump() {
}

void SoundManager::playScore() {
}

void SoundManager::playDeath() {
}

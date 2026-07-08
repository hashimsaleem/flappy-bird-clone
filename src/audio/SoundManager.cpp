#include "audio/SoundManager.hpp"
#include "core/ConfigLoader.hpp"
#include "systems/ResourceManager.hpp"
#include <algorithm>

SoundManager::SoundManager(sf::Music& bgmMusic, bool bgmLoaded)
    : bgmMusic(bgmMusic), bgmLoaded(bgmLoaded) {
    sf::SoundBuffer dummy;
    for (int i = 0; i < MAX_SFX_COUNT; i++) {
        sfxPool[i] = std::make_unique<sf::Sound>(dummy);
    }
}

void SoundManager::playSfx(const std::string& assetPath) {
    for (int i = 0; i < MAX_SFX_COUNT; i++) {
        if (sfxPool[i]->getStatus() != sf::Sound::Status::Playing) {
            sf::Sound& ref = ResourceManager::getInstance().getSound(assetPath);
            sfxPool[i]->setBuffer(ref.getBuffer());
            sfxPool[i]->play();
            return;
        }
    }
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

void SoundManager::playJump() { playSfx("assets/jump.ogg"); }

void SoundManager::playScore() { playSfx("assets/score.ogg"); }

void SoundManager::playDeath() { playSfx("assets/death.ogg"); }

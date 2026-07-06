#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <string>

class SoundManager {
public:
    SoundManager(sf::Music& bgmMusic, bool bgmLoaded);
    
    void update(float dt);
    
    // BGM Controls
    void setBGMVolume(float volume);
    void stopBGM();
    void fadeBGM(float targetVolume, float duration);
    
    // SFX Controls
    void playJump();
    void playScore();
    void playDeath();
    float getCurrentBGMVolume() const { return currentBGMVolume; }

private:
    sf::Music& bgmMusic;
    bool bgmLoaded;
    
    float currentBGMVolume = 100.f;
    float targetBGMVolume = 100.f;
    float fadeTimer = 0.f;
    float fadeDuration = 0.f;

    // SFX Pool to allow concurrent sounds
    static const int MAX_SFX_COUNT = 8;
    std::unique_ptr<sf::Sound> sfxPool[MAX_SFX_COUNT];
    int nextSfxIndex = 0;

    void playSfx(const std::string& assetPath);
};
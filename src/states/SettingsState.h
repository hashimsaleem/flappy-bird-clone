#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "GameState.h"
#include "core/ConfigValues.hpp"
#include "audio/SoundManager.hpp"

class SettingsState : public GameState {
public:
    SettingsState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                  const sf::Font& fontRef, const std::string& assetsBase);

    void update(float dt) override;
    void draw(sf::RenderWindow& window, const sf::Font& font) override;
    void handleKeyPress(sf::Keyboard::Key key) override;
    void handleMouseClick(sf::Vector2f pos) override;
    void onEnter() override;

    float getAudioVolume() const { return audioVolume; }
    float getSfxVolume() const { return sfxVolume; }
    int getTargetFPS() const { return targetFPS; }
    int getParticleQuality() const { return particleQuality; }
    bool isFullscreen() const { return fullscreen; }

private:
    ConfigValues cfg;
    std::unique_ptr<SoundManager> soundManager;
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;
    std::string assetsBase;

    float audioVolume = 50.f;
    float sfxVolume = 100.f;
    int difficulty = 1;
    int targetFPS = 60;
    int particleQuality = 1;
    bool fullscreen = false;

    int selectedOption = 0;
    float hoverAlpha = 0.f;
    float timer = 0.f;
    StateAction nextActionCode = StateAction::None;

public:
    StateAction nextAction() const override { return nextActionCode; }
    int selectedDifficulty() const override { return difficulty; }
};

#endif

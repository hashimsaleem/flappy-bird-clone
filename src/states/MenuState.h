#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include "GameState.h"
#include "core/ConfigValues.hpp"
#include "audio/SoundManager.hpp"


class MenuState : public GameState {
public:
    MenuState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
               const sf::Font& fontRef)
        : cfg(cfg),
          soundManager(std::make_unique<SoundManager>(bgmMusic, bgmLoaded)),
          bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
          font(&fontRef) {
        volume = bgmMusic.getVolume();
    }


    void update(float dt) override {
        blinkTimer += dt;
        if (blinkTimer > 0.5f) blinkTimer -= 0.5f;
        bobTimer += dt;
        birdBobTimer += dt;
        birdFlapTimer += dt;
        if (selectedOption >= 0) {
            if (selectedOption == 0) {
                nextActionCode = StateAction::PlayGame;
            } else if (selectedOption == 1) {
                nextActionCode = StateAction::ShowHighScore;
            }
        }
    }

    void draw(sf::RenderWindow& window, const sf::Font& fontRef) override {
        (void)fontRef;
        sf::RectangleShape bg;
        bg.setSize({static_cast<float>(cfg.screenWidth), static_cast<float>(cfg.screenHeight)});
        bg.setFillColor(sf::Color(20, 20, 40));
        window.draw(bg);

        if (birdSprite) {
            birdSprite->setOrigin({20.f, 20.f});
            float birdOffset = std::sin(birdBobTimer * 1.5f) * 3.0f;
            birdSprite->setPosition({400.f, 250.f + birdOffset});
            float flapScale = 2.2f + std::sin(birdFlapTimer * 8.0f) * 0.05f;
            birdSprite->setScale({flapScale, flapScale});
            birdSprite->setColor(sf::Color(255, 255, 255, 200));
            window.draw(*birdSprite);
        }

        float titleBob = std::sin(bobTimer * 2.0f) * 5.0f;
        auto titleText = makeText(fontRef, "FLAPPY CLONE", 60, Config::TEXT_COLOR,
            {static_cast<float>(cfg.screenWidth) / 2.f - 100.f, static_cast<float>(cfg.screenHeight) / 2.f - 120.f + titleBob});
        window.draw(titleText);

        auto option1 = makeText(fontRef, "1: Play Game", 30, Config::TEXT_COLOR,
            {static_cast<float>(cfg.screenWidth) / 2.f - 80.f, static_cast<float>(cfg.screenHeight) / 2.f});
        centerText(window, option1);
        window.draw(option1);

        auto option2 = makeText(fontRef, "2: High Score", 30, Config::TEXT_COLOR,
            {static_cast<float>(cfg.screenWidth) / 2.f - 80.f, static_cast<float>(cfg.screenHeight) / 2.f + 45.f});
        centerText(window, option2);
        window.draw(option2);

        auto volText = makeText(fontRef, "Volume: " + std::to_string(static_cast<int>(volume)) + "%  (+/-)",
            22, sf::Color(180, 180, 180),
            {static_cast<float>(cfg.screenWidth) / 2.f - 80.f, static_cast<float>(cfg.screenHeight) / 2.f + 100.f});
        centerText(window, volText);
        window.draw(volText);

        static const char* diffNames[] = {"Easy", "Medium", "Hard"};
        auto diffText = makeText(fontRef,
            std::string("Difficulty: ") + diffNames[difficulty] + "  (3/4/5)",
            22, sf::Color(180, 180, 180),
            {static_cast<float>(cfg.screenWidth) / 2.f - 80.f, static_cast<float>(cfg.screenHeight) / 2.f + 130.f});
        centerText(window, diffText);
        window.draw(diffText);

        if (blinkTimer < 0.3f) {
            auto hintText = makeText(fontRef, "Press 1 to Play", 22,
                sf::Color(200, 200, 255, 180),
                {static_cast<float>(cfg.screenWidth) / 2.f - 80.f, static_cast<float>(cfg.screenHeight) / 2.f - 50.f});
            centerText(window, hintText);
            window.draw(hintText);
        }
    }

    void handleKeyPress(sf::Keyboard::Key key) override {
        if (key == sf::Keyboard::Key::Num1) {
            selectedOption = 0;
        } else if (key == sf::Keyboard::Key::Num2) {
            selectedOption = 1;
        } else if (key == sf::Keyboard::Key::Num3) {
            difficulty = 0;
        } else if (key == sf::Keyboard::Key::Num4) {
            difficulty = 1;
        } else if (key == sf::Keyboard::Key::Num5) {
            difficulty = 2;
        } else if (key == sf::Keyboard::Key::Equal || key == sf::Keyboard::Key::Add) {
            volume = std::min(100.f, volume + 10.f);
            bgmMusic.setVolume(volume);
        } else if (key == sf::Keyboard::Key::Hyphen || key == sf::Keyboard::Key::Subtract) {
            volume = std::max(0.f, volume - 10.f);
            bgmMusic.setVolume(volume);
        }
    }

    void onEnter() override {
        blinkTimer = 0.f;
        birdTexture.loadFromFile(cfg.birdPath);
        birdSprite = std::make_unique<sf::Sprite>(birdTexture);
        birdSprite->setPosition(sf::Vector2f(355.f, 250.f));
        birdSprite->setScale(sf::Vector2f(2.2f, 2.2f));
    }

    StateAction nextAction() const override { return nextActionCode; }
    int selectedDifficulty() const override { return difficulty; }
    float getVolume() const { return volume; }

private:
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;
    
    std::unique_ptr<SoundManager> soundManager;
    int selectedOption = -1;
    StateAction nextActionCode = StateAction::None;
    float blinkTimer = 0.f;
    ConfigValues cfg;
    float volume = 50.f;
    int difficulty = 1;

    // Menu polish
    float bobTimer = 0.f;
    float birdBobTimer = 0.f;
    float birdFlapTimer = 0.f;
    sf::Texture birdTexture;
    std::unique_ptr<sf::Sprite> birdSprite;
};

#endif

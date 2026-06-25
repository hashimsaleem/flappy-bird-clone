#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "GameState.h"
#include "Config.hpp"

class MenuState : public GameState {
public:
    MenuState(sf::Sound jumpSnd, sf::Sound scoreSnd, sf::Sound deathSnd,
              sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
              const sf::Font& fontRef)
        : jumpSound(jumpSnd), scoreSnd(scoreSnd), deathSound(deathSnd),
          bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
          font(&fontRef) {
    }

    void update(float dt) override {
        if (selectedOption >= 0) {
            if (selectedOption == 0) {
                nextActionCode = 1;
            } else if (selectedOption == 1) {
                nextActionCode = 2;
            }
        }
    }

    void draw(sf::RenderWindow& window, const sf::Font& fontRef) override {
        (void)fontRef;
        // Draw background
        sf::RectangleShape bg;
        bg.setSize({static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)});
        bg.setFillColor(sf::Color(20, 20, 40));
        window.draw(bg);

        // Draw Title
        auto titleText = makeText(fontRef, "FLAPPY CLONE", 60, Config::TEXT_COLOR, {static_cast<float>(Config::SCREEN_WIDTH) / 2.f - 100.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 100.f});
        window.draw(titleText);

        // Draw Options
        auto option1 = makeText(fontRef, "Press 1: Play Game", 30, Config::TEXT_COLOR, {static_cast<float>(Config::SCREEN_WIDTH) / 2.f - 100.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f});
        window.draw(option1);

        auto option2 = makeText(fontRef, "Press 2: High Score", 30, Config::TEXT_COLOR, {static_cast<float>(Config::SCREEN_WIDTH) / 2.f - 100.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f + 50.f});
        window.draw(option2);
    }

    void handleKeyPress(sf::Keyboard::Key key) override {
        if (key == sf::Keyboard::Key::Space) {
            // Do nothing
        } else if (key == sf::Keyboard::Key::Num1) {
            selectedOption = 0;
        } else if (key == sf::Keyboard::Key::Num2) {
            selectedOption = 1;
        }
    }

    void onEnter() override {}

    int nextAction() const { return nextActionCode; }
    void setRestartState(float posX, float posY, float vel) {}

private:
    sf::Sound jumpSound;
    sf::Sound scoreSnd;
    sf::Sound deathSound;
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;

public: // Made public so main.cpp can access it
    int selectedOption = -1;
    int nextActionCode = 0;
};

#endif // MENUSTATE_H

#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.h"

class MenuState : public GameState {
public:
    void update(float dt) override {
        titleBobTimer += dt;
        float bob = std::sin(titleBobTimer * 2.0f) * 5.0f;
        titlePos = sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 200), 100.f + bob);
        birdPreviewTimer += dt;
        birdPreviewBob = std::sin(birdPreviewTimer * 1.5f) * 15.0f;
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) override {
        for (int i = 0; i < (int)menuOptions.size(); ++i) {
            auto opt = makeText(font, menuOptions[i], 30,
                i == menuOption ? sf::Color::Yellow : Config::TEXT_COLOR,
                sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 100),
                             250.f + i * 50.f));
            window.draw(opt);
        }
        auto title = makeText(font, "FLAPPY CLONE", 60, Config::TEXT_COLOR, titlePos);
        window.draw(title);
        auto preview = makeText(font, "\u2739", 40, sf::Color(255, 220, 50),
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 20),
                         static_cast<float>(Config::SCREEN_HEIGHT - 80 + birdPreviewBob)));
        window.draw(preview);
        auto subtitle = makeText(font, "A SFML C++ Game", 20, sf::Color(200, 200, 200),
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 70),
                         static_cast<float>(titlePos.y + 70.f)));
        window.draw(subtitle);
    }

    void handleKeyPress(sf::Keyboard::Key key) override {
        int optCount = (int)menuOptions.size();
        if (key == sf::Keyboard::Key::Up) {
            menuOption = (menuOption - 1 + optCount) % optCount;
        } else if (key == sf::Keyboard::Key::Down) {
            menuOption = (menuOption + 1) % optCount;
        } else if (key == sf::Keyboard::Key::Enter) {
            selectedOption = menuOption;
        }
    }

    int selectedOption = -1;
    int menuOption = 0;

private:
    const std::vector<std::string> menuOptions = {"Start Game", "High Scores", "Exit"};
    sf::Vector2f titlePos;
    float titleBobTimer = 0.0f;
    float birdPreviewTimer = 0.0f;
    float birdPreviewBob = 0.0f;
};

#endif // MENUSTATE_H

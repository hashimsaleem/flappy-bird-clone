#ifndef HIGHSCORESCREENSTATE_H
#define HIGHSCORESCREENSTATE_H

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "core/HighScore.hpp"

class HighScoreScreenState : public GameState {
public:
    void update(float dt) override { (void)dt; }

    void draw(sf::RenderWindow& window, const sf::Font& font) override {
        auto title = makeText(font, "HIGH SCORE", 50, Config::TEXT_COLOR,
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 150), 150.f));
        window.draw(title);

        auto hsValue = makeText(font, std::to_string(HighScore::load()), 80,
            sf::Color::Yellow,
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 50), 250.f));
        window.draw(hsValue);

        auto backText = makeText(font, "Press ESC to return", 24, Config::TEXT_COLOR,
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 100), 450.f));
        window.draw(backText);
    }

    void handleKeyPress(sf::Keyboard::Key key) override {
        if (key == sf::Keyboard::Key::Escape) {
            returnToMenu = true;
        }
    }

    StateAction nextAction() const override {
        return returnToMenu ? StateAction::ReturnToMenu : StateAction::None;
    }

private:
    bool returnToMenu = false;
};

#endif

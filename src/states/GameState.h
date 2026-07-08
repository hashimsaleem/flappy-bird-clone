#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "core/Config.hpp"
#include "entities/BirdState.h"

enum class StateAction {
    None,
    PlayGame,
    ShowHighScore,
    GameOver,
    ReturnToMenu,
    ShowSettings,
    Exit
};

class GameState {
public:
    virtual ~GameState() = default;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window, const sf::Font& font) = 0;
    virtual void handleKeyPress(sf::Keyboard::Key key) = 0;
    virtual void handleMouseClick(sf::Vector2f pos) { (void)pos; }

    virtual void onEnter() {}

    virtual StateAction nextAction() const { return StateAction::None; }
    virtual BirdState getRestartBirdState() const { return {}; }
    virtual int selectedDifficulty() const { return 1; }

    static sf::Text makeText(const sf::Font& font, const std::string& str,
                             unsigned int charSize, sf::Color color, sf::Vector2f pos) {
        sf::Text t(font, str, charSize);
        t.setFillColor(color);
        t.setPosition(pos);
        return t;
    }

    static void centerText(sf::RenderWindow& window, sf::Text& text) {
        auto sz = text.getLocalBounds().size;
        text.setPosition(sf::Vector2f(
            (static_cast<float>(window.getSize().x) - sz.x) / 2.f,
            text.getPosition().y));
    }
};

#endif

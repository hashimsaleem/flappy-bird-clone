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
        titlePos = sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 180), 100.f + bob);
        birdPreviewTimer += dt;
        birdPreviewBob = std::sin(birdPreviewTimer * 1.5f) * 15.0f;
        menuGlowTimer += dt;
        menuGlowIntensity = 0.5f + 0.5f * std::sin(menuGlowTimer * 3.0f);
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) override {
        // Background gradient
        sf::RectangleShape bgTop;
        bgTop.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH),
                                    static_cast<float>(Config::SCREEN_HEIGHT / 2)));
        bgTop.setFillColor(sf::Color(60, 100, 150));
        window.draw(bgTop);
        
        sf::RectangleShape bgBot;
        bgBot.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH),
                                    static_cast<float>(Config::SCREEN_HEIGHT / 2)));
        bgBot.setPosition(sf::Vector2f(0.f, static_cast<float>(Config::SCREEN_HEIGHT / 2)));
        bgBot.setFillColor(sf::Color(40, 70, 100));
        window.draw(bgBot);

        // Pulsing title glow
        unsigned char glowAlpha = static_cast<unsigned char>(menuGlowIntensity * 80 + 175);
        auto glowColor = sf::Color(
            static_cast<unsigned char>(255 * menuGlowIntensity + 200 * (1 - menuGlowIntensity)),
            static_cast<unsigned char>(220 * menuGlowIntensity + 220 * (1 - menuGlowIntensity)),
            50
        );
        
        auto title = makeText(font, "FLAPPY CLONE", 60, glowColor, titlePos);
        window.draw(title);
        
        auto subtitle = makeText(font, "A SFML C++ Game", 22, sf::Color(200, 200, 200),
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 90),
                         static_cast<float>(titlePos.y + 75.f)));
        window.draw(subtitle);

        // Menu options with hover effect
        for (int i = 0; i < (int)menuOptions.size(); ++i) {
            float scale = i == menuOption ? 1.15f : 1.0f;
            auto color = i == menuOption ? sf::Color::Yellow : Config::TEXT_COLOR;
            auto opt = makeText(font, menuOptions[i], static_cast<unsigned int>(30 * scale),
                color,
                sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 100 * (scale)),
                             250.f + i * 50.f));
            window.draw(opt);
            
            // Option icon
            const char* icon = i == menuOption ? "▶" : "•";
            auto iconText = makeText(font, std::string(icon), 24,
                i == menuOption ? sf::Color::Yellow : sf::Color(150, 150, 150),
                sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 150),
                             252.f + i * 50.f));
            window.draw(iconText);
        }

        auto preview = makeText(font, "\u2739", 40, sf::Color(255, 220, 50),
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 20),
                         static_cast<float>(Config::SCREEN_HEIGHT - 80 + birdPreviewBob)));
        window.draw(preview);
        
        // "Press to start" prompt
        float alphaVal = (0.5f + 0.5f * std::sin(menuGlowTimer * 2.0f)) * 255.f;
        auto prompt = makeText(font, "Press ENTER to start", 20, sf::Color(static_cast<unsigned char>(alphaVal), static_cast<unsigned char>(alphaVal), static_cast<unsigned char>(alphaVal)),
            sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 100),
                         static_cast<float>(Config::SCREEN_HEIGHT - 120)));
        window.draw(prompt);
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
    float menuGlowTimer = 0.0f;
    float menuGlowIntensity = 0.5f;
};

#endif // MENUSTATE_H

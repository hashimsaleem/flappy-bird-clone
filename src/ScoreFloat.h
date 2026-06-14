#ifndef SCOREFLOAT_H
#define SCOREFLOAT_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>
#include "Config.hpp"

struct ScoreFloat {
    sf::Text text;
    float lifetime;
    float startY;

    ScoreFloat(const sf::Font& font, sf::Vector2f pos)
        : text(font, "+1", 28), lifetime(Config::SCORE_FLOAT_DURATION), startY(pos.y) {
        text.setFillColor(sf::Color(255, 255, 100));
        text.setPosition(sf::Vector2f(pos));
    }

    void update(float dt) {
        lifetime -= dt;
        text.move({0.f, -Config::SCORE_FLOAT_SPEED * dt});
        float progress = 1.0f - (lifetime / Config::SCORE_FLOAT_DURATION);
        float scale;
        if (progress < 0.3f) {
            scale = 1.0f + Config::SCORE_FLOAT_SCALE_MAX * (progress / 0.3f);
        } else {
            scale = Config::SCORE_FLOAT_SCALE_MAX * (1.0f - (progress - 0.3f) / 0.7f);
        }
        text.setCharacterSize(static_cast<unsigned int>(28 * scale));
        float alpha = std::max(0.f, (lifetime / Config::SCORE_FLOAT_DURATION) * 255.f);
        auto bc = sf::Color(255, 255, 100);
        text.setFillColor(sf::Color(bc.r, bc.g, bc.b, static_cast<unsigned char>(alpha)));
    }

    void draw(sf::RenderWindow& window) const {
        if (lifetime > 0) window.draw(text);
    }

    bool alive() const { return lifetime > 0; }
};

#endif // SCOREFLOAT_H

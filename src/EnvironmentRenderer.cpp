#include "EnvironmentRenderer.hpp"
#include <cmath>

void EnvironmentRenderer::draw(sf::RenderWindow& window, float scrollOffset, float skyTimer) {
    float totalW = static_cast<float>(Config::SCREEN_WIDTH);
    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));

    // Sky rendering logic (from PlayState.cpp 103-136)
    float cyclePos = std::fmod(skyTimer, Config::SKY_CYCLE_INTERVAL) / Config::SKY_CYCLE_INTERVAL;
    int idx1 = static_cast<int>(cyclePos * Config::SKY_COLOR_COUNT) % Config::SKY_COLOR_COUNT;
    int idx2 = (idx1 + 1) % Config::SKY_COLOR_COUNT;
    float t = (cyclePos * Config::SKY_COLOR_COUNT) - idx1;

    auto lerp = [](float a, float b, float t) -> unsigned char {
        return static_cast<unsigned char>(a * (1 - t) + b * t);
    };

    sf::Color top(
        lerp(Config::SKY_TOP[idx1].r, Config::SKY_TOP[idx2].r, t),
        lerp(Config::SKY_TOP[idx1].g, Config::SKY_TOP[idx2].g, t),
        lerp(Config::SKY_TOP[idx1].b, Config::SKY_TOP[idx2].b, t)
    );
    sf::Color bot(
        lerp(Config::SKY_BOT[idx1].r, Config::SKY_BOT[idx2].r, t),
        lerp(Config::SKY_BOT[idx1].g, Config::SKY_BOT[idx2].g, t),
        lerp(Config::SKY_BOT[idx1].b, Config::SKY_BOT[idx2].b, t)
    );

    float halfH = static_cast<float>(Config::SCREEN_HEIGHT) / 2.f;
    sf::RectangleShape topRect;
    topRect.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), halfH));
    topRect.setFillColor(top);
    window.draw(topRect);

    sf::RectangleShape botRect;
    botRect.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), halfH));
    botRect.setPosition(sf::Vector2f(0.f, halfH));
    botRect.setFillColor(bot);
    window.draw(botRect);

    // Ground tiles logic (from PlayState.cpp 160-189)
    sf::RectangleShape base;
    base.setSize(sf::Vector2f(totalW, static_cast<float>(Config::GROUND_HEIGHT)));
    base.setPosition(sf::Vector2f(0.f, groundY));
    base.setFillColor(Config::GROUND_COLOR);
    window.draw(base);

    for (float x = -scrollOffset - Config::GROUND_TILE_W; x < totalW + Config::GROUND_TILE_W; x += Config::GROUND_TILE_W) {
        int idx = static_cast<int>(std::floor((x + scrollOffset) / Config::GROUND_TILE_W)) % Config::GROUND_TILE_COUNT;
        if (idx < 0) idx += Config::GROUND_TILE_COUNT;
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(static_cast<float>(Config::GROUND_TILE_W), 8.f));
        tile.setPosition(sf::Vector2f(x, groundY));
        tile.setFillColor(Config::GROUND_TOP_COLORS[idx]);
        window.draw(tile);
    }

    float grassY = groundY - 2.f;
    for (float x = -scrollOffset; x < totalW; x += 12.f) {
        float h = std::sin(x * 0.5f) * 2.f + 3.f;
        sf::RectangleShape blade;
        blade.setSize(sf::Vector2f(2.f, h));
        blade.setPosition(sf::Vector2f(x, grassY - h));
        blade.setFillColor(sf::Color(80, 140, 60));
        window.draw(blade);
    }

    // Background layers logic (from PlayState.cpp 351-360)
    sf::RectangleShape bgLayer;
    bgLayer.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), 150.f));
    bgLayer.setPosition(sf::Vector2f(bgOffset, groundY - 150.f));
    bgLayer.setFillColor(sf::Color(100, 150, 100));
    window.draw(bgLayer);

    bgLayer.setPosition(sf::Vector2f(bgOffset + static_cast<float>(Config::SCREEN_WIDTH), groundY - 150.f));
    window.draw(bgLayer);
}

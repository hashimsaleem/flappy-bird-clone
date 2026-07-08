#include "CloudSystem.hpp"
#include <cstdlib>
#include <cmath>

CloudSystem::CloudSystem(const ConfigValues& cfg) {
    for (int i = 0; i < 6; i++) {
        Cloud c;
        c.x = (static_cast<float>(i) / 6.f) * static_cast<float>(cfg.screenWidth) + static_cast<float>(rand() % 100);
        c.y = 30.f + static_cast<float>(rand() % 120);
        c.speed = 15.f + static_cast<float>(rand() % 25);
        c.radius = 20.f + static_cast<float>(rand() % 40);
        c.alpha = static_cast<unsigned char>(60 + rand() % 80);
        clouds.push_back(c);
    }
    this->cfg = cfg;
}

void CloudSystem::update(float dt) {
    cloudOffset += cfg.backgroundSpeed * dt;
}

void CloudSystem::draw(sf::RenderWindow& window) {
    for (const auto& c : clouds) {
        float cx = std::fmod(c.x + cloudOffset * (c.speed / 40.f), static_cast<float>(cfg.screenWidth) + c.radius * 2.f) - c.radius;
        sf::CircleShape shape(c.radius);
        shape.setFillColor(sf::Color(255, 255, 255, c.alpha));
        shape.setPosition({cx, c.y});
        window.draw(shape);
        sf::CircleShape shape2(c.radius * 0.7f);
        shape2.setFillColor(sf::Color(255, 255, 255, c.alpha - 20));
        shape2.setPosition({cx + c.radius * 0.5f, c.y - c.radius * 0.3f});
        window.draw(shape2);
    }
}

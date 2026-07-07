#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct Particle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime; // In seconds

    Particle(sf::Vector2f position, sf::Vector2f vel, float life) 
        : velocity(vel), lifetime(life) {
        shape.setSize({5.f, 5.f});
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(position);
    }

    void update(float dt) {
        shape.move(velocity * dt);
        lifetime -= dt;
        // Fade out
        float alpha = std::max(0.f, lifetime * 255.f);
        shape.setFillColor(sf::Color(255, 255, 0, static_cast<unsigned char>(alpha)));
    }

    void draw(sf::RenderWindow& window) const {
        if (lifetime > 0) {
            window.draw(shape);
        }
    }
};

#endif // PARTICLE_HPP

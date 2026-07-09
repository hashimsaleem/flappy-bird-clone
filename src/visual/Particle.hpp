#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

enum class ParticleType {
    Fire,
    Dust,
    Spark,
    Bubble
};

struct Particle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    ParticleType type = ParticleType::Fire;
    float wobblePhase = 0.0f;

    Particle(sf::Vector2f position, sf::Vector2f vel, float life, ParticleType t = ParticleType::Fire)
        : velocity(vel), lifetime(life), type(t) {
        shape.setSize({5.f, 5.f});
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(position);
    }

    void update(float dt) {
        shape.move(velocity * dt);
        lifetime -= dt;

        if (type == ParticleType::Bubble) {
            wobblePhase += dt * 5.0f;
        }

        switch (type) {
            case ParticleType::Fire: {
                float alpha = std::max(0.f, lifetime * 255.f);
                shape.setFillColor(sf::Color(255, 255, 0, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Dust: {
                float alpha = std::max(0.f, lifetime * 400.f);
                shape.setFillColor(sf::Color(200, 190, 170, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Spark: {
                float alpha = std::max(0.f, lifetime * 800.f);
                shape.setFillColor(sf::Color(255, 140, 50, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Bubble: {
                float alpha = std::max(0.f, (lifetime - 0.1f) * 350.f);
                if (alpha < 0) alpha = 0;
                shape.setFillColor(sf::Color(255, 215, 0, static_cast<unsigned char>(alpha)));
                float wobble = std::sin(wobblePhase) * 10.f;
                shape.move(sf::Vector2f(wobble * dt, 0.f));
                break;
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (lifetime > 0) {
            window.draw(shape);
        }
    }
};

#endif // PARTICLE_HPP

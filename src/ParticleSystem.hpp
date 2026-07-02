#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "ObjectPool.h"
#include "Particle.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity);
    std::vector<Particle> getParticles() const;
private:
    std::unique_ptr<ObjectPool<Particle>> particlePool;
    std::vector<int> activeParticles;
};

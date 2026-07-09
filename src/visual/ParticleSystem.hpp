#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include "core/ObjectPool.h"
#include "Particle.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void spawnDust(sf::Vector2f pos, int count);
    void spawnSparks(sf::Vector2f pos, int count);
    void spawnScoreSparkle(sf::Vector2f pos, int count);
    std::vector<Particle> getParticles() const;
    size_t getActiveCount() const { return activeParticles.size(); }
private:
    std::unique_ptr<ObjectPool<Particle>> particlePool;
    std::vector<int> activeParticles;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_real_distribution<float> spawnDist;
    std::uniform_real_distribution<float> dustSizeDist;
    std::uniform_real_distribution<float> dustVelDist;
    std::uniform_real_distribution<float> dustAngleDist;
    std::uniform_real_distribution<float> sparkSizeDist;
    std::uniform_real_distribution<float> sparkSpeedDist;
    std::uniform_real_distribution<float> sparkAngleDist;
    std::uniform_real_distribution<float> sparkleSizeDist;
    std::uniform_real_distribution<float> sparkleWobbleDist;
    std::uniform_real_distribution<float> lifetimeDist;
};

#include "ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <random>

ParticleSystem::ParticleSystem()
    : rng(rd()),
      spawnDist(-2.0f, 2.0f),
      dustSizeDist(2.0f, 4.0f),
      dustVelDist(0.5f, 1.5f),
      dustAngleDist(-0.2f, 0.2f),
      sparkSizeDist(3.0f, 5.0f),
      sparkSpeedDist(200.0f, 600.0f),
      sparkAngleDist(0.0f, 6.283185f),
      sparkleSizeDist(4.0f, 6.0f),
      sparkleWobbleDist(-0.1f, 0.1f),
      lifetimeDist(0.0f, 0.2f) {
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void ParticleSystem::spawn(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    for (int i = 0; i < count; ++i) {
        float vx = (spawnDist(rng) - 1.0f) * 200.0f;
        float vy = (spawnDist(rng) - 1.0f) * 200.0f;
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setPosition(pos);
        p.velocity = velocity + sf::Vector2f(vx, vy);
        p.lifetime = 1.0f;
        p.type = ParticleType::Fire;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnDust(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        float size = dustSizeDist(rng);
        float angle = dustAngleDist(rng);
        float speed = dustVelDist(rng) * 30.0f;
        sf::Vector2f vel(std::cos(angle) * speed, 20.0f + std::abs(std::sin(angle)) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.3f + lifetimeDist(rng);
        p.type = ParticleType::Dust;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnSparks(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        float size = sparkSizeDist(rng);
        float speed = sparkSpeedDist(rng);
        float angle = sparkAngleDist(rng);
        sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.2f + lifetimeDist(rng);
        p.type = ParticleType::Spark;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnScoreSparkle(sf::Vector2f pos, int count) {
    for (int i = 0; i < count; ++i) {
        float size = sparkleSizeDist(rng);
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = sf::Vector2f(sparkleWobbleDist(rng) * 100.f, -100.f);
        p.lifetime = 0.6f + lifetimeDist(rng);
        p.type = ParticleType::Bubble;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::update(float dt) {
    for (auto it = activeParticles.begin(); it != activeParticles.end(); ) {
        int idx = *it;
        Particle& p = (*particlePool)[idx];
        p.update(dt);
        if (p.lifetime <= 0) {
            particlePool->release(idx);
            it = activeParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (int idx : activeParticles) {
        (*particlePool)[idx].draw(window);
    }
}

std::vector<Particle> ParticleSystem::getParticles() const {
    std::vector<Particle> particles;
    for (int idx : activeParticles) {
        particles.push_back((*particlePool)[idx]);
    }
    return particles;
}

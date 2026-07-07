#include "ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <random>

ParticleSystem::ParticleSystem() : rng(rd()) {
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void ParticleSystem::spawn(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    for (int i = 0; i < count; ++i) {
        float vx = (dist(rng) - 1.0f) * 200.0f;
        float vy = (dist(rng) - 1.0f) * 200.0f;
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
    std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> velDist(0.5f, 1.5f);
    std::uniform_real_distribution<float> angleDist(-0.2f, 0.2f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float angle = angleDist(rng);
        float speed = velDist(rng) * 30.0f;
        sf::Vector2f vel(std::cos(angle) * speed, 20.0f + std::abs(std::sin(angle)) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.3f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
        p.type = ParticleType::Dust;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnSparks(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(3.0f, 5.0f);
    std::uniform_real_distribution<float> speedDist(200.0f, 600.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 6.283185f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float speed = speedDist(rng);
        float angle = angleDist(rng);
        sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.2f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
        p.type = ParticleType::Spark;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnScoreSparkle(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(4.0f, 6.0f);
    std::uniform_real_distribution<float> wobbleDist(-0.1f, 0.1f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = sf::Vector2f(wobbleDist(rng) * 100.f, -100.f);
        p.lifetime = 0.6f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
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

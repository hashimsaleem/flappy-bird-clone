#include "ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <random>

ParticleSystem::ParticleSystem() {
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void ParticleSystem::spawn(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    for (int i = 0; i < count; ++i) {
        float vx = (dist(rng) - 1.0f) * 200.0f;
        float vy = (dist(rng) - 1.0f) * 200.0f;
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setPosition(pos);
        p.velocity = velocity + sf::Vector2f(vx, vy);
        p.lifetime = 1.0f;
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

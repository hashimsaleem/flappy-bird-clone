#include "VisualEffectManager.hpp"
#include "core/ConfigValues.hpp"
#include <cmath>

VisualEffectManager::VisualEffectManager(const ConfigValues& cfg) : cfg(cfg) {
    clouds = std::make_unique<CloudSystem>(cfg);
    particles = std::make_unique<ParticleSystem>();
    environment = std::make_unique<EnvironmentRenderer>(cfg);
}

void VisualEffectManager::update(float dt, float pipeSpeed) {
    clouds->update(dt);
    particles->update(dt);
    groundScrollOffset += pipeSpeed * dt;
    skyTimer += dt;
    cloudOffset += cfg.backgroundSpeed * dt;
}

void VisualEffectManager::draw(sf::RenderWindow& window) {
    clouds->draw(window);
    particles->draw(window);
    environment->draw(window, groundScrollOffset, skyTimer);
}

std::vector<Particle> VisualEffectManager::getParticles() const {
    return particles->getParticles();
}

void VisualEffectManager::spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    particles->spawn(pos, count, velocity);
}

void VisualEffectManager::spawnDust(sf::Vector2f pos, int count) {
    particles->spawnDust(pos, count);
}

void VisualEffectManager::spawnSparks(sf::Vector2f pos, int count) {
    particles->spawnSparks(pos, count);
}

void VisualEffectManager::spawnScoreSparkle(sf::Vector2f pos, int count) {
    particles->spawnScoreSparkle(pos, count);
}

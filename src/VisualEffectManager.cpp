#include "VisualEffectManager.hpp"
#include "Config.hpp"
#include <cmath>
 
VisualEffectManager::VisualEffectManager() {
    clouds = std::make_unique<CloudSystem>();
    particles = std::make_unique<ParticleSystem>();
    environment = std::make_unique<EnvironmentRenderer>();
}
 
void VisualEffectManager::update(float dt) {
    clouds->update(dt);
    particles->update(dt);
}
 
void VisualEffectManager::update(float dt, float pipeSpeed) {
    clouds->update(dt);
    particles->update(dt);
    groundScrollOffset += pipeSpeed * dt;
    skyTimer += dt;
    cloudOffset += Config::BACKGROUND_SPEED * dt;
}
 
void VisualEffectManager::draw(sf::RenderWindow& window) {
    clouds->draw(window);
    particles->draw(window);
    environment->draw(window, groundScrollOffset, skyTimer);
}
 
void VisualEffectManager::spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    particles->spawn(pos, count, velocity);
}

#pragma once
#include <memory>
#include "CloudSystem.hpp"
#include "ParticleSystem.hpp"
#include "EnvironmentRenderer.hpp"

class VisualEffectManager {
public:
    VisualEffectManager();
    void update(float dt, float currentPipeSpeed);
    void draw(sf::RenderWindow& window);
    void spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void setGroundScrollOffset(float offset) { groundScrollOffset = offset; }
    void setCloudOffset(float offset) { cloudOffset = offset; }
    void setSkyTimer(float timer) { skyTimer = timer; }
    const std::vector<Particle>& getParticles() const { return particles->getParticles(); }

private:
    std::unique_ptr<CloudSystem> clouds;
    std::unique_ptr<ParticleSystem> particles;
    std::unique_ptr<EnvironmentRenderer> environment;
    float groundScrollOffset = 0.f;
    float cloudOffset = 0.f;
    float skyTimer = 0.f;
};

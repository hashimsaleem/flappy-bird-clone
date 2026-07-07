#pragma once
#include <memory>
#include "visual/CloudSystem.hpp"
#include "visual/ParticleSystem.hpp"
#include "visual/EnvironmentRenderer.hpp"

class VisualEffectManager {
public:
    VisualEffectManager();
    void update(float dt, float currentPipeSpeed);
    void draw(sf::RenderWindow& window);
    void spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void spawnDust(sf::Vector2f pos, int count);
    void spawnSparks(sf::Vector2f pos, int count);
    void spawnScoreSparkle(sf::Vector2f pos, int count);
    void setGroundScrollOffset(float offset) { groundScrollOffset = offset; }
    float getGroundScrollOffset() const { return groundScrollOffset; }
    void setCloudOffset(float offset) { cloudOffset = offset; }
    float getCloudOffset() const { return cloudOffset; }
    void setSkyTimer(float timer) { skyTimer = timer; }
    float getSkyTimer() const { return skyTimer; }
    std::vector<Particle> getParticles() const;

private:
    std::unique_ptr<CloudSystem> clouds;
    std::unique_ptr<ParticleSystem> particles;
    std::unique_ptr<EnvironmentRenderer> environment;
    float groundScrollOffset = 0.f;
    float cloudOffset = 0.f;
    float skyTimer = 0.f;
};

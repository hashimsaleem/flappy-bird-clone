#include <gtest/gtest.h>
#include "systems/VisualEffectManager.hpp"
#include "core/Config.hpp"
#include "core/ConfigValues.hpp"
#include <SFML/Graphics.hpp>

// --- VisualEffectManager constructs without crash ---

TEST(VisualEffectManagerTest, Constructs) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    EXPECT_NO_THROW(manager.update(0.01f, 200.f));
}

// --- getParticles empty on new manager ---

TEST(VisualEffectManagerTest, GetParticlesEmptyInitially) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    auto particles = manager.getParticles();
    EXPECT_TRUE(particles.empty());
}

// --- spawnParticles creates particles ---

TEST(VisualEffectManagerTest, SpawnParticles) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.spawnParticles(sf::Vector2f(100.f, 100.f), 5, sf::Vector2f(50.f, 50.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 5u);
}

// --- spawnParticles at different positions ---

TEST(VisualEffectManagerTest, SpawnParticlesAtPosition) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    sf::Vector2f pos(300.f, 400.f);
    manager.spawnParticles(pos, 3, sf::Vector2f(0.f, 0.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 3u);
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().x, 300.f);
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().y, 400.f);
}

// --- getGroundScrollOffset starts at 0 ---

TEST(VisualEffectManagerTest, GroundScrollOffsetStartsAtZero) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 0.f);
}

// --- getGroundScrollOffset updates with pipe speed ---

TEST(VisualEffectManagerTest, GroundScrollOffsetUpdates) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.update(1.0f, 200.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 200.f);
}

// --- setGroundScrollOffset changes offset ---

TEST(VisualEffectManagerTest, SetGroundScrollOffset) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.setGroundScrollOffset(500.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 500.f);
}

// --- getCloudOffset starts at 0 ---

TEST(VisualEffectManagerTest, CloudOffsetStartsAtZero) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    EXPECT_FLOAT_EQ(manager.getCloudOffset(), 0.f);
}

// --- getSkyTimer starts at 0 ---

TEST(VisualEffectManagerTest, SkyTimerStartsAtZero) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    EXPECT_FLOAT_EQ(manager.getSkyTimer(), 0.f);
}

// --- setSkyTimer changes timer ---

TEST(VisualEffectManagerTest, SetSkyTimer) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.setSkyTimer(42.0f);
    EXPECT_FLOAT_EQ(manager.getSkyTimer(), 42.0f);
}

// --- update increases sky timer ---

TEST(VisualEffectManagerTest, UpdateIncreasesSkyTimer) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.update(1.0f, 0.f);
    EXPECT_GT(manager.getSkyTimer(), 0.f);
}

// --- Multiple spawn accumulates particles ---

TEST(VisualEffectManagerTest, MultipleSpawnsAccumulate) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.spawnParticles(sf::Vector2f(0.f, 0.f), 3, sf::Vector2f(10.f, 10.f));
    manager.spawnParticles(sf::Vector2f(50.f, 50.f), 2, sf::Vector2f(-10.f, -10.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 5u);
}

// --- Particles expire after update ---

TEST(VisualEffectManagerTest, ParticlesExpire) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.spawnParticles(sf::Vector2f(0.f, 0.f), 10, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(manager.getParticles().size(), 10u);
    // Lifetime is 1.0f
    manager.update(1.5f, 0.f);
    EXPECT_TRUE(manager.getParticles().empty());
}

// --- update with zero dt does not crash ---

TEST(VisualEffectManagerTest, UpdateZeroDt) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    EXPECT_NO_THROW(manager.update(0.0f, 0.f));
}

// --- update with large pipe speed ---

TEST(VisualEffectManagerTest, UpdateLargePipeSpeed) {
    ConfigValues cfg;
    VisualEffectManager manager(cfg);
    manager.update(0.01f, 1000.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 10.f);
}

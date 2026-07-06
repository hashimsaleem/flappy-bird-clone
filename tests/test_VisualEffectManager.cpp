#include <gtest/gtest.h>
#include "systems/VisualEffectManager.hpp"
#include "core/Config.hpp"
#include <SFML/Graphics.hpp>

// --- VisualEffectManager constructs without crash ---

TEST(VisualEffectManagerTest, Constructs) {
    VisualEffectManager manager;
    EXPECT_NO_THROW(manager.update(0.01f, 200.f));
}

// --- getParticles empty on new manager ---

TEST(VisualEffectManagerTest, GetParticlesEmptyInitially) {
    VisualEffectManager manager;
    auto particles = manager.getParticles();
    EXPECT_TRUE(particles.empty());
}

// --- spawnParticles creates particles ---

TEST(VisualEffectManagerTest, SpawnParticles) {
    VisualEffectManager manager;
    manager.spawnParticles(sf::Vector2f(100.f, 100.f), 5, sf::Vector2f(50.f, 50.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 5u);
}

// --- spawnParticles at different positions ---

TEST(VisualEffectManagerTest, SpawnParticlesAtPosition) {
    VisualEffectManager manager;
    sf::Vector2f pos(300.f, 400.f);
    manager.spawnParticles(pos, 3, sf::Vector2f(0.f, 0.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 3u);
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().x, 300.f);
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().y, 400.f);
}

// --- getGroundScrollOffset starts at 0 ---

TEST(VisualEffectManagerTest, GroundScrollOffsetStartsAtZero) {
    VisualEffectManager manager;
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 0.f);
}

// --- getGroundScrollOffset updates with pipe speed ---

TEST(VisualEffectManagerTest, GroundScrollOffsetUpdates) {
    VisualEffectManager manager;
    manager.update(1.0f, 200.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 200.f);
}

// --- setGroundScrollOffset changes offset ---

TEST(VisualEffectManagerTest, SetGroundScrollOffset) {
    VisualEffectManager manager;
    manager.setGroundScrollOffset(500.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 500.f);
}

// --- getCloudOffset starts at 0 ---

TEST(VisualEffectManagerTest, CloudOffsetStartsAtZero) {
    VisualEffectManager manager;
    EXPECT_FLOAT_EQ(manager.getCloudOffset(), 0.f);
}

// --- getSkyTimer starts at 0 ---

TEST(VisualEffectManagerTest, SkyTimerStartsAtZero) {
    VisualEffectManager manager;
    EXPECT_FLOAT_EQ(manager.getSkyTimer(), 0.f);
}

// --- setSkyTimer changes timer ---

TEST(VisualEffectManagerTest, SetSkyTimer) {
    VisualEffectManager manager;
    manager.setSkyTimer(42.0f);
    EXPECT_FLOAT_EQ(manager.getSkyTimer(), 42.0f);
}

// --- update increases sky timer ---

TEST(VisualEffectManagerTest, UpdateIncreasesSkyTimer) {
    VisualEffectManager manager;
    manager.update(1.0f, 0.f);
    EXPECT_GT(manager.getSkyTimer(), 0.f);
}

// --- Multiple spawn accumulates particles ---

TEST(VisualEffectManagerTest, MultipleSpawnsAccumulate) {
    VisualEffectManager manager;
    manager.spawnParticles(sf::Vector2f(0.f, 0.f), 3, sf::Vector2f(10.f, 10.f));
    manager.spawnParticles(sf::Vector2f(50.f, 50.f), 2, sf::Vector2f(-10.f, -10.f));
    auto particles = manager.getParticles();
    EXPECT_EQ(particles.size(), 5u);
}

// --- Particles expire after update ---

TEST(VisualEffectManagerTest, ParticlesExpire) {
    VisualEffectManager manager;
    manager.spawnParticles(sf::Vector2f(0.f, 0.f), 10, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(manager.getParticles().size(), 10u);
    // Lifetime is 1.0f
    manager.update(1.5f, 0.f);
    EXPECT_TRUE(manager.getParticles().empty());
}

// --- update with zero dt does not crash ---

TEST(VisualEffectManagerTest, UpdateZeroDt) {
    VisualEffectManager manager;
    EXPECT_NO_THROW(manager.update(0.0f, 0.f));
}

// --- update with large pipe speed ---

TEST(VisualEffectManagerTest, UpdateLargePipeSpeed) {
    VisualEffectManager manager;
    manager.update(0.01f, 1000.f);
    EXPECT_FLOAT_EQ(manager.getGroundScrollOffset(), 10.f);
}

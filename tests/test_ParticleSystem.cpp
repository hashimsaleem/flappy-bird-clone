#include <gtest/gtest.h>
#include "visual/ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigValues.hpp"

// --- ParticleSystem starts with no active particles ---

TEST(ParticleSystemTest, StartsEmpty) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    EXPECT_EQ(system.getActiveCount(), 0u);
}

// --- ParticleSystem spawn creates particles ---

TEST(ParticleSystemTest, SpawnCreatesParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(100.f, 100.f), 5, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(system.getActiveCount(), 5u);
}

// --- ParticleSystem spawn different counts ---

TEST(ParticleSystemTest, SpawnDifferentCounts) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(0.f, 0.f), 1, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(system.getActiveCount(), 1u);

    ConfigValues cfg2;
    ParticleSystem system2(cfg2);
    system2.spawn(sf::Vector2f(0.f, 0.f), 20, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(system2.getActiveCount(), 20u);
}

// --- ParticleSystem update with particles ---

TEST(ParticleSystemTest, UpdateParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(100.f, 100.f), 3, sf::Vector2f(50.f, 50.f));
    EXPECT_EQ(system.getActiveCount(), 3u);
    system.update(0.01f);
    EXPECT_EQ(system.getActiveCount(), 3u); // Not expired yet (lifetime = 1.0f)
}

// --- Particles expire after lifetime ---

TEST(ParticleSystemTest, ParticlesExpireAfterLifetime) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(100.f, 100.f), 5, sf::Vector2f(0.f, 0.f));
    EXPECT_EQ(system.getActiveCount(), 5u);
    // Lifetime is 1.0f per particle
    system.update(1.1f);
    // All particles should be expired and removed
    EXPECT_EQ(system.getActiveCount(), 0u);
}

// --- getParticles returns correct count ---

TEST(ParticleSystemTest, GetParticlesReturnsCorrectCount) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    EXPECT_TRUE(system.getParticles().empty());

    system.spawn(sf::Vector2f(50.f, 50.f), 4, sf::Vector2f(10.f, 10.f));
    auto particles = system.getParticles();
    EXPECT_EQ(particles.size(), 4u);
}

// --- getParticles returns particles with correct positions ---

TEST(ParticleSystemTest, GetParticlesHasCorrectPositions) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    sf::Vector2f spawnPos(200.f, 300.f);
    system.spawn(spawnPos, 2, sf::Vector2f(0.f, 0.f));
    auto particles = system.getParticles();
    EXPECT_EQ(particles.size(), 2u);
    // Particle struct has a shape with position
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().x, 200.f);
    EXPECT_FLOAT_EQ(particles[0].shape.getPosition().y, 300.f);
}

// --- Multiple spawn calls accumulate ---

TEST(ParticleSystemTest, MultipleSpawnsAccumulate) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(0.f, 0.f), 3, sf::Vector2f(10.f, 10.f));
    system.spawn(sf::Vector2f(50.f, 50.f), 2, sf::Vector2f(-10.f, -10.f));
    EXPECT_EQ(system.getActiveCount(), 5u);
}

// --- Update with zero dt does not remove particles ---

TEST(ParticleSystemTest, UpdateZeroDtKeepsParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(0.f, 0.f), 5, sf::Vector2f(0.f, 0.f));
    system.update(0.0f);
    EXPECT_EQ(system.getActiveCount(), 5u);
}

// --- Large update removes all particles ---

TEST(ParticleSystemTest, LargeUpdateRemovesAllParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawn(sf::Vector2f(0.f, 0.f), 10, sf::Vector2f(0.f, 0.f));
    system.update(2.0f);
    EXPECT_EQ(system.getActiveCount(), 0u);
}

// --- spawnDust creates dust particles ---

TEST(ParticleSystemTest, SpawnDustCreatesParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnDust(sf::Vector2f(100.f, 500.f), 4);
    EXPECT_EQ(system.getActiveCount(), 4u);
}

// --- Dust particles float downward ---

TEST(ParticleSystemTest, DustFloatsDownward) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnDust(sf::Vector2f(100.f, 100.f), 1);
    auto particles = system.getParticles();
    EXPECT_GT(particles[0].velocity.y, 0.f);
}

// --- spawnSparks creates spark particles ---

TEST(ParticleSystemTest, SpawnSparksCreatesParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnSparks(sf::Vector2f(200.f, 200.f), 6);
    EXPECT_EQ(system.getActiveCount(), 6u);
}

// --- Sparks have explosive burst velocities ---

TEST(ParticleSystemTest, SparksExplosiveBurst) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnSparks(sf::Vector2f(300.f, 300.f), 10);
    auto particles = system.getParticles();
    for (const auto& p : particles) {
        float speed = std::sqrt(p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y);
        EXPECT_GE(speed, 200.f);
        EXPECT_LE(speed, 600.f);
    }
}

// --- spawnScoreSparkle creates bubble particles ---

TEST(ParticleSystemTest, SpawnScoreSparkleCreatesParticles) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnScoreSparkle(sf::Vector2f(400.f, 100.f), 3);
    EXPECT_EQ(system.getActiveCount(), 3u);
}

// --- Score bubbles rise upward ---

TEST(ParticleSystemTest, ScoreBubbleRisesUpward) {
    ConfigValues cfg;
    ParticleSystem system(cfg);
    system.spawnScoreSparkle(sf::Vector2f(500.f, 300.f), 1);
    auto particles = system.getParticles();
    EXPECT_LT(particles[0].velocity.y, 0.f);
}

#include <gtest/gtest.h>
#include "PowerUp.hpp"
#include "Config.hpp"

class PowerUpTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize a power-up at (0, 0) with INVINCIBILITY type
        powerUp = std::make_unique<PowerUp>(0.f, 0.f, PowerUpType::INVINCIBILITY);
    }

    std::unique_ptr<PowerUp> powerUp;
};

// --- PowerUp starts at the correct position ---
TEST_F(PowerUpTest, StartsAtCorrectPosition) {
    EXPECT_FLOAT_EQ(powerUp->getX(), 0.f);
    EXPECT_FLOAT_EQ(powerUp->getY(), 0.f);
}

// --- PowerUp has correct type ---
TEST_F(PowerUpTest, HasCorrectType) {
    EXPECT_EQ(powerUp->getType(), PowerUpType::INVINCIBILITY);
}

// --- Update moves power-up according to velocity ---
TEST_F(PowerUpTest, UpdateDoesNotMoveStaticPowerUp) {
    float initialX = powerUp->getX();
    float initialY = powerUp->getY();
    float expectedX = initialX + powerUp->getVelocityX() * 1.0f;
    powerUp->update(1.0f);
    EXPECT_FLOAT_EQ(powerUp->getX(), expectedX);
    EXPECT_FLOAT_EQ(powerUp->getY(), initialY);
}

// --- Collision detection works correctly ---
TEST_F(PowerUpTest, CheckCollisionWorks) {
    sf::FloatRect birdBounds({0.f, 0.f}, {10.f, 10.f});
    EXPECT_TRUE(powerUp->checkCollision(birdBounds));

    sf::FloatRect distantBounds({50.f, 50.f}, {10.f, 10.f});
    EXPECT_FALSE(powerUp->checkCollision(distantBounds));
}

// --- isOffScreen returns true when outside screen bounds ---
TEST_F(PowerUpTest, IsOffScreenWorks) {
    // Assuming screen size from Config
    float screenW = static_cast<float>(Config::SCREEN_WIDTH);
    float screenH = static_cast<float>(Config::SCREEN_HEIGHT);

    // Position it off-screen to the left (isOffScreen checks x < -50)
    powerUp->reset(-60.f, 0.f);
    EXPECT_TRUE(powerUp->isOffScreen());

    // Position it on-screen
    powerUp->reset(100.f, 100.f);
    EXPECT_FALSE(powerUp->isOffScreen());
}

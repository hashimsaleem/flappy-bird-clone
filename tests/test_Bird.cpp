#include <gtest/gtest.h>
#include "Bird.hpp"
#include "Config.hpp"

// Bird creates an sf::Sprite using a placeholder texture — this works without a window.

class BirdTest : public ::testing::Test {
protected:
    void SetUp() override {
        bird.load(Config::BIRD_PATH);
    }

    Bird bird;
};

// --- Bird starts at configured position ---

TEST_F(BirdTest, StartsAtConfiguredPosition) {
    EXPECT_FLOAT_EQ(bird.getX(), Config::BIRD_START_X);
}

// --- Initial velocity is zero ---

TEST_F(BirdTest, InitialVelocityIsZero) {
    EXPECT_FLOAT_EQ(bird.getVelocity(), 0.0f);
}

// --- Flap resets velocity to jump strength ---

TEST_F(BirdTest, FlapSetsJumpVelocity) {
    bird.flap();
    EXPECT_FLOAT_EQ(bird.getVelocity(), Config::JUMP_STRENGTH);
}

// --- Flap multiple times resets velocity each time ---

TEST_F(BirdTest, FlapResetsVelocity) {
    bird.update(0.1f);          // gravity pulls down
    float afterGravity = bird.getVelocity();
    EXPECT_GT(afterGravity, 0.0f); // velocity should be positive (falling)

    bird.flap();
    EXPECT_FLOAT_EQ(bird.getVelocity(), Config::JUMP_STRENGTH);
}

// --- Update applies gravity (positive delta) ---

TEST_F(BirdTest, GravityIncreasesVelocityDownward) {
    float v0 = bird.getVelocity();
    bird.update(1.0f);
    float v1 = bird.getVelocity();
    EXPECT_GT(v1, v0);  // velocity should be more positive (falling faster)
}

// --- Update moves bird downward under gravity ---

TEST_F(BirdTest, GravityMovesBirdDownward) {
    // We need getY() — but Bird doesn't expose posY directly.
    // Instead, we verify velocity increased and bounding box moved.
    sf::FloatRect bounds0 = bird.getBoundingBox();

    bird.update(0.5f);

    sf::FloatRect bounds1 = bird.getBoundingBox();
    EXPECT_GT(bounds1.position.y, bounds0.position.y); // moved down
}

// --- After flap, bird moves upward ---

TEST_F(BirdTest, FlapMovesBirdUpward) {
    bird.flap();
    sf::FloatRect bounds0 = bird.getBoundingBox();

    bird.update(0.05f); // small dt so we catch upward movement before gravity reverses

    sf::FloatRect bounds1 = bird.getBoundingBox();
    EXPECT_LT(bounds1.position.y, bounds0.position.y); // moved up
}

// --- Bounding box has correct dimensions ---

TEST_F(BirdTest, BoundingBoxHasCorrectSize) {
    sf::FloatRect bounds = bird.getBoundingBox();
    EXPECT_FLOAT_EQ(bounds.size.x, Config::BIRD_WIDTH);
    EXPECT_FLOAT_EQ(bounds.size.y, Config::BIRD_HEIGHT);
}

// --- Bounding box is centered on bird position ---

TEST_F(BirdTest, BoundingBoxCenteredOnPosition) {
    sf::FloatRect bounds = bird.getBoundingBox();
    float expectedLeft = Config::BIRD_START_X - Config::BIRD_WIDTH / 2.0f;
    EXPECT_FLOAT_EQ(bounds.position.x, expectedLeft);
}

// --- Reset returns bird to initial state ---

TEST_F(BirdTest, ResetRestoresInitialState) {
    bird.flap();
    bird.update(0.5f);
    float velAfter = bird.getVelocity();
    EXPECT_NE(velAfter, 0.0f); // Bird is moving

    bird.reset();
    bird.load(Config::BIRD_PATH);
    EXPECT_FLOAT_EQ(bird.getVelocity(), 0.0f);
    EXPECT_FLOAT_EQ(bird.getX(), Config::BIRD_START_X);
}

// --- getVelocity returns current velocity ---

TEST_F(BirdTest, GetVelocityReflectsGravity) {
    EXPECT_FLOAT_EQ(bird.getVelocity(), 0.0f);
    bird.update(0.5f);
    float expected = 0.0f + Config::GRAVITY * 0.5f;
    EXPECT_FLOAT_EQ(bird.getVelocity(), expected);
}

// --- Two updates compound correctly ---

TEST_F(BirdTest, UpdateCompoundsGravity) {
    float dt = 0.1f;
    bird.update(dt);
    float v1 = bird.getVelocity(); // v1 = 0 + GRAVITY * dt

    bird.update(dt);
    float v2 = bird.getVelocity(); // v2 = v1 + GRAVITY * dt

    EXPECT_FLOAT_EQ(v2, v1 + Config::GRAVITY * dt);
}

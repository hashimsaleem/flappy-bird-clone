#include <gtest/gtest.h>
#include "Pipe.hpp"
#include "Config.hpp"

class PipeTest : public ::testing::Test {
protected:
    // Default pipe: x=400, y=300, gap=150, speed=200
    Pipe defaultPipe{400.0f, 300.0f, 150.0f, 200.0f, PipeType::STATIC};
};

// --- Pipe starts at the given X position ---

TEST_F(PipeTest, StartsAtGivenX) {
    EXPECT_FLOAT_EQ(defaultPipe.getX(), 400.0f);
}

// --- Update moves pipe left ---

TEST_F(PipeTest, UpdateMovesPipeLeft) {
    float x0 = defaultPipe.getX();
    defaultPipe.update(1.0f);
    float x1 = defaultPipe.getX();
    // velocity = -200, so after 1 second: x1 = x0 - 200
    EXPECT_FLOAT_EQ(x1, x0 - 200.0f);
}

// --- Update with zero delta does not move ---

TEST_F(PipeTest, UpdateWithZeroDeltaDoesNotMove) {
    float x0 = defaultPipe.getX();
    defaultPipe.update(0.0f);
    EXPECT_FLOAT_EQ(defaultPipe.getX(), x0);
}

// --- Update with partial delta moves proportionally ---

TEST_F(PipeTest, UpdateWithPartialDeltaMovesProportionally) {
    float x0 = defaultPipe.getX();
    defaultPipe.update(0.5f);
    // velocity = -200, so after 0.5s: x1 = x0 - 100
    EXPECT_FLOAT_EQ(defaultPipe.getX(), x0 - 100.0f);
}

// --- Pipe with custom speed moves at that speed ---

TEST_F(PipeTest, CustomSpeed) {
    Pipe fastPipe{500.0f, 300.0f, 150.0f, 400.0f, PipeType::STATIC};
    fastPipe.update(1.0f);
    // velocity = -400, so after 1s: x = 500 - 400 = 100
    EXPECT_FLOAT_EQ(fastPipe.getX(), 100.0f);
}

// --- Pipe with zero speed does not move ---

TEST_F(PipeTest, ZeroSpeedPipeDoesNotMove) {
    Pipe stationary{400.0f, 300.0f, 150.0f, 0.0f, PipeType::STATIC};
    stationary.update(10.0f);
    EXPECT_FLOAT_EQ(stationary.getX(), 400.0f);
}

// --- isOffScreen returns false for pipe on screen ---

TEST_F(PipeTest, IsNotOffScreenInitially) {
    EXPECT_FALSE(defaultPipe.isOffScreen());
}

// --- isOffScreen returns true after pipe passes left edge ---

TEST_F(PipeTest, IsOffScreenAfterMovingPastLeftEdge) {
    // Pipe starts at x=400, speed=200. Config::PIPE_WIDTH = 60.
    // It's off-screen when x + 60 < 0 → x < -60
    // Time needed: just past (400 - (-60)) / 200 = 2.3 seconds
    defaultPipe.update(2.35f); // x = 400 - 200*2.35 = 400 - 470 = -70 < -60 ✓
    EXPECT_TRUE(defaultPipe.isOffScreen());
}

// --- isOffScreen at edge boundary ---

TEST_F(PipeTest, IsOffScreenBoundary) {
    // Just before off-screen: x ≈ -60 - epsilon
    defaultPipe.update(2.29f); // x ≈ 400 - 200*2.29 = 400 - 458 = -58
    EXPECT_FALSE(defaultPipe.isOffScreen());

    defaultPipe.update(0.02f); // x ≈ -58 - 4 = -62
    EXPECT_TRUE(defaultPipe.isOffScreen());
}

// --- passed flag starts false ---

TEST_F(PipeTest, PassedFlagStartsFalse) {
    EXPECT_FALSE(defaultPipe.passed);
}

// --- Moving pipe oscillates but still moves left ---

TEST_F(PipeTest, MovingPipeStillMovesLeft) {
    Pipe movingPipe{500.0f, 300.0f, 150.0f, 200.0f, PipeType::MOVING};
    movingPipe.update(1.0f);
    // After 1s at speed 200: should have moved left roughly 200 units
    // (oscillation adds Y variation but doesn't affect X)
    EXPECT_LT(movingPipe.getX(), 400.0f);
    EXPECT_GT(movingPipe.getX(), 250.0f); // within reasonable range
}

// --- Pipe with tall gap still works ---

TEST_F(PipeTest, LargeGapPipe) {
    Pipe wideGapPipe{400.0f, 300.0f, 300.0f, 200.0f, PipeType::STATIC};
    wideGapPipe.update(1.0f);
    EXPECT_FLOAT_EQ(wideGapPipe.getX(), 200.0f);
}

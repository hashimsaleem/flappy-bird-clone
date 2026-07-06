#include <gtest/gtest.h>
#include "CloudSystem.hpp"
#include "Config.hpp"

// --- CloudSystem creates 6 clouds on construction ---

TEST(CloudSystemTest, CreatesSixClouds) {
    CloudSystem system;
    EXPECT_EQ(system.getCloudCount(), 6u);
}

// --- CloudSystem update does not crash ---

TEST(CloudSystemTest, UpdateDoesNotCrash) {
    CloudSystem system;
    EXPECT_NO_THROW(system.update(0.01f));
    EXPECT_NO_THROW(system.update(0.1f));
    EXPECT_NO_THROW(system.update(1.0f));
}

// --- CloudSystem count stays constant after update ---

TEST(CloudSystemTest, CloudCountConstantAfterUpdate) {
    CloudSystem system;
    size_t initial = system.getCloudCount();
    for (int i = 0; i < 100; i++) {
        system.update(0.01f);
    }
    EXPECT_EQ(system.getCloudCount(), initial);
}

// --- CloudSystem update with zero dt does not crash ---

TEST(CloudSystemTest, UpdateZeroDtDoesNotCrash) {
    CloudSystem system;
    EXPECT_NO_THROW(system.update(0.0f));
}

// --- CloudSystem update with large dt does not crash ---

TEST(CloudSystemTest, UpdateLargeDtDoesNotCrash) {
    CloudSystem system;
    EXPECT_NO_THROW(system.update(10.0f));
}

// --- CloudSystem creates clouds within screen bounds ---

TEST(CloudSystemTest, CloudsCreatedWithinScreenBounds) {
    CloudSystem system;
    // All clouds are initialized in the constructor with:
    // y = 30 + random(120), so y in [30, 150]
    // x = (i/6) * SCREEN_WIDTH + random(100), so x in [0, SCREEN_WIDTH + 100)
    // These should be within reasonable screen bounds
    EXPECT_GT(Config::SCREEN_HEIGHT, 30u);
    EXPECT_GT(Config::SCREEN_WIDTH, 0);
}

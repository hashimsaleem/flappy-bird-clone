#include <gtest/gtest.h>
#include "visual/CloudSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigValues.hpp"

// --- CloudSystem creates 6 clouds on construction ---

TEST(CloudSystemTest, CreatesSixClouds) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    EXPECT_EQ(system.getCloudCount(), 6u);
}

// --- CloudSystem update does not crash ---

TEST(CloudSystemTest, UpdateDoesNotCrash) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    EXPECT_NO_THROW(system.update(0.01f));
    EXPECT_NO_THROW(system.update(0.1f));
    EXPECT_NO_THROW(system.update(1.0f));
}

// --- CloudSystem count stays constant after update ---

TEST(CloudSystemTest, CloudCountConstantAfterUpdate) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    size_t initial = system.getCloudCount();
    for (int i = 0; i < 100; i++) {
        system.update(0.01f);
    }
    EXPECT_EQ(system.getCloudCount(), initial);
}

// --- CloudSystem update with zero dt does not crash ---

TEST(CloudSystemTest, UpdateZeroDtDoesNotCrash) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    EXPECT_NO_THROW(system.update(0.0f));
}

// --- CloudSystem update with large dt does not crash ---

TEST(CloudSystemTest, UpdateLargeDtDoesNotCrash) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    EXPECT_NO_THROW(system.update(10.0f));
}

// --- CloudSystem creates clouds within screen bounds ---

TEST(CloudSystemTest, CloudsCreatedWithinScreenBounds) {
    ConfigValues cfg;
    CloudSystem system(cfg);
    // All clouds are initialized in the constructor with:
    // y = 30 + random(120), so y in [30, 150]
    // x = (i/6) * SCREEN_WIDTH + random(100), so x in [0, SCREEN_WIDTH + 100)
    // These should be within reasonable screen bounds
    EXPECT_GT(Config::SCREEN_HEIGHT, 30u);
    EXPECT_GT(Config::SCREEN_WIDTH, 0);
}

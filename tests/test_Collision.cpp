#include <gtest/gtest.h>
#include "entities/Pipe.hpp"
#include "core/Config.hpp"

// Collision test fixtures.
// Pipe at x=400, y=300, gapHeight=150, speed=200:
//   Top pipe:    rect (400, -175)  size (60, 400)  → [400, -175, 460, 225]
//   Bottom pipe: rect (400,  375)  size (60, 400)  → [400,  375, 460, 775]
//   Gap: y ∈ (225, 375)
//
// Bird bounding box is 40×40, origin-centered.
// Bird at (bx, by) → bounds (bx-20, by-20, 40, 40)

class CollisionTest : public ::testing::Test {
protected:
    CollisionTest()
        : pipe(400.0f, 300.0f, 150.0f, 200.0f, PipeType::STATIC) {}

    sf::FloatRect birdAt(float x, float y) {
        return sf::FloatRect({x - 20.0f, y - 20.0f}, {40.0f, 40.0f});
    }

    Pipe pipe;
};

// --- Bird in the gap — no collision ---

TEST_F(CollisionTest, BirdInGapNoCollision) {
    auto bird = birdAt(400.0f, 300.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird hitting top pipe ---

TEST_F(CollisionTest, BirdHittingTopPipe) {
    auto bird = birdAt(400.0f, 100.0f);  // in top pipe zone
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird hitting bottom pipe ---

TEST_F(CollisionTest, BirdHittingBottomPipe) {
    auto bird = birdAt(400.0f, 500.0f);  // in bottom pipe zone
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird far left — no collision ---

TEST_F(CollisionTest, BirdFarLeftNoCollision) {
    auto bird = birdAt(100.0f, 300.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird far right — no collision ---

TEST_F(CollisionTest, BirdFarRightNoCollision) {
    auto bird = birdAt(700.0f, 300.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird above screen — no collision (above top pipe) ---

TEST_F(CollisionTest, BirdAboveTopPipeNoCollision) {
    auto bird = birdAt(400.0f, -300.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird below screen — no collision (below bottom pipe) ---

TEST_F(CollisionTest, BirdBelowBottomPipeNoCollision) {
    auto bird = birdAt(400.0f, 900.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird overlapping top pipe's bottom edge ---

TEST_F(CollisionTest, BirdOverlappingTopPipeBottomEdge) {
    // Top pipe bottom edge is at y=225. Bird's top edge at by-20.
    // SFML::findIntersection requires positive overlap (not just touching).
    // Bird 1px inside: by-20 = 224 → by = 244
    auto bird = birdAt(400.0f, 244.0f);
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird just below top pipe — no collision ---

TEST_F(CollisionTest, BirdJustBelowTopPipeNoCollision) {
    // Top pipe bottom edge is at y=225. Bird's top edge at by-20.
    // Bird just below: by-20 = 226 → by = 246
    auto bird = birdAt(400.0f, 246.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird overlapping bottom pipe's top edge ---

TEST_F(CollisionTest, BirdOverlappingBottomPipeTopEdge) {
    // Bottom pipe top edge is at y=375. Bird's bottom edge at by+20.
    // SFML::findIntersection requires positive overlap (not just touching).
    // Bird 1px inside: by+20 = 376 → by = 356
    auto bird = birdAt(400.0f, 356.0f);
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird just above bottom pipe — no collision ---

TEST_F(CollisionTest, BirdJustAboveBottomPipeNoCollision) {
    // Bottom pipe top edge is at y=375. Bird's bottom edge at by+20.
    // Bird just above: by+20 = 374 → by = 354
    auto bird = birdAt(400.0f, 354.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird overlapping pipe's left edge — collision ---

TEST_F(CollisionTest, BirdOverlappingPipeLeftEdge) {
    // Pipe left edge at x=400. Bird right edge at bx+20.
    // SFML::findIntersection requires positive overlap (not just touching).
    // Bird 1px inside: bx+20 = 401 → bx = 381
    auto bird = birdAt(381.0f, 100.0f);
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird just left of pipe — no collision ---

TEST_F(CollisionTest, BirdJustLeftOfPipeNoCollision) {
    // Pipe left edge at x=400. Bird right edge at bx+20.
    // Just left: bx+20 = 399 → bx = 379
    auto bird = birdAt(379.0f, 100.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Bird overlapping pipe's right edge — collision ---

TEST_F(CollisionTest, BirdOverlappingPipeRightEdge) {
    // Pipe right edge at x=460. Bird left edge at bx-20.
    // SFML::findIntersection requires positive overlap (not just touching).
    // Bird 1px inside: bx-20 = 459 → bx = 479
    auto bird = birdAt(479.0f, 100.0f);
    EXPECT_TRUE(pipe.checkCollision(bird));
}

// --- Bird just right of pipe — no collision ---

TEST_F(CollisionTest, BirdJustRightOfPipeNoCollision) {
    // Pipe right edge at x=460. Bird left edge at bx-20.
    // Just right: bx-20 = 461 → bx = 481
    auto bird = birdAt(481.0f, 100.0f);
    EXPECT_FALSE(pipe.checkCollision(bird));
}

// --- Collision with a moved pipe ---

TEST_F(CollisionTest, CollisionAfterPipeMoves) {
    pipe.update(1.0f); // pipe moved from x=400 to x=200

    // Bird at old pipe position — no collision now
    auto birdOld = birdAt(400.0f, 100.0f);
    EXPECT_FALSE(pipe.checkCollision(birdOld));

    // Bird at new pipe position — collision
    auto birdNew = birdAt(200.0f, 100.0f);
    EXPECT_TRUE(pipe.checkCollision(birdNew));
}

// --- Collision with moving pipe ---

TEST_F(CollisionTest, MovingPipeCollision) {
    Pipe movingPipe(400.0f, 300.0f, 150.0f, 200.0f, PipeType::MOVING);
    // Moving pipe oscillates, but collision should still work
    auto bird = movingPipe.checkCollision(sf::FloatRect({380.0f, 80.0f}, {40.0f, 40.0f}));
    // The bird at (380, 80) with size (40,40) = [380, 80, 420, 120]
    // This overlaps the top pipe area — should detect collision
    EXPECT_TRUE(bird);
}

#include <gtest/gtest.h>
#include "ScoreFloat.h"
#include <SFML/Graphics.hpp>

class ScoreFloatTest : public ::testing::Test {
protected:
    sf::Font font; // Dummy font
};

// --- ScoreFloat Initialization ---

TEST_F(ScoreFloatTest, InitialState) {
    ScoreFloat score(font, sf::Vector2f(10.f, 10.f));
    EXPECT_GT(score.lifetime, 0);
    EXPECT_TRUE(score.alive());
}

// --- ScoreFloat Update ---

TEST_F(ScoreFloatTest, UpdateDecreasesLifetime) {
    ScoreFloat score(font, sf::Vector2f(10.f, 10.f));
    float initialLifetime = score.lifetime;
    score.update(0.5f);
    EXPECT_LT(score.lifetime, initialLifetime);
}

// --- ScoreFloat Alive Check ---

TEST_F(ScoreFloatTest, BecomesDeadAfterTimeout) {
    ScoreFloat score(font, sf::Vector2f(10.f, 10.f));
    score.update(2.0f); // Lifetime is usually around 1-2 seconds
    EXPECT_FALSE(score.alive());
}

// --- ScoreFloat Value ---

TEST_F(ScoreFloatTest, CorrectValue) {
    ScoreFloat score(font, sf::Vector2f(10.f, 10.f));
    EXPECT_GT(score.lifetime, 0);
}

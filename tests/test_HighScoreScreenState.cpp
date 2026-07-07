#include <gtest/gtest.h>
#include "states/HighScoreScreenState.h"
#include "states/GameState.h"
#include "core/HighScore.hpp"
#include <SFML/Graphics.hpp>

// --- HighScoreScreenState constructs without crash ---

TEST(HighScoreScreenStateTest, Constructs) {
    HighScoreScreenState state;
    EXPECT_NO_THROW({});
}

// --- HighScoreScreenState update does not crash ---

TEST(HighScoreScreenStateTest, UpdateDoesNotCrash) {
    HighScoreScreenState state;
    EXPECT_NO_THROW(state.update(0.01f));
    EXPECT_NO_THROW(state.update(0.1f));
    EXPECT_NO_THROW(state.update(1.0f));
}

// --- HighScoreScreenState nextAction is None initially ---

TEST(HighScoreScreenStateTest, NextActionNoneInitially) {
    HighScoreScreenState state;
    EXPECT_EQ(state.nextAction(), StateAction::None);
}

// --- HighScoreScreenState handles escape key ---

TEST(HighScoreScreenStateTest, HandleEscapeReturnsToMenu) {
    HighScoreScreenState state;
    state.handleKeyPress(sf::Keyboard::Key::Escape);
    EXPECT_EQ(state.nextAction(), StateAction::ReturnToMenu);
}

// --- HighScoreScreenState handles other keys ---

TEST(HighScoreScreenStateTest, HandleOtherKeyDoesNothing) {
    HighScoreScreenState state;
    state.handleKeyPress(sf::Keyboard::Key::Space);
    EXPECT_EQ(state.nextAction(), StateAction::None);
}

// --- HighScoreScreenState draw does not crash ---

TEST(HighScoreScreenStateTest, DrawDoesNotCrash) {
    HighScoreScreenState state;
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    sf::Font font;
    EXPECT_NO_THROW(state.draw(window, font));
}

// --- HighScoreScreenState load returns valid value ---

TEST(HighScoreScreenStateTest, LoadReturnsValidValue) {
    int val = HighScore::load();
    EXPECT_GE(val, 0);
}

// --- HighScoreScreenState handles multiple escape ---

TEST(HighScoreScreenStateTest, MultipleEscape) {
    HighScoreScreenState state;
    state.handleKeyPress(sf::Keyboard::Key::Escape);
    EXPECT_EQ(state.nextAction(), StateAction::ReturnToMenu);
    state.handleKeyPress(sf::Keyboard::Key::Escape);
    EXPECT_EQ(state.nextAction(), StateAction::ReturnToMenu);
}

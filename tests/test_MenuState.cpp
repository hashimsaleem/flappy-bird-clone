#include <gtest/gtest.h>
#include "states/MenuState.h"
#include "states/GameState.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "core/ConfigValues.hpp"

static ConfigValues g_cfg;

// --- MenuState constructs without crash ---

TEST(MenuStateTest, Constructs) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    EXPECT_NO_THROW({
        MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    });
}

// --- MenuState update does not crash ---

TEST(MenuStateTest, UpdateDoesNotCrash) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    EXPECT_NO_THROW(state.update(0.01f));
    EXPECT_NO_THROW(state.update(0.1f));
    EXPECT_NO_THROW(state.update(1.0f));
}

// --- MenuState starts with default difficulty 1 (Medium) ---

TEST(MenuStateTest, DefaultDifficulty) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    EXPECT_EQ(state.selectedDifficulty(), 1);
}

// --- MenuState nextAction is None initially ---

TEST(MenuStateTest, NextActionNoneInitially) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    EXPECT_EQ(state.nextAction(), StateAction::None);
}

// --- MenuState handles Num1 key (Play Game) ---

TEST(MenuStateTest, HandleNum1SetsPlayGame) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    state.handleKeyPress(sf::Keyboard::Key::Num1);
    state.update(0.01f);
    EXPECT_EQ(state.nextAction(), StateAction::PlayGame);
}

// --- MenuState handles Num2 key (High Score) ---

TEST(MenuStateTest, HandleNum2SetsShowHighScore) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    state.handleKeyPress(sf::Keyboard::Key::Num2);
    state.update(0.01f);
    EXPECT_EQ(state.nextAction(), StateAction::ShowHighScore);
}

// --- MenuState handles difficulty key 3 (Easy) ---

TEST(MenuStateTest, HandleDifficultyKey3) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    state.handleKeyPress(sf::Keyboard::Key::Num3);
    EXPECT_EQ(state.selectedDifficulty(), 0);
}

// --- MenuState handles difficulty key 4 (Medium) ---

TEST(MenuStateTest, HandleDifficultyKey4) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    state.handleKeyPress(sf::Keyboard::Key::Num4);
    EXPECT_EQ(state.selectedDifficulty(), 1);
}

// --- MenuState handles difficulty key 5 (Hard) ---

TEST(MenuStateTest, HandleDifficultyKey5) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    state.handleKeyPress(sf::Keyboard::Key::Num5);
    EXPECT_EQ(state.selectedDifficulty(), 2);
}

// --- MenuState volume increases with + key ---

TEST(MenuStateTest, VolumeIncreasesWithPlusKey) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(50);
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, true, highScore, font, "");

    state.handleKeyPress(sf::Keyboard::Key::Equal);
    EXPECT_FLOAT_EQ(state.getVolume(), 60.f);
}

// --- MenuState volume decreases with - key ---

TEST(MenuStateTest, VolumeDecreasesWithMinusKey) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(50);
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, true, highScore, font, "");

    state.handleKeyPress(sf::Keyboard::Key::Hyphen);
    EXPECT_FLOAT_EQ(state.getVolume(), 40.f);
}

// --- MenuState onEnter can be called multiple times ---

TEST(MenuStateTest, OnEnterCanBeCalledMultipleTimes) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    EXPECT_NO_THROW(state.onEnter());
    EXPECT_NO_THROW(state.onEnter());
}

// --- MenuState draw does not crash ---

TEST(MenuStateTest, DrawDoesNotCrash) {
    sf::Music bgmMusic;
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, false, highScore, font, "");
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    EXPECT_NO_THROW(state.draw(window, font));
}

// --- MenuState volume clamped at 100 ---

TEST(MenuStateTest, VolumeClampedAt100) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(100);
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, true, highScore, font, "");

    state.handleKeyPress(sf::Keyboard::Key::Equal);
    EXPECT_LE(state.getVolume(), 100.f);
}

// --- MenuState volume clamped at 0 ---

TEST(MenuStateTest, VolumeClampedAtZero) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(0);
    sf::Font font;
    int highScore = 0;
    MenuState state(g_cfg, bgmMusic, true, highScore, font, "");

    state.handleKeyPress(sf::Keyboard::Key::Hyphen);
    EXPECT_GE(state.getVolume(), 0.f);
}

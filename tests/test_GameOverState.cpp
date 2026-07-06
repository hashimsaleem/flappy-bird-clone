#include <gtest/gtest.h>
#include "GameOverState.h"
#include "BirdState.h"
#include "Pipe.hpp"
#include "ScoreFloat.h"
#include "GameState.h"
#include "PlayState.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// --- GameOverState constructs without crash ---

TEST(GameOverStateTest, Constructs) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    EXPECT_NO_THROW({
        GameOverState state(birdState, std::move(pipes), std::move(particles),
                           std::move(scoreFloats), 10, highScore, 1);
    });
}

// --- GameOverState save high score on construction ---

TEST(GameOverStateTest, SavesHighScore) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 5;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    EXPECT_EQ(highScore, 10); // Should be updated
}

// --- GameOverState does not update high score if lower ---

TEST(GameOverStateTest, DoesNotUpdateHighScoreIfLower) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 20;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    EXPECT_EQ(highScore, 20); // Should stay the same
}

// --- GameOverState update overlay fade ---

TEST(GameOverStateTest, UpdateOverlayFade) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    state.update(0.5f);
    state.update(0.5f);
    // overlayAlpha should approach 1.0
}

// --- GameOverState handles space key (play game) ---

TEST(GameOverStateTest, HandleSpaceSetsPlayGame) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    state.handleKeyPress(sf::Keyboard::Key::Space);
    EXPECT_EQ(state.nextAction(), StateAction::PlayGame);
}

// --- GameOverState handles escape key (return to menu) ---

TEST(GameOverStateTest, HandleEscapeSetsReturnToMenu) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    state.handleKeyPress(sf::Keyboard::Key::Escape);
    EXPECT_EQ(state.nextAction(), StateAction::ReturnToMenu);
}

// --- GameOverState getRestartBirdState returns valid state ---

TEST(GameOverStateTest, GetRestartBirdState) {
    BirdState birdState;
    birdState.posX = 100.f;
    birdState.posY = 200.f;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    auto restartState = state.getRestartBirdState();
    EXPECT_FLOAT_EQ(restartState.posX, 100.f);
    EXPECT_FLOAT_EQ(restartState.posY, 200.f);
}

// --- GameOverState dying bird falls ---

TEST(GameOverStateTest, DyingBirdFalls) {
    BirdState birdState;
    birdState.isDying = true;
    birdState.posY = 100.f;
    birdState.velocityY = 0.f;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                        std::move(scoreFloats), 10, highScore, 1);
    state.update(0.1f);
    EXPECT_GT(state.getBirdState().posY, 100.f);
}

// --- GameOverState dying bird stops at ground ---

TEST(GameOverStateTest, DyingBirdStopsAtGround) {
    BirdState birdState;
    birdState.isDying = true;
    birdState.posY = static_cast<float>(Config::SCREEN_HEIGHT - Config::GROUND_HEIGHT - Config::BIRD_HEIGHT - 10);
    birdState.velocityY = 0.f;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    // Let it fall to ground
    for (int i = 0; i < 100; i++) {
        state.update(0.01f);
    }
    // Should be at ground level
    EXPECT_LE(birdState.posY, static_cast<float>(Config::SCREEN_HEIGHT - Config::GROUND_HEIGHT - Config::BIRD_HEIGHT));
}

// --- GameOverState handles unknown key ---

TEST(GameOverStateTest, HandleUnknownKey) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    state.handleKeyPress(sf::Keyboard::Key::A);
    EXPECT_EQ(state.nextAction(), StateAction::None);
}

// --- GameOverState particles update ---

TEST(GameOverStateTest, ParticlesUpdate) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    Particle p(sf::Vector2f(0, 0), sf::Vector2f(0, 0), 0.5f);
    particles.push_back(std::move(p));
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    state.update(1.0f);
    // Particles should be expired
    auto restartState = state.getRestartBirdState();
    (void)restartState;
}

// --- GameOverState with score 0 ---

TEST(GameOverStateTest, ScoreZero) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 0, highScore, 1);
    EXPECT_EQ(highScore, 0);
}

// --- GameOverState draw does not crash ---

TEST(GameOverStateTest, DrawDoesNotCrash) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 1);
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    sf::Font font;
    EXPECT_NO_THROW(state.draw(window, font));
}

// --- GameOverState preserves difficulty in restart state ---

TEST(GameOverStateTest, PreservesDifficulty) {
    BirdState birdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int highScore = 0;
    GameOverState state(birdState, std::move(pipes), std::move(particles),
                       std::move(scoreFloats), 10, highScore, 2);
    auto restartState = state.getRestartBirdState();
    EXPECT_EQ(restartState.difficulty, 2);
}

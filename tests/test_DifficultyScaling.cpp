#include <gtest/gtest.h>
#include "states/PlayState.h"
#include "scoring/ScoreManager.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include "core/ConfigValues.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

// Mocking or providing dummy dependencies for PlayState
class DifficultyScalingTest : public ::testing::Test {
protected:
    ConfigValues cfg;
    sf::Font font;
    int highScore = 0;
    bool bgmLoaded = false;

    std::unique_ptr<PlayState> createPlayState() {
        // We need a dummy music object. Since we can't easily mock sf::Music, 
        // and it requires an open file, we might have issues if the test runner 
        // doesn't have access to assets. However, PlayState constructor 
        // only uses bgmLoaded flag for some logic.
        sf::Music dummyMusic;
        return std::make_unique<PlayState>(cfg, dummyMusic, bgmLoaded, highScore, font);
    }

    // PlayState::update() early-returns until the 3-second countdown elapses.
    // Use small dt ticks to pass countdown without bird crashing.
    void startGame(std::unique_ptr<PlayState>& state) {
        // Tick with small dt to pass 3-second countdown without large physics
        for (int i = 0; i < 300; ++i) {
            state->update(0.01f);
        }
    }
};

TEST_F(DifficultyScalingTest, ScalingLogicVerification) {
    auto state = createPlayState();
    startGame(state);
    
    // Initial state (score 0)
    float initialSpeed = state->getScoreManager()->getCurrentPipeSpeed();
    float initialInterval = state->getScoreManager()->getCurrentSpawnInterval();

    // Simulate reaching a high score by manually manipulating the ScoreManager
    for(int i = 0; i < 100; ++i) {
         state->getScoreManager()->addScore();
    }
    
    // Tick with small dt to trigger difficulty scaling without bird crash
    for(int i = 0; i < 10; ++i) {
         state->update(0.01f);
    }
    float finalSpeed = state->getScoreManager()->getCurrentPipeSpeed();
    float finalInterval = state->getScoreManager()->getCurrentSpawnInterval();

    // Verify that speed increased and interval decreased
    EXPECT_GT(finalSpeed, initialSpeed);
    EXPECT_LT(finalInterval, initialInterval);
}

TEST_F(DifficultyScalingTest, ZoneTransitions) {
    auto state = createPlayState();
    startGame(state);
    
    // Test Easy -> Normal transition (Threshold 50)
    for(int i = 0; i < 50; ++i) state->getScoreManager()->addScore();
    for(int i = 0; i < 10; ++i) state->update(0.01f);
    float speedAfterEasy = state->getScoreManager()->getCurrentPipeSpeed();
    
    // One more score to enter Normal (Threshold 51)
    state->getScoreManager()->addScore();
    for(int i = 0; i < 10; ++i) state->update(0.01f);
    float speedAfterNormal = state->getScoreManager()->getCurrentPipeSpeed();
    
    EXPECT_GT(speedAfterNormal, speedAfterEasy);

    // Test Normal -> Hard transition (Threshold 150)
    for(int i = 0; i < 100; ++i) state->getScoreManager()->addScore();
    for(int i = 0; i < 10; ++i) state->update(0.01f);
    float speedAfterHard = state->getScoreManager()->getCurrentPipeSpeed();
    
    EXPECT_GT(speedAfterHard, speedAfterNormal);

    // Test Hard -> Insane transition (Threshold 300)
    for(int i = 0; i < 150; ++i) state->getScoreManager()->addScore();
    for(int i = 0; i < 10; ++i) state->update(0.01f);
    float speedAfterInsane = state->getScoreManager()->getCurrentPipeSpeed();
    
    EXPECT_GT(speedAfterInsane, speedAfterHard);
}

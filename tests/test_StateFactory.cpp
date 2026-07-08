#include <gtest/gtest.h>
#include <string>
#include "states/StateFactory.h"
#include "states/GameState.h"
#include "states/PlayState.h"
#include "core/ConfigValues.hpp"
#include <SFML/Audio.hpp>

class StateFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No assets needed for basic null checks
    }
};

TEST_F(StateFactoryTest, CreateMenuStateReturnsNonNull) {
    ConfigValues cfg;
    sf::Music bgmMusic;
    bool bgmLoaded = false;
    int highScore = 0;
    sf::Font font;

    auto state = StateFactory::createMenuState(cfg, bgmMusic, bgmLoaded, highScore, font);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreatePlayStateReturnsNonNull) {
    ConfigValues cfg;
    sf::Music bgmMusic;
    bool bgmLoaded = false;
    int highScore = 0;
    sf::Font font;

    auto state = StateFactory::createPlayState(cfg, bgmMusic, bgmLoaded, highScore, font);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreateGameOverStateReturnsNonNull) {
    ConfigValues cfg;
    PlayStateSnapshot snap;
    int highScore = 0;
    auto state = StateFactory::createGameOverState(cfg, snap, 100, highScore);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreateHighScoreScreenStateReturnsNonNull) {
    auto state = StateFactory::createHighScoreScreenState();
    EXPECT_NE(state, nullptr);
}

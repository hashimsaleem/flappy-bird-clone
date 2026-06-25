#include <gtest/gtest.h>
#include "StateFactory.h"
#include "GameState.h"
#include "PlayState.h"
#include <SFML/Audio.hpp>

class StateFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No assets needed for basic null checks
    }
};

TEST_F(StateFactoryTest, CreateMenuStateReturnsNonNull) {
    sf::SoundBuffer dummyBuffer;
    sf::Sound jumpSnd(dummyBuffer);
    sf::Sound scoreSnd(dummyBuffer);
    sf::Sound deathSnd(dummyBuffer);
    sf::Music bgmMusic;
    bool bgmLoaded = false;
    int highScore = 0;
    sf::Font font;

    auto state = StateFactory::createMenuState(jumpSnd, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreatePlayStateReturnsNonNull) {
    sf::SoundBuffer dummyBuffer;
    sf::Sound jumpSnd(dummyBuffer);
    sf::Sound scoreSnd(dummyBuffer);
    sf::Sound deathSnd(dummyBuffer);
    sf::Music bgmMusic;
    bool bgmLoaded = false;
    int highScore = 0;
    sf::Font font;

    auto state = StateFactory::createPlayState(jumpSnd, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreateGameOverStateReturnsNonNull) {
    PlayStateSnapshot snap;
    int highScore = 0;
    auto state = StateFactory::createGameOverState(snap, 100, highScore);
    EXPECT_NE(state, nullptr);
}

TEST_F(StateFactoryTest, CreateHighScoreScreenStateReturnsNonNull) {
    auto state = StateFactory::createHighScoreScreenState();
    EXPECT_NE(state, nullptr);
}

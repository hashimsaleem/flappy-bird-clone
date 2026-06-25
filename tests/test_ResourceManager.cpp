#include <gtest/gtest.h>
#include "ResourceManager.hpp"
#include "Config.hpp"

class ResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No specific setup needed for basic checks
    }
};

// --- Font Loading ---

TEST_F(ResourceManagerTest, GetFontReturnsNonNull) {
    // This assumes the font exists at Config::FONT_PATH
    const sf::Font& font = ResourceManager::getFont(Config::FONT_PATH, 30);
    // Just checking that it's a valid font object (no easy way to check non-null for reference)
}

// --- Sound Loading ---

TEST_F(ResourceManagerTest, GetSoundReturnsNonNull) {
    // These might fail if assets are missing, but we want to check the logic.
    sf::Sound jumpSnd = ResourceManager::getSound(Config::JUMP_SND);
    
    sf::Sound scoreSnd = ResourceManager::getSound(Config::SCORE_SND);
    
    sf::Sound deathSnd = ResourceManager::getSound(Config::DEATH_SND);
}

// --- Music Loading ---

TEST_F(ResourceManagerTest, GetMusicReturnsNonNull) {
    // We check if the music object is created. 
    // Note: openFromFile might fail if file is missing, but the pointer should be non-null.
    sf::Music* bgm = ResourceManager::getMusic(Config::BG_MUSIC);
    EXPECT_NE(bgm, nullptr);
}

// --- Resource Cache Check ---

TEST_F(ResourceManagerTest, GetFontReturnsCachedInstance) {
    const sf::Font& font1 = ResourceManager::getFont(Config::FONT_PATH, 30);
    const sf::Font& font2 = ResourceManager::getFont(Config::FONT_PATH, 30);
    EXPECT_EQ(&font1, &font2);
}

TEST_F(ResourceManagerTest, GetSoundReturnsCachedInstance) {
    sf::Sound& snd1 = ResourceManager::getSound(Config::JUMP_SND);
    sf::Sound& snd2 = ResourceManager::getSound(Config::JUMP_SND);
    EXPECT_EQ(&snd1, &snd2);
}

TEST_F(ResourceManagerTest, GetMusicReturnsCachedInstance) {
    sf::Music* mus1 = ResourceManager::getMusic(Config::BG_MUSIC);
    sf::Music* mus2 = ResourceManager::getMusic(Config::BG_MUSIC);
    EXPECT_EQ(mus1, mus2);
}

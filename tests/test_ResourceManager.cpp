#include <gtest/gtest.h>
#include "ResourceManager.hpp"
#include "Config.hpp"

class ResourceManagerTest : public ::testing::Test {
protected:
    ResourceManager resMgr;
};

TEST_F(ResourceManagerTest, GetFontReturnsNonNull) {
    const sf::Font& font = resMgr.getFont(Config::FONT_PATH, 30);
}

TEST_F(ResourceManagerTest, GetSoundReturnsNonNull) {
    sf::Sound& jumpSnd = resMgr.getSound(Config::JUMP_SND);
    sf::Sound& scoreSnd = resMgr.getSound(Config::SCORE_SND);
    sf::Sound& deathSnd = resMgr.getSound(Config::DEATH_SND);
}

TEST_F(ResourceManagerTest, GetMusicReturnsNonNull) {
    sf::Music* bgm = resMgr.getMusic(Config::BG_MUSIC);
    EXPECT_NE(bgm, nullptr);
}

TEST_F(ResourceManagerTest, GetFontReturnsCachedInstance) {
    const sf::Font& font1 = resMgr.getFont(Config::FONT_PATH, 30);
    const sf::Font& font2 = resMgr.getFont(Config::FONT_PATH, 30);
    EXPECT_EQ(&font1, &font2);
}

TEST_F(ResourceManagerTest, GetSoundReturnsCachedInstance) {
    sf::Sound& snd1 = resMgr.getSound(Config::JUMP_SND);
    sf::Sound& snd2 = resMgr.getSound(Config::JUMP_SND);
    EXPECT_EQ(&snd1, &snd2);
}

TEST_F(ResourceManagerTest, GetMusicReturnsCachedInstance) {
    sf::Music* mus1 = resMgr.getMusic(Config::BG_MUSIC);
    sf::Music* mus2 = resMgr.getMusic(Config::BG_MUSIC);
    EXPECT_EQ(mus1, mus2);
}

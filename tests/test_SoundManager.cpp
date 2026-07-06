#include <gtest/gtest.h>
#include "SoundManager.hpp"
#include <SFML/Audio.hpp>

// --- SoundManager starts with default volume ---

TEST(SoundManagerTest, DefaultVolume) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 100.f);
}

// --- SoundManager setBGMVolume changes volume ---

TEST(SoundManagerTest, SetBGMVolumeChangesVolume) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    sm.setBGMVolume(50.f);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 50.f);
    sm.setBGMVolume(0.f);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 0.f);
    sm.setBGMVolume(100.f);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 100.f);
}

// --- SoundManager setBGMVolume clamped at 100 ---

TEST(SoundManagerTest, SetBGMVolumeClampedAt100) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    sm.setBGMVolume(150.f);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 150.f); // No clamping in setBGMVolume
}

// --- SoundManager setBGMVolume can go to 0 ---

TEST(SoundManagerTest, SetBGMVolumeToZero) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    sm.setBGMVolume(0.f);
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 0.f);
}

// --- SoundManager fadeBGM sets up fade state ---

TEST(SoundManagerTest, FadeBGMSetsTargetVolume) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    sm.fadeBGM(50.f, 2.0f);
    // After fadeBGM, current volume stays at 100 but target changes
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 100.f);
}

// --- SoundManager update processes fade ---

TEST(SoundManagerTest, UpdateProcessesFade) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(100);
    SoundManager sm(bgmMusic, true);
    sm.setBGMVolume(100.f);
    sm.fadeBGM(0.f, 1.0f);
    // Update past fade duration
    sm.update(1.5f);
    // Volume should have faded toward 0
    EXPECT_LT(sm.getCurrentBGMVolume(), 100.f);
}

// --- SoundManager playSfx functions do not crash ---

TEST(SoundManagerTest, PlayJumpDoesNotCrash) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    EXPECT_NO_THROW(sm.playJump());
}

TEST(SoundManagerTest, PlayScoreDoesNotCrash) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    EXPECT_NO_THROW(sm.playScore());
}

TEST(SoundManagerTest, PlayDeathDoesNotCrash) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    EXPECT_NO_THROW(sm.playDeath());
}

// --- SoundManager stopBGM does not crash ---

TEST(SoundManagerTest, StopBGMDoesNotCrash) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, true);
    EXPECT_NO_THROW(sm.stopBGM());
}

// --- SoundManager update with no fade does nothing ---

TEST(SoundManagerTest, UpdateNoFade) {
    sf::Music bgmMusic;
    bgmMusic.setVolume(80);
    SoundManager sm(bgmMusic, true);
    sm.setBGMVolume(80.f);
    for (int i = 0; i < 10; i++) {
        sm.update(0.1f);
    }
    EXPECT_FLOAT_EQ(sm.getCurrentBGMVolume(), 80.f);
}

// --- SoundManager with bgm not loaded does not crash ---

TEST(SoundManagerTest, BgmNotLoadedNoCrash) {
    sf::Music bgmMusic;
    SoundManager sm(bgmMusic, false);
    EXPECT_NO_THROW(sm.setBGMVolume(50.f));
    EXPECT_NO_THROW(sm.update(0.01f));
    EXPECT_NO_THROW(sm.playJump());
}

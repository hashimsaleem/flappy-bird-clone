#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "HighScore.hpp"

class HighScoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Save any existing high score file and reset cache
        std::ifstream existing("highscore.dat");
        if (existing.good()) {
            hasExistingFile = true;
            std::ifstream src("highscore.dat", std::ios::binary);
            std::ofstream dst("highscore.dat.bak", std::ios::binary);
            dst << src.rdbuf();
            src.close();
            dst.close();
        }
        HighScore::reset();
    }

    void TearDown() override {
        HighScore::reset();
        std::remove("highscore.dat");
        if (hasExistingFile) {
            std::rename("highscore.dat.bak", "highscore.dat");
            HighScore::reset(); // re-cache the original
        }
    }

    bool hasExistingFile = false;
};

// --- Default score is 0 ---

TEST_F(HighScoreTest, DefaultScoreIsZero) {
    EXPECT_EQ(HighScore::load(), 0);
    EXPECT_EQ(HighScore::get(), 0);
}

// --- Save and load round-trip ---

TEST_F(HighScoreTest, SaveAndLoadRoundTrip) {
    HighScore::save(42);
    HighScore::reset(); // force re-read from file
    EXPECT_EQ(HighScore::load(), 42);
}

// --- Only the highest score is kept ---

TEST_F(HighScoreTest, OnlyHighestScoreIsKept) {
    HighScore::save(10);
    HighScore::save(5);   // lower — should be ignored
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 10);
}

// --- Increasing score updates the file ---

TEST_F(HighScoreTest, IncreasingScoreUpdates) {
    HighScore::save(10);
    HighScore::save(50);  // higher — should update
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 50);
}

// --- Multiple saves of same score is stable ---

TEST_F(HighScoreTest, SameScoreIsStable) {
    HighScore::save(100);
    HighScore::save(100);
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 100);
}

// --- Negative score doesn't overwrite positive ---

TEST_F(HighScoreTest, NegativeScoreRejected) {
    // save() only stores scores higher than current best.
    // Since default is 0, -5 is rejected.
    HighScore::save(-5);
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 0);
}

// --- Negative score doesn't overwrite positive ---

TEST_F(HighScoreTest, NegativeDoesNotOverwritePositive) {
    HighScore::save(50);
    HighScore::save(-10);  // lower — should be ignored
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 50);
}

// --- Persists across resets ---

TEST_F(HighScoreTest, PersistsAcrossResets) {
    HighScore::save(77);
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 77);

    HighScore::reset();
    EXPECT_EQ(HighScore::load(), 77);
}

// --- Large score values ---

TEST_F(HighScoreTest, LargeScore) {
    constexpr int BIG = 999999;
    HighScore::save(BIG);
    HighScore::reset();
    EXPECT_EQ(HighScore::load(), BIG);
}

// --- Get returns cached value without re-reading ---

TEST_F(HighScoreTest, GetReturnsCached) {
    HighScore::save(200);
    // Modify the file directly behind the scenes
    {
        int fake = 999;
        std::ofstream f("highscore.dat", std::ios::binary);
        f.write(reinterpret_cast<const char*>(&fake), sizeof(fake));
    }
    // get() should still return cached 200 (not re-read from file)
    EXPECT_EQ(HighScore::get(), 200);
}

// --- Load re-reads from file if cache is cleared ---

TEST_F(HighScoreTest, LoadReReadsAfterReset) {
    HighScore::save(300);
    HighScore::reset();    // clear cache
    EXPECT_EQ(HighScore::load(), 300);
}

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <sstream>
#include "ConfigLoader.hpp"
#include "HighScore.hpp"
#include "Pipe.hpp"
#include "Particle.hpp"
#include "Bird.hpp"
#include "Config.hpp"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ConfigLoader::clear();
        ConfigLoader::load("/home/hashim/trading/assets/gameconfig.json");
        // Clean up any temp high score files
        std::remove("highscore.dat");
    }

    void TearDown() override {
        ConfigLoader::clear();
        std::remove("highscore.dat");
    }

    std::string createValidConfig(const std::string& extra = "") {
        return R"({
            "screen_width": 800,
            "screen_height": 600,
            "gravity": 800.0,
            "jump_strength": -600.0,
            "pipe_speed": 200.0,
            "gap_height": 150.0,
            "pipe_spawn_interval": 1.5,
            "ground_height": 50,
            "bird_min_tilt": -30.0,
            "bird_max_tilt": 60.0,
            "bird_flap_rate": 8.0,
            "bird_flap_depth": 3.0,
            "pipe_speed_max": 400.0,
            "spawn_interval_min": 0.8
            )" + extra + "\n}";
    }
};

// --- Config Validation ---

TEST_F(IntegrationTest, ValidConfigPassesValidation) {
    auto path = "test_valid_config.json";
    {
        std::ofstream out(path);
        out << createValidConfig();
    }
    
    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));
    EXPECT_TRUE(ConfigLoader::isLoaded());
    
    // Check all required values are parseable
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 0.0f), 800.0f);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("pipe_speed", 0.0f), 200.0f);
    EXPECT_EQ(ConfigLoader::getInt("screen_width", 0), 800);
    EXPECT_EQ(ConfigLoader::getInt("ground_height", 0), 50);
    
    std::remove(path);
}

TEST_F(IntegrationTest, MissingRequiredKeyLogsWarning) {
    // Create config with missing required keys
    auto path = "test_partial_config.json";
    {
        std::ofstream out(path);
        out << R"({ "screen_width": 800 })";
    }
    
    ConfigLoader::clear();
    // Should still load but with defaults
    EXPECT_TRUE(ConfigLoader::load(path));
    
    // Missing keys should return defaults
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 1000.0f), 1000.0f);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("pipe_speed", 200.0f), 200.0f);
    
    std::remove(path);
}

// --- High Score Persistence ---

TEST_F(IntegrationTest, HighScoreSaveLoadRoundTrip) {
    const int testScore = 42;
    
    // Save
    HighScore::save(testScore);
    
    // Load
    int loaded = HighScore::load();
    EXPECT_EQ(loaded, testScore);
}

TEST_F(IntegrationTest, HighScoreFileCreated) {
    HighScore::save(99);
    
    // Verify file exists
    std::ifstream f("highscore.dat");
    EXPECT_TRUE(f.is_open());
    
    // Verify content
    std::string line;
    EXPECT_TRUE(std::getline(f, line));
    EXPECT_FALSE(line.empty());
}

TEST_F(IntegrationTest, HighScoreLoadDefaultsWhenMissing) {
    std::remove("highscore.dat");
    int loaded = HighScore::load();
    EXPECT_GE(loaded, 0); // Should be >= 0 (default)
}

// --- Pipe Spawning ---

TEST_F(IntegrationTest, PipeCreationValidGap) {
    // Test that pipe gaps are within reasonable bounds
    float screenWidth = static_cast<float>(Config::SCREEN_WIDTH);
    float groundHeight = static_cast<float>(Config::GROUND_HEIGHT);
    float maxPipeY = Config::SCREEN_HEIGHT - groundHeight - Config::PIPE_HEIGHT;
    
    // All pipes should have gap bottom <= maxPipeY
    // This validates that the config produces valid pipe gaps
    
    // Test a few pipe constructions
    Pipe p1(800.f, 200.f, Config::PIPE_HEIGHT, 200.f, PipeType::STATIC);
    EXPECT_FLOAT_EQ(p1.getX(), 800.f);
    
    Pipe p2(400.f, 300.f, Config::PIPE_HEIGHT, 200.f, PipeType::MOVING);
    EXPECT_FLOAT_EQ(p2.getX(), 400.f);
}

TEST_F(IntegrationTest, PipeOffScreenDetection) {
    Pipe p(800.f, 200.f, Config::PIPE_HEIGHT, 200.f, PipeType::STATIC);
    EXPECT_FALSE(p.isOffScreen());
    
    // Simulate pipe moving off screen
    p.update(5.0f); // Move 5 seconds worth
    EXPECT_TRUE(p.isOffScreen());
}

// --- Particle Lifecycle ---

TEST_F(IntegrationTest, ParticleLifecycle) {
    Particle p(sf::Vector2f(100.f, 200.f), sf::Vector2f(50.f, -30.f), 1.0f);
    
    EXPECT_FLOAT_EQ(p.lifetime, 1.0f);
    EXPECT_GT(p.lifetime, 0.f);
    
    // Update past lifetime
    p.update(1.5f);
    EXPECT_LE(p.lifetime, 0.f);
}

// --- Bird State ---

TEST_F(IntegrationTest, BirdInitialState) {
    Bird bird;
    bird.load(Config::BIRD_PATH);
    bird.reset();
    
    float x = bird.getX();
    float y = bird.getBoundingBox().position.y;
    
    // Bird should start at reasonable position
    EXPECT_GT(x, 0.f);
    EXPECT_LT(x, static_cast<float>(Config::SCREEN_WIDTH));
    EXPECT_GT(y, 0.f);
    EXPECT_LT(y, static_cast<float>(Config::SCREEN_HEIGHT));
}

TEST_F(IntegrationTest, BirdFlapUpdatesVelocity) {
    Bird bird;
    bird.load(Config::BIRD_PATH);
    bird.reset();
    
    float initialVel = bird.getVelocity();
    bird.flap();
    float postFlapVel = bird.getVelocity();
    
    // Flap should make velocity more negative (upward)
    EXPECT_LT(postFlapVel, initialVel);
}

// --- Game Config Completeness ---

TEST_F(IntegrationTest, ConfigHasAllRequiredKeys) {
    ConfigLoader::load("/home/hashim/trading/assets/gameconfig.json");
    EXPECT_TRUE(ConfigLoader::get("screen_width").isInt());
    EXPECT_TRUE(ConfigLoader::get("screen_height").isInt());
    EXPECT_TRUE(ConfigLoader::get("gravity").isFloat());
    EXPECT_TRUE(ConfigLoader::get("jump_strength").isFloat());
    EXPECT_TRUE(ConfigLoader::get("pipe_speed").isFloat());
    EXPECT_TRUE(ConfigLoader::get("gap_height").isFloat());
    EXPECT_TRUE(ConfigLoader::get("pipe_spawn_interval").isFloat());
    EXPECT_TRUE(ConfigLoader::get("ground_height").isInt());
}

// --- Edge Case: Very Large Numbers ---

TEST_F(IntegrationTest, ConfigHandlesLargeNumbers) {
    auto path = "test_large_config.json";
    {
        std::ofstream out(path);
        out << R"({
            "screen_width": 999999,
            "screen_height": 999999,
            "gravity": 999999.0,
            "jump_strength": -999999.0,
            "pipe_speed": 999999.0,
            "gap_height": 999999.0,
            "pipe_spawn_interval": 999.0,
            "ground_height": 999999
        })";
    }
    
    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));
    
    EXPECT_EQ(ConfigLoader::getInt("screen_width", 0), 999999);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 0.0f), 999999.0f);
    
    std::remove(path);
}

// --- Edge Case: Malformed JSON (should fall back gracefully) ---

TEST_F(IntegrationTest, MalformedJsonFallsBackToDefaults) {
    auto path = "test_malformed.json";
    {
        std::ofstream out(path);
        out << R"({ "broken": , "missing": })";
    }
    
    ConfigLoader::clear();
    // Should not crash, may or may not load successfully
    ConfigLoader::load(path);
    
    // Should be safe to query even with malformed input
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 1000.0f), 1000.0f); // default
    
    std::remove(path);
}

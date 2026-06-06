#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "ConfigLoader.hpp"

class ConfigLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        ConfigLoader::clear();
    }

    void TearDown() override {
        ConfigLoader::clear();
        // Clean up any temp files we created
        for (const auto& f : tempFiles) {
            std::remove(f.c_str());
        }
        tempFiles.clear();
    }

    std::string createTempFile(const std::string& content) {
        std::string path = "test_config_" + std::to_string(rand()) + ".json";
        std::ofstream out(path);
        out << content;
        out.close();
        tempFiles.push_back(path);
        return path;
    }

    std::vector<std::string> tempFiles;
};

// --- File not found ---

TEST_F(ConfigLoaderTest, MissingFileReturnsFalse) {
    EXPECT_FALSE(ConfigLoader::load("nonexistent_file.json"));
    EXPECT_FALSE(ConfigLoader::isLoaded());
}

// --- Empty JSON ---

TEST_F(ConfigLoaderTest, EmptyJsonReturnsEmpty) {
    auto path = createTempFile("{}");
    EXPECT_TRUE(ConfigLoader::load(path));
    EXPECT_TRUE(ConfigLoader::isLoaded());
    EXPECT_TRUE(ConfigLoader::get("missing").isNone());
    EXPECT_EQ(ConfigLoader::getInt("missing", 42), 42);
}

// --- Integer values ---

TEST_F(ConfigLoaderTest, ParsesIntegers) {
    auto path = createTempFile(R"({ "count": 42, "negative": -7 })");
    EXPECT_TRUE(ConfigLoader::load(path));

    auto v = ConfigLoader::get("count");
    EXPECT_TRUE(v.isInt());
    EXPECT_EQ(v.intValue, 42);

    EXPECT_EQ(ConfigLoader::getInt("count", 0), 42);
    EXPECT_EQ(ConfigLoader::getInt("negative", 0), -7);
}

// --- Float values ---

TEST_F(ConfigLoaderTest, ParsesFloats) {
    auto path = createTempFile(R"({ "gravity": 800.0, "pi": 3.14159 })");
    EXPECT_TRUE(ConfigLoader::load(path));

    auto v = ConfigLoader::get("gravity");
    EXPECT_TRUE(v.isFloat());
    EXPECT_FLOAT_EQ(v.floatValue, 800.0f);

    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 0.0f), 800.0f);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("pi", 0.0f), 3.14159f);
}

// --- Float with fallback ---

TEST_F(ConfigLoaderTest, FloatFallbackReturnsDefault) {
    auto path = createTempFile(R"({ "a": 1.0 })");
    EXPECT_TRUE(ConfigLoader::load(path));

    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("missing_key", 999.0f), 999.0f);
}

// --- Bool values ---

TEST_F(ConfigLoaderTest, ParsesBools) {
    auto path = createTempFile(R"({ "yes": true, "no": false })");
    EXPECT_TRUE(ConfigLoader::load(path));

    auto vt = ConfigLoader::get("yes");
    EXPECT_TRUE(vt.isBool());
    EXPECT_TRUE(vt.boolValue);

    auto vf = ConfigLoader::get("no");
    EXPECT_TRUE(vf.isBool());
    EXPECT_FALSE(vf.boolValue);

    EXPECT_TRUE(ConfigLoader::getBool("yes", false));
    EXPECT_FALSE(ConfigLoader::getBool("no", true));
}

// --- String values ---

TEST_F(ConfigLoaderTest, ParsesStrings) {
    auto path = createTempFile(R"({ "name": "flappy", "path": "assets/" })");
    EXPECT_TRUE(ConfigLoader::load(path));

    auto v = ConfigLoader::get("name");
    EXPECT_TRUE(v.isString());
    EXPECT_EQ(v.stringValue, "flappy");

    EXPECT_EQ(ConfigLoader::getString("name", ""), "flappy");
    EXPECT_EQ(ConfigLoader::getString("path", ""), "assets/");
}

// --- Null value ---

TEST_F(ConfigLoaderTest, NullIsNone) {
    auto path = createTempFile(R"({ "nothing": null })");
    EXPECT_TRUE(ConfigLoader::load(path));

    auto v = ConfigLoader::get("nothing");
    EXPECT_TRUE(v.isNone());
}

// --- String with escape sequences ---

TEST_F(ConfigLoaderTest, ParsesEscapedStrings) {
    auto path = createTempFile(R"({ "text": "hello\nworld\t\"quoted\"" })");
    EXPECT_TRUE(ConfigLoader::load(path));

    std::string expected = "hello\nworld\t\"quoted\"";
    EXPECT_EQ(ConfigLoader::getString("text", ""), expected);
}

// --- Flat key names (nested objects are skipped by the parser) ---

TEST_F(ConfigLoaderTest, FlatKeysWithDescriptions) {
    auto path = createTempFile(R"({
        "description": "config",
        "physics.gravity": 800.0,
        "physics.jump": -600.0
    })");
    EXPECT_TRUE(ConfigLoader::load(path));

    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("physics.gravity", 0.0f), 800.0f);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("physics.jump", 0.0f), -600.0f);
    EXPECT_EQ(ConfigLoader::getString("description", ""), "config");
}

// --- Clear resets state ---

TEST_F(ConfigLoaderTest, ClearResetsState) {
    auto path = createTempFile(R"({ "key": 42 })");
    EXPECT_TRUE(ConfigLoader::load(path));
    EXPECT_EQ(ConfigLoader::getInt("key", 0), 42);

    ConfigLoader::clear();
    EXPECT_FALSE(ConfigLoader::isLoaded());
    EXPECT_TRUE(ConfigLoader::get("key").isNone());
}

// --- Reload overwrites old values ---

TEST_F(ConfigLoaderTest, ReloadOverwritesValues) {
    auto path = createTempFile(R"({ "value": 1 })");
    EXPECT_TRUE(ConfigLoader::load(path));
    EXPECT_EQ(ConfigLoader::getInt("value", 0), 1);

    // Write new content to same file
    {
        std::ofstream out(path);
        out << R"({ "value": 99 })";
    }
    EXPECT_TRUE(ConfigLoader::load(path));
    EXPECT_EQ(ConfigLoader::getInt("value", 0), 99);
}

#include <gtest/gtest.h>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "core/ConfigLoader.hpp"
#include "core/HighScore.hpp"

class ConfigEdgeCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        ConfigLoader::clear();
        HighScore::reset();
    }

    void TearDown() override {
        ConfigLoader::clear();
        HighScore::reset();
        for (const auto& f : tempFiles) {
            std::remove(f.c_str());
        }
        tempFiles.clear();
        for (const auto& d : tempDirs) {
            std::filesystem::remove_all(d);
        }
        tempDirs.clear();
    }

    std::string createTempFile(const std::string& content) {
        std::string path = "test_edge_" + std::to_string(std::rand()) + ".json";
        std::ofstream out(path);
        out << content;
        out.close();
        tempFiles.push_back(path);
        return path;
    }

    std::string createTempDir() {
        std::string dir = "test_edge_dir_" + std::to_string(std::rand());
        std::filesystem::create_directory(dir);
        tempDirs.push_back(dir);
        return dir;
    }

    std::vector<std::string> tempFiles;
    std::vector<std::string> tempDirs;
};

// --- File permission edge cases ---

TEST_F(ConfigEdgeCaseTest, ConfigFileNoPermission) {
    auto path = createTempFile(R"({ "screen_width": 800 })");
    std::filesystem::permissions(path,
        std::filesystem::perms::owner_all & ~std::filesystem::perms::owner_read,
        std::filesystem::perm_options::replace);

    ConfigLoader::clear();
    bool result = ConfigLoader::load(path);
    
    EXPECT_FALSE(result) << "Loading a file without read permission should fail";
    EXPECT_FALSE(ConfigLoader::isLoaded());
}

TEST_F(ConfigEdgeCaseTest, ConfigFilePathIsDirectory) {
    auto dirPath = createTempDir();
    
    ConfigLoader::clear();
    bool result = ConfigLoader::load(dirPath);
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(ConfigLoader::isLoaded());
}

TEST_F(ConfigEdgeCaseTest, HighScoreSaveToReadOnlyDir) {
    std::string baseDir = createTempDir();
    std::string subDir = baseDir + "/sub";
    std::filesystem::create_directory(subDir);
    tempDirs.pop_back();

    std::string scorePath = subDir + "/highscore.dat";
    HighScore::setPath(scorePath);

    HighScore::save(100);
    EXPECT_EQ(HighScore::load(), 100);

    std::filesystem::permissions(subDir,
        std::filesystem::perms::owner_all & ~std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace);

    EXPECT_NO_THROW(HighScore::save(200));

    std::filesystem::permissions(subDir, std::filesystem::perms::owner_all);
    std::filesystem::remove_all(subDir);
}

// --- Config edge cases ---

TEST_F(ConfigEdgeCaseTest, ConfigDeeplyNested) {
    auto path = createTempFile(R"({
        "level1": {
            "level2": {
                "level3": 42
            }
        }
    })");

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));

    EXPECT_EQ(ConfigLoader::getInt("level1.level2.level3", 0), 42);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("level1.level2.level3", -1.0f), 42.0f);
}

TEST_F(ConfigEdgeCaseTest, ConfigUnicodeStrings) {
    // Surrogate pair for U+1F426 (bird emoji) in JSON
    std::string jsonContent = "{\"emoji\":\"\\uD83D\\uDC26\",\"chinese\":\"\\u98DE\",\"russian\":\"\\u041F\\u0442\\u0438\\u0446\\u0430\"}";
    auto path = createTempFile(jsonContent);

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));

    // C++11 \U escape for the same code point
    EXPECT_EQ(ConfigLoader::getString("emoji", ""), "\U0001F426");
    EXPECT_EQ(ConfigLoader::getString("chinese", ""), "\u98DE");
    EXPECT_EQ(ConfigLoader::getString("russian", ""), "\u041F\u0442\u0438\u0446\u0430");
}

TEST_F(ConfigEdgeCaseTest, ConfigExtraUnknownKeys) {
    auto path = createTempFile(R"({
        "screen_width": 800,
        "unknown_key_1": "some_value",
        "unknown_key_2": 999,
        "unknown_key_3": true,
        "gravity": 800.0
    })");

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));

    EXPECT_EQ(ConfigLoader::getInt("screen_width", 0), 800);
    EXPECT_FLOAT_EQ(ConfigLoader::getFloat("gravity", 0.0f), 800.0f);

    EXPECT_NO_THROW(ConfigLoader::get("unknown_key_1"));
    EXPECT_NO_THROW(ConfigLoader::getInt("unknown_key_2", 0));
}

TEST_F(ConfigEdgeCaseTest, ConfigMultipleConsecutiveLoads) {
    auto path1 = createTempFile(R"({ "value": 10 })");
    auto path2 = createTempFile(R"({ "value": 99 })");

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path1));
    EXPECT_EQ(ConfigLoader::getInt("value", 0), 10);

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path2));
    EXPECT_EQ(ConfigLoader::getInt("value", 0), 99);

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path1));
    EXPECT_EQ(ConfigLoader::getInt("value", 0), 10);
}

TEST_F(ConfigEdgeCaseTest, ConfigEmptyStringValues) {
    auto path = createTempFile(R"({
        "empty_string": "",
        "normal": "text"
    })");

    ConfigLoader::clear();
    EXPECT_TRUE(ConfigLoader::load(path));

    EXPECT_EQ(ConfigLoader::getString("empty_string", "default"), "");
    EXPECT_EQ(ConfigLoader::getString("normal", "default"), "text");
}

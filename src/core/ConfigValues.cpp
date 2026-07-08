#include "ConfigValues.hpp"
#include "ConfigLoader.hpp"
#include <fstream>
#include <iostream>

static float getFloat(const std::string& key, float def) {
    return ConfigLoader::getFloat(key, def);
}

static int getInt(const std::string& key, int def) {
    return ConfigLoader::getInt(key, def);
}

static std::string getString(const std::string& key, const std::string& def) {
    return ConfigLoader::getString(key, def);
}

ConfigValues loadConfig(const std::string& path) {
    ConfigLoader::load(path);

    ConfigValues cfg;
    cfg.screenWidth = getInt("screen_width", cfg.screenWidth);
    cfg.screenHeight = getInt("screen_height", cfg.screenHeight);
    cfg.gravity = getFloat("gravity", cfg.gravity);
    cfg.jumpStrength = getFloat("jump_strength", cfg.jumpStrength);
    cfg.pipeSpeed = getFloat("pipe_speed", cfg.pipeSpeed);
    cfg.gapHeight = getFloat("gap_height", cfg.gapHeight);
    cfg.pipeSpawnInterval = getFloat("pipe_spawn_interval", cfg.pipeSpawnInterval);
    cfg.groundHeight = getFloat("ground_height", cfg.groundHeight);
    cfg.birdMinTilt = getFloat("bird_min_tilt", cfg.birdMinTilt);
    cfg.birdMaxTilt = getFloat("bird_max_tilt", cfg.birdMaxTilt);
    cfg.birdFlapRate = getFloat("bird_flap_rate", cfg.birdFlapRate);
    cfg.birdFlapDepth = getFloat("bird_flap_depth", cfg.birdFlapDepth);
    cfg.pipeSpeedMax = getFloat("pipe_speed_max", cfg.pipeSpeedMax);
    cfg.spawnIntervalMin = getFloat("spawn_interval_min", cfg.spawnIntervalMin);
    return cfg;
}

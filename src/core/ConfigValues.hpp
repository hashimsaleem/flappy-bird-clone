#ifndef CONFIG_VALUES_HPP
#define CONFIG_VALUES_HPP

#include <string>
#include "core/Config.hpp"
#include "core/json.hpp"

struct ConfigValues {
    // Screen
    int screenWidth = Config::SCREEN_WIDTH;
    int screenHeight = Config::SCREEN_HEIGHT;
    int targetFPS = Config::TARGET_FPS;

    // Bird
    float gravity = Config::GRAVITY;
    float jumpStrength = Config::JUMP_STRENGTH;
    float birdStartX = Config::BIRD_START_X;
    float birdStartY = Config::BIRD_START_Y;
    float birdWidth = Config::BIRD_WIDTH;
    float birdHeight = Config::BIRD_HEIGHT;
    float birdMinTilt = Config::BIRD_MIN_TILT;
    float birdMaxTilt = Config::BIRD_MAX_TILT;
    float birdFlapRate = Config::BIRD_FLAP_RATE;
    float birdFlapDepth = Config::BIRD_FLAP_DEPTH;
    float birdTiltLerp = Config::BIRD_TILT_LERP;

    // Pipe / Gameplay
    float pipeWidth = Config::PIPE_WIDTH;
    float pipeHeight = Config::PIPE_HEIGHT;
    float pipeSpeed = Config::PIPE_SPEED;
    float pipeSpeedMax = Config::PIPE_SPEED_MAX;
    float gapHeight = Config::GAP_HEIGHT;
    float pipeSpawnInterval = Config::PIPE_SPAWN_INTERVAL;
    float pipeMinY = Config::PIPE_MIN_Y;
    float pipeMaxY = Config::PIPE_MAX_Y;
    float pipeGapRange = Config::PIPE_GAP_RANGE;
    float spawnIntervalMin = Config::SPAWN_INTERVAL_MIN;

    // Assets
    std::string assetsDir = "assets/";
    std::string configPath = "gameconfig.json";
    std::string fontPath = "assets/font.ttf";
    std::string birdPath = "assets/bird.png";
    std::string bgMusic = "assets/bgm.ogg";

    // Ground / Physics
    float groundHeight = Config::GROUND_HEIGHT;
    float backgroundSpeed = Config::BACKGROUND_SPEED;

    // Visual
    float screenShakeDuration = Config::SCREEN_SHAKE_DURATION;
    float screenShakeIntensity = Config::SCREEN_SHAKE_INTENSITY;
};

ConfigValues loadConfig(const std::string& path);

#endif // CONFIG_VALUES_HPP

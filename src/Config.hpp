#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SFML/Graphics.hpp>

namespace Config {
    // Screen
    constexpr int SCREEN_WIDTH  = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr int TARGET_FPS    = 60;

    // Bird
    constexpr float GRAVITY          = 800.0f;
    constexpr float JUMP_STRENGTH    = -600.0f;
    constexpr float BIRD_START_X     = 50.0f;
    constexpr float BIRD_START_Y     = 300.0f;
    constexpr float BIRD_WIDTH       = 40.0f;
    constexpr float BIRD_HEIGHT      = 40.0f;

    // Pipe
    constexpr float PIPE_WIDTH       = 60.0f;
    constexpr float PIPE_HEIGHT      = 400.0f;
    constexpr float PIPE_SPEED       = 200.0f;
    constexpr float BACKGROUND_SPEED = 50.0f;
    constexpr float GAP_HEIGHT       = 150.0f;
    constexpr float PIPE_SPAWN_INTERVAL = 1.5f;
    constexpr float PIPE_MIN_Y       = 150.0f;
    constexpr float PIPE_MAX_Y       = 450.0f;
    constexpr float PIPE_GAP_RANGE   = 250.0f;

    // Difficulty caps
    constexpr float PIPE_SPEED_MAX     = 400.0f;
    constexpr float SPAWN_INTERVAL_MIN = 0.8f;

    // Assets (relative to executable directory)
    constexpr const char* FONT_PATH   = "assets/font.ttf";
    constexpr const char* BIRD_PATH   = "assets/bird.png";
    constexpr const char* ASSETS_DIR  = "assets/";
    constexpr const char* CONFIG_PATH = "gameconfig.json";

    // Audio
    constexpr const char* JUMP_SND    = "assets/jump.ogg";
    constexpr const char* SCORE_SND   = "assets/score.ogg";
    constexpr const char* DEATH_SND   = "assets/death.ogg";
    constexpr const char* BG_MUSIC    = "assets/bgm.ogg";

    // Ground
    constexpr float GROUND_HEIGHT    = 50.0f;
    constexpr sf::Color GROUND_COLOR = sf::Color(139, 119, 70);  // Sandy brown
    constexpr sf::Color GROUND_TOP   = sf::Color(107, 87, 48);   // Darker top edge

    // Bird animation
    constexpr float BIRD_MIN_TILT    = -30.0f;   // Degrees when flapping hard
    constexpr float BIRD_MAX_TILT    = 60.0f;    // Degrees when falling fast
    constexpr float BIRD_FLAP_RATE   = 8.0f;     // Wing flap oscillations per second
    constexpr float BIRD_FLAP_DEPTH  = 3.0f;     // Pixels of vertical oscillation
    constexpr float BIRD_TILT_LERP   = 5.0f;     // How fast tilt interpolates (higher = snappier)

    // Colors
    constexpr sf::Color SKY_COLOR    = sf::Color(135, 206, 235);
    constexpr sf::Color PIPE_COLOR   = sf::Color(34, 139, 34);
    constexpr sf::Color TEXT_COLOR   = sf::Color(255, 255, 255);
    constexpr sf::Color GAME_OVER_COLOR = sf::Color(255, 0, 0);

    // Difficulty Zones
    struct DifficultyZone {
        int minScore;
        int maxScore;
        float basePipeSpeed;
        float baseSpawnInterval;
    };

    const DifficultyZone EASY_ZONE   = {0, 50, 180.0f, 2.0f};
    const DifficultyZone NORMAL_ZONE = {51, 150, 220.0f, 1.6f};
    const DifficultyZone HARD_ZONE   = {151, 300, 280.0f, 1.2f};
    const DifficultyZone INSANE_ZONE  = {301, 9999, 350.0f, 0.9f};

    // TEST EDIT
    constexpr float SCORE_FLOAT_DURATION  = 0.8f;
    constexpr float SCORE_FLOAT_SPEED     = 60.0f;
    constexpr float SCORE_FLOAT_SCALE_MAX  = 1.6f;
    constexpr float SCORE_FLOAT_SCALE_SPEED = 4.0f;

    // Time-of-day (sky gradient)
    constexpr int SKY_COLOR_COUNT  = 4;
    constexpr sf::Color SKY_TOP[SKY_COLOR_COUNT] = {
        sf::Color(135, 206, 235),  // daytime blue
        sf::Color(255, 160,  50),  // sunset orange
        sf::Color( 20,  30,  60),  // dusk purple-blue
        sf::Color(  8,  12,  30)   // night dark blue
    };
    constexpr sf::Color SKY_BOT[SKY_COLOR_COUNT] = {
        sf::Color(180, 220, 240),  // daytime light blue
        sf::Color(220, 100,  40),  // sunset warm
        sf::Color( 15,  20,  50),  // dusk dark
        sf::Color(  5,   8,  20)   // night
    };
    constexpr float SKY_CYCLE_INTERVAL = 60.0f;  // seconds for full cycle

    // Ground
    constexpr int GROUND_TILE_W = 32;
    constexpr int GROUND_TILE_H = 16;
    constexpr int GROUND_TILE_COUNT = 4;
    constexpr float SCREEN_SHAKE_DURATION = 0.5f;
    constexpr float SCREEN_SHAKE_INTENSITY = 10.0f;
}

#endif // CONFIG_HPP

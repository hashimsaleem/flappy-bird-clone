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
}

#endif // CONFIG_HPP

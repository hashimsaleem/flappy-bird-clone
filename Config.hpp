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
    constexpr float GAP_HEIGHT       = 150.0f;
    constexpr float PIPE_SPAWN_INTERVAL = 1.5f;
    constexpr float PIPE_MIN_Y       = 150.0f;
    constexpr float PIPE_MAX_Y       = 450.0f;
    constexpr float PIPE_GAP_RANGE   = 250.0f;

    // Assets
    constexpr const char* FONT_PATH  = "font.ttf";
    constexpr const char* BIRD_PATH  = "bird.png";

    // Colors
    constexpr sf::Color SKY_COLOR    = sf::Color(135, 206, 235);
    constexpr sf::Color PIPE_COLOR   = sf::Color(34, 139, 34);
    constexpr sf::Color TEXT_COLOR   = sf::Color(255, 255, 255);
    constexpr sf::Color GAME_OVER_COLOR = sf::Color(255, 0, 0);
}

#endif // CONFIG_HPP

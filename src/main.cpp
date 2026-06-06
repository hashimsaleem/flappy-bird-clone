#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Config.hpp"

enum GameState { START, PLAYING, GAME_OVER };

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)Config::SCREEN_WIDTH, (unsigned int)Config::SCREEN_HEIGHT}), "Flappy Clone SFML");
    window.setFramerateLimit(Config::TARGET_FPS);

    sf::Font font;
    bool fontLoaded = false;
    // Note: You need to provide a font file (e.g., font.ttf) in the project directory.
    if (font.openFromFile(Config::FONT_PATH)) {
        fontLoaded = true;
    } else {
        std::cerr << "Warning: Could not load '" << Config::FONT_PATH << "'. Text rendering will be disabled." << std::endl;
    }

    sf::Text scoreText(font);
    if (fontLoaded) {
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(Config::TEXT_COLOR);
        scoreText.setPosition({10.f, 10.f});
    }

    sf::Text gameOverText(font);
    if (fontLoaded) {
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(Config::GAME_OVER_COLOR);
        gameOverText.setPosition({Config::SCREEN_WIDTH / 2.f - 150.f, Config::SCREEN_HEIGHT / 2.f - 25.f});
        gameOverText.setString("GAME OVER");
    }

    sf::Text startText(font);
    if (fontLoaded) {
        startText.setString("Press SPACE to Start");
        startText.setCharacterSize(36);
        startText.setFillColor(Config::TEXT_COLOR);
        startText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f});
    }

    Bird bird;
    bird.load(Config::BIRD_PATH);
    std::vector<Pipe> pipes;

    GameState currentState = START;
    int score = 0;
    float spawnTimer = 0.0f;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> yDist(Config::PIPE_MIN_Y, Config::PIPE_MAX_Y);
    std::uniform_real_distribution<float> gapDist(Config::PIPE_GAP_RANGE * 0.6f, Config::PIPE_GAP_RANGE);

    sf::Clock gameClock;
    gameClock.restart();

    std::cout << "Game Started. Press SPACE to begin." << std::endl;

    while (window.isOpen()) {
        sf::Time elapsed = gameClock.restart();
        float dt = elapsed.asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    if (currentState == START) {
                        currentState = PLAYING;
                        std::cout << "Game Playing!" << std::endl;
                    } else if (currentState == PLAYING) {
                        bird.flap();
                    } else if (currentState == GAME_OVER) {
                        // Reset game
                        bird = Bird();
                        bird.load(Config::BIRD_PATH);
                        pipes.clear();
                        score = 0;
                        currentState = PLAYING;
                        std::cout << "Restarting game..." << std::endl;
                    }
                }
            }
        }
        if (currentState == PLAYING) {
            bird.update(dt);

            // Boundary check
            sf::FloatRect birdBounds = bird.getBoundingBox();
            if (birdBounds.position.y < 0 || birdBounds.position.y + birdBounds.size.y > Config::SCREEN_HEIGHT) {
                currentState = GAME_OVER;
                std::cout << "Game Over! Final Score: " << score << std::endl;
            }

            for (auto& pipe : pipes) {
                pipe.update(dt);
                if (pipe.checkCollision(birdBounds)) {
                    currentState = GAME_OVER;
                    std::cout << "Game Over! Final Score: " << score << std::endl;
                }

                // Scoring logic
                if (!pipe.passed && pipe.getX() < 50.0f) {
                    score++;
                    pipe.passed = true;
                    std::cout << "Score: " << score << std::endl;
                }
            }

            // Remove pipes that are off-screen
            pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
                [](const Pipe& p) { return p.isOffScreen(); }), pipes.end());

            // Spawn new pipes
            spawnTimer += dt;
            if (spawnTimer > Config::PIPE_SPAWN_INTERVAL) {
                float randomY = yDist(generator);
                float randomGap = gapDist(generator);
                pipes.push_back(Pipe(Config::SCREEN_WIDTH, randomY, randomGap));
                spawnTimer = 0.0f;
            }
        }

        window.clear(Config::SKY_COLOR);

        if (currentState == PLAYING) {
            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            if (fontLoaded) {
                scoreText.setString("Score: " + std::to_string(score));
                window.draw(scoreText);
            }
        } else if (currentState == START) {
            bird.draw(window);
            if (fontLoaded) {
                startText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f});
                window.draw(startText);
            }
        } else if (currentState == GAME_OVER) {
            // Semi-transparent overlay
            sf::RectangleShape overlay;
            overlay.setSize({static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)});
            overlay.setFillColor(sf::Color(0, 0, 0, 128));
            window.draw(overlay);

            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            if (fontLoaded) {
                scoreText.setString("Score: " + std::to_string(score));
                window.draw(scoreText);
                window.draw(gameOverText);

                sf::Text restartText(font);
                restartText.setString("Press SPACE to Restart");
                restartText.setCharacterSize(28);
                restartText.setFillColor(Config::TEXT_COLOR);
                restartText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f + 50.f});
                window.draw(restartText);
            }
        }

        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include "ResourceManager.hpp"
#include "HighScore.hpp"

enum GameState { START, PLAYING, GAME_OVER };

int main() {
    // Load config file (overrides defaults if present)
    std::string configPath = std::string(Config::ASSETS_DIR) + Config::CONFIG_PATH;
    bool configLoaded = ConfigLoader::load(configPath);

    // Apply config overrides to screen settings
    unsigned int screenW = (unsigned int)Config::SCREEN_WIDTH;
    unsigned int screenH = (unsigned int)Config::SCREEN_HEIGHT;
    if (configLoaded) {
        int cw = ConfigLoader::getInt("screen_width", 0);
        int ch = ConfigLoader::getInt("screen_height", 0);
        if (cw > 0 && ch > 0) {
            screenW = (unsigned int)cw;
            screenH = (unsigned int)ch;
        }
    }

    sf::RenderWindow window(sf::VideoMode({screenW, screenH}), "Flappy Clone SFML");
    window.setFramerateLimit(Config::TARGET_FPS);

    sf::Font font = ResourceManager::getFont(Config::FONT_PATH, 30);
    bool fontLoaded = true; // Font is loaded or falls back to default in ResourceManager
    if (!fontLoaded) {
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

    sf::Text highScoreText(font);
    if (fontLoaded) {
        highScoreText.setCharacterSize(24);
        highScoreText.setFillColor(Config::TEXT_COLOR);
        highScoreText.setPosition({Config::SCREEN_WIDTH - 180.f, 10.f});
        highScoreText.setString("High Score: 0");
    }

    // Sound effects
    sf::Sound jumpSound = ResourceManager::getSound(Config::JUMP_SND);
    sf::Sound scoreSound = ResourceManager::getSound(Config::SCORE_SND);
    sf::Sound deathSound = ResourceManager::getSound(Config::DEATH_SND);

    // Background music (loops continuously during gameplay)
    sf::Music bgmMusic;
    bool bgmLoaded = bgmMusic.openFromFile(Config::BG_MUSIC);
    if (!bgmLoaded) {
        // Fallback: try WAV
        bgmLoaded = bgmMusic.openFromFile("assets/bgm_bgm.wav");
    }
    std::cerr << "[bgm] bgmLoaded=" << bgmLoaded << "\n";
    if (bgmLoaded) {
        bgmMusic.setLooping(true);
        bgmMusic.setVolume(40.f);
        bgmMusic.play();
    }

    int highScore = HighScore::load();

    // Ground surface
    float groundY = (float)Config::SCREEN_HEIGHT - Config::GROUND_HEIGHT;
    sf::RectangleShape groundShape;
    groundShape.setSize({(float)Config::SCREEN_WIDTH, Config::GROUND_HEIGHT});
    groundShape.setPosition({0.f, groundY});
    groundShape.setFillColor(Config::GROUND_COLOR);

    // Ground top edge (grass-like strip)
    sf::RectangleShape groundEdge;
    groundEdge.setSize({(float)Config::SCREEN_WIDTH, 8.f});
    groundEdge.setPosition({0.f, groundY});
    groundEdge.setFillColor(Config::GROUND_TOP);

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
                        if (bgmLoaded && bgmMusic.getStatus() != sf::SoundSource::Status::Playing) {
                            bgmMusic.play();
                        }
                        std::cout << "Game Playing!" << std::endl;
                    } else if (currentState == PLAYING) {
                        bird.flap();
                        if (jumpSound.getStatus() != sf::SoundSource::Status::Playing) {
                            jumpSound.play();
                        }
                    } else if (currentState == GAME_OVER) {
                        // Reset game — reset bird in-place to avoid shallow-copy double-free
                        bird.reset();
                        bird.load(Config::BIRD_PATH);
                        pipes.clear();
                        score = 0;
                        currentState = PLAYING;
                        if (bgmLoaded) bgmMusic.play();
                        std::cout << "Restarting game..." << std::endl;
                    }
                }
            }
        }
        if (currentState == PLAYING) {
            bird.update(dt);

            // Boundary check (including ground)
            sf::FloatRect birdBounds = bird.getBoundingBox();
            if (birdBounds.position.y < 0 || birdBounds.position.y + birdBounds.size.y > groundY) {
                currentState = GAME_OVER;
                std::cout << "Game Over! Final Score: " << score << std::endl;
                deathSound.play();
                if (bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
                    bgmMusic.stop();
            }

            for (auto& pipe : pipes) {
                pipe.update(dt);
                if (pipe.checkCollision(birdBounds)) {
                    currentState = GAME_OVER;
                    std::cout << "Game Over! Final Score: " << score << std::endl;
                    deathSound.play();
                    if (bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
                        bgmMusic.stop();
                }

                // Scoring logic
                if (!pipe.passed && pipe.getX() < 50.0f) {
                    score++;
                    pipe.passed = true;
                    std::cout << "Score: " << score << std::endl;
                    scoreSound.play();
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

        // Draw ground
        window.draw(groundShape);
        window.draw(groundEdge);

        if (currentState == PLAYING) {
            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            if (fontLoaded) {
                scoreText.setString("Score: " + std::to_string(score));
                window.draw(scoreText);
                highScoreText.setString("High Score: " + std::to_string(highScore));
                window.draw(highScoreText);
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

            // Update and display high score
            if (score > highScore) {
                highScore = score;
                HighScore::save(highScore);
            }
            if (fontLoaded) {
                highScoreText.setString("High Score: " + std::to_string(highScore));
                highScoreText.setPosition({Config::SCREEN_WIDTH / 2.f - 80.f, Config::SCREEN_HEIGHT / 2.f + 80.f});
                window.draw(highScoreText);
            }
        }

        window.display();
    }

    return 0;
}

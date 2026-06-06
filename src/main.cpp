#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include "ResourceManager.hpp"
#include "HighScore.hpp"
#include "Particle.hpp"

// Uncomment to enable debug console output
// #define DEBUG

enum GameState { MENU, START, PLAYING, GAME_OVER, HIGH_SCORE_SCREEN };

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

    const sf::Font& font = ResourceManager::getFont(Config::FONT_PATH, 30);

    sf::Text scoreText(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Config::TEXT_COLOR);
    scoreText.setPosition({10.f, 10.f});

    sf::Text gameOverText(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(Config::GAME_OVER_COLOR);
    gameOverText.setPosition({Config::SCREEN_WIDTH / 2.f - 150.f, Config::SCREEN_HEIGHT / 2.f - 25.f});
    gameOverText.setString("GAME OVER");

    sf::Text startText(font);
    startText.setString("Press SPACE to Start");
    startText.setCharacterSize(36);
    startText.setFillColor(Config::TEXT_COLOR);
    startText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f});

    sf::Text highScoreText(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(Config::TEXT_COLOR);
    highScoreText.setPosition({Config::SCREEN_WIDTH - 180.f, 10.f});
    highScoreText.setString("High Score: 0");

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
#ifdef DEBUG
    std::cerr << "[bgm] bgmLoaded=" << bgmLoaded << "\n";
#endif
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

    // Parallax background layer
    sf::RectangleShape bgLayer;
    bgLayer.setSize({(float)Config::SCREEN_WIDTH, 150.f}); // Simple layer
    bgLayer.setPosition({0.f, groundY - 150.f});
    bgLayer.setFillColor(sf::Color(100, 150, 100)); // Dark green hills

    float bgOffset = 0.f;

    // Ground top edge (grass-like strip)
    sf::RectangleShape groundEdge;
    groundEdge.setSize({(float)Config::SCREEN_WIDTH, 8.f});
    groundEdge.setPosition({0.f, groundY});
    groundEdge.setFillColor(Config::GROUND_TOP);

    Bird bird;
    bird.load(Config::BIRD_PATH);
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;

    // Difficulty caps (overridable via gameconfig.json)
    float pipeSpeedMax = ConfigLoader::getFloat("pipe_speed_max", Config::PIPE_SPEED_MAX);
    float spawnIntervalMin = ConfigLoader::getFloat("spawn_interval_min", Config::SPAWN_INTERVAL_MIN);

    float currentPipeSpeed = Config::PIPE_SPEED;
    float currentSpawnInterval = Config::PIPE_SPAWN_INTERVAL;

    // Menu options (defined here so input handler can use .size())
    const std::vector<std::string> menuOptions = {"Start Game", "High Scores", "Exit"};

    GameState currentState = MENU;
    int menuOption = 0;
    int score = 0;
    float spawnTimer = 0.0f;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> yDist(Config::PIPE_MIN_Y, Config::PIPE_MAX_Y);
    std::uniform_real_distribution<float> gapDist(Config::PIPE_GAP_RANGE * 0.6f, Config::PIPE_GAP_RANGE);
    std::uniform_int_distribution<int> particleDist(-100, 100);

#ifdef DEBUG
    std::cout << "Game Started. Press SPACE to begin." << std::endl;
#endif

    // Helper lambda to trigger game-over (avoids duplicating ~20 lines)
    auto triggerGameOver = [&]() {
        currentState = GAME_OVER;
#ifdef DEBUG
        std::cout << "Game Over! Final Score: " << score << std::endl;
#endif
        deathSound.play();
        bird.setDying();

        // Spawn particles on death
        sf::FloatRect birdBounds = bird.getBoundingBox();
        for (int i = 0; i < 20; ++i) {
            particles.emplace_back(birdBounds.position,
                sf::Vector2f(static_cast<float>(particleDist(generator)),
                             static_cast<float>(particleDist(generator))),
                1.0f);
        }

        if (bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
            bgmMusic.stop();
    };

    sf::Clock gameClock;
    gameClock.restart();

    while (window.isOpen()) {
        sf::Time elapsed = gameClock.restart();
        float dt = elapsed.asSeconds();

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (currentState == MENU) {
                    int optCount = (int)menuOptions.size();
                    if (keyPressed->code == sf::Keyboard::Key::Up) {
                        menuOption = (menuOption - 1 + optCount) % optCount;
                    } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                        menuOption = (menuOption + 1) % optCount;
                    } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (menuOption == 0) {
                            currentState = START;
                        } else if (menuOption == 1) {
                            currentState = HIGH_SCORE_SCREEN;
                        } else if (menuOption == 2) {
                            window.close();
                        }
                    }
                } else if (keyPressed->code == sf::Keyboard::Key::Space) {
                    if (currentState == START) {
                        currentState = PLAYING;
                        if (bgmLoaded && bgmMusic.getStatus() != sf::SoundSource::Status::Playing) {
                            bgmMusic.play();
                        }
#ifdef DEBUG
                        std::cout << "Game Playing!" << std::endl;
#endif
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
                        particles.clear();
                        score = 0;
                        currentPipeSpeed = Config::PIPE_SPEED;
                        currentSpawnInterval = Config::PIPE_SPAWN_INTERVAL;
                        currentState = PLAYING;
                        if (bgmLoaded) bgmMusic.play();
#ifdef DEBUG
                        std::cout << "Restarting game..." << std::endl;
#endif
                    }
                } else if (currentState == HIGH_SCORE_SCREEN && keyPressed->code == sf::Keyboard::Key::Escape) {
                    currentState = MENU;
                }
            }
        }
        if (currentState == PLAYING || currentState == GAME_OVER) {
            bird.update(dt);

            // Update particles
            for (auto& p : particles) p.update(dt);
            particles.erase(std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return p.lifetime <= 0; }), particles.end());

            if (currentState == PLAYING) {
                // Update background
                bgOffset -= Config::BACKGROUND_SPEED * dt;
                if (bgOffset <= -Config::SCREEN_WIDTH) {
                    bgOffset = 0.f;
                }

                // Boundary check (including ground)
                sf::FloatRect birdBounds = bird.getBoundingBox();
                if (birdBounds.position.y < 0 || birdBounds.position.y + birdBounds.size.y > groundY) {
                    triggerGameOver();
                }

                for (auto& pipe : pipes) {
                    pipe.update(dt);
                    if (pipe.checkCollision(birdBounds)) {
                        triggerGameOver();
                    }

                    // Scoring logic — compare against the bird's actual X position
                    float birdX = bird.getX();
                    if (!pipe.passed && pipe.getX() < birdX) {
                        score++;
                        pipe.passed = true;
#ifdef DEBUG
                        std::cout << "Score: " << score << std::endl;
#endif
                        scoreSound.play();

                        // Dynamic Difficulty: increase speed and decrease spawn interval every 5 points
                        if (score % 5 == 0) {
                            currentPipeSpeed *= 1.05f; // 5% speed increase
                            currentSpawnInterval *= 0.95f; // 5% faster spawn

                            // Caps to prevent game from becoming impossible
                            if (currentPipeSpeed > pipeSpeedMax) currentPipeSpeed = pipeSpeedMax;
                            if (currentSpawnInterval < spawnIntervalMin) currentSpawnInterval = spawnIntervalMin;

#ifdef DEBUG
                            std::cout << "[Difficulty Up] Speed: " << currentPipeSpeed << ", Interval: " << currentSpawnInterval << std::endl;
#endif
                        }
                    }
                }

                // Remove pipes that are off-screen
                pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
                    [](const Pipe& p) { return p.isOffScreen(); }), pipes.end());

                // Spawn new pipes
                spawnTimer += dt;
                if (spawnTimer > currentSpawnInterval) {
                    float randomY = yDist(generator);
                    float randomGap = gapDist(generator);

                    // 20% chance to spawn a moving pipe
                    std::uniform_int_distribution<int> typeDist(0, 4);
                    PipeType type = (typeDist(generator) == 0) ? PipeType::MOVING : PipeType::STATIC;

                    pipes.push_back(Pipe(Config::SCREEN_WIDTH, randomY, randomGap, currentPipeSpeed, type));
                    spawnTimer = 0.0f;
                }
            }
        }

        window.clear(Config::SKY_COLOR);

        // Draw background layer
        bgLayer.setPosition({bgOffset, groundY - 150.f});
        window.draw(bgLayer);
        bgLayer.setPosition({bgOffset + Config::SCREEN_WIDTH, groundY - 150.f});
        window.draw(bgLayer);

        // Draw ground
        window.draw(groundShape);
        window.draw(groundEdge);

        if (currentState == PLAYING) {
            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
            highScoreText.setString("High Score: " + std::to_string(highScore));
            window.draw(highScoreText);
        } else if (currentState == START) {
            bird.draw(window);
            startText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f});
            window.draw(startText);
        } else if (currentState == MENU) {
            sf::Text menuTitle(font);
            menuTitle.setString("FLAPPY CLONE");
            menuTitle.setCharacterSize(60);
            menuTitle.setFillColor(Config::TEXT_COLOR);
            menuTitle.setPosition({Config::SCREEN_WIDTH / 2.f - 200.f, 100.f});
            window.draw(menuTitle);

            for (int i = 0; i < (int)menuOptions.size(); ++i) {
                sf::Text optionText(font);
                optionText.setString(menuOptions[i]);
                optionText.setCharacterSize(30);
                optionText.setFillColor(i == menuOption ? sf::Color::Yellow : Config::TEXT_COLOR);
                optionText.setPosition({Config::SCREEN_WIDTH / 2.f - 100.f, 250.f + i * 50.f});
                window.draw(optionText);
            }
        } else if (currentState == HIGH_SCORE_SCREEN) {
            sf::Text hsTitle(font);
            hsTitle.setString("HIGH SCORE");
            hsTitle.setCharacterSize(50);
            hsTitle.setFillColor(Config::TEXT_COLOR);
            hsTitle.setPosition({Config::SCREEN_WIDTH / 2.f - 150.f, 150.f});
            window.draw(hsTitle);

            sf::Text hsValue(font);
            hsValue.setString(std::to_string(HighScore::load()));
            hsValue.setCharacterSize(80);
            hsValue.setFillColor(sf::Color::Yellow);
            hsValue.setPosition({Config::SCREEN_WIDTH / 2.f - 50.f, 250.f});
            window.draw(hsValue);

            sf::Text backText(font);
            backText.setString("Press ESC to return");
            backText.setCharacterSize(24);
            backText.setFillColor(Config::TEXT_COLOR);
            backText.setPosition({Config::SCREEN_WIDTH / 2.f - 100.f, 450.f});
            window.draw(backText);
        } else if (currentState == GAME_OVER) {
            // Semi-transparent overlay
            sf::RectangleShape overlay;
            overlay.setSize({static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)});
            overlay.setFillColor(sf::Color(0, 0, 0, 128));
            window.draw(overlay);

            // Draw particles
            for (const auto& p : particles) p.draw(window);

            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
            window.draw(gameOverText);

            sf::Text restartText(font);
            restartText.setString("Press SPACE to Restart");
            restartText.setCharacterSize(28);
            restartText.setFillColor(Config::TEXT_COLOR);
            restartText.setPosition({Config::SCREEN_WIDTH / 2.f - 130.f, Config::SCREEN_HEIGHT / 2.f + 50.f});
            window.draw(restartText);

            // Update and display high score
            if (score > highScore) {
                highScore = score;
                HighScore::save(highScore);
            }
            highScoreText.setString("High Score: " + std::to_string(highScore));
            highScoreText.setPosition({Config::SCREEN_WIDTH / 2.f - 80.f, Config::SCREEN_HEIGHT / 2.f + 80.f});
            window.draw(highScoreText);
        }

        window.display();
    }

    return 0;
}

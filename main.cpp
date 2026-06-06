#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "Bird.hpp"
#include "Pipe.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

enum GameState { START, PLAYING, GAME_OVER };

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)SCREEN_WIDTH, (unsigned int)SCREEN_HEIGHT}), "Flappy Clone SFML");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = false;
    // Note: You need to provide a font file (e.g., font.ttf) in the project directory.
    if (font.openFromFile("font.ttf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Warning: Could not load 'font.ttf'. Text rendering will be disabled." << std::endl;
    }

    sf::Text scoreText(font);
    if (fontLoaded) {
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({10.f, 10.f});
    }

    sf::Text gameOverText(font);
    if (fontLoaded) {
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition({SCREEN_WIDTH / 2.f - 150.f, SCREEN_HEIGHT / 2.f - 25.f});
        gameOverText.setString("GAME OVER");
    }

    Bird bird;
    bird.load("bird.png");
    std::vector<Pipe> pipes;

    GameState currentState = START;
    int score = 0;
    float spawnTimer = 0.0f;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> yDist(150.0f, 450.0f);
    std::uniform_real_distribution<float> gapDist(150.0f, 250.0f);

    sf::Clock gameClock;

    std::cout << "Game Started. Press SPACE to begin." << std::endl;

    while (window.isOpen()) {
        sf::Time elapsed = gameClock.restart();
        float dt = elapsed.asSeconds();

        while (auto event = window.pollEvent()) {
...            if (event->is<sf::Event::Closed>()) {
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
                        bird.load("bird.png");
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
            if (birdBounds.position.y < 0 || birdBounds.position.y + birdBounds.size.y > SCREEN_HEIGHT) {
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
            if (spawnTimer > 1.5f) {
                float randomY = yDist(generator);
                float randomGap = gapDist(generator);
                pipes.push_back(Pipe(SCREEN_WIDTH, randomY, randomGap));
                spawnTimer = 0.0f;
            }
        }

        window.clear(sf::Color(135, 206, 235));

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
            // Maybe draw a "Press Space" text?
        } else if (currentState == GAME_OVER) {
            bird.draw(window);
            for (const auto& pipe : pipes) {
                pipe.draw(window);
            }
            if (fontLoaded) {
                scoreText.setString("Score: " + std::to_string(score));
                window.draw(scoreText);
                window.draw(gameOverText);
            }
        }

        window.display();
    }

    return 0;
}

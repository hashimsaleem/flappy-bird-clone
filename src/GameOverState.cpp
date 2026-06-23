#include "GameOverState.h"
#include "Config.hpp"
#include "HighScore.hpp"
#include <algorithm>

GameOverState::GameOverState(BirdState birdState, std::vector<Pipe> pipes,
                             std::vector<Particle> particles,
                             std::vector<std::shared_ptr<ScoreFloat>> scoreFloats,
                             int score, int& highScoreRef)
    : birdState(birdState), pipes(std::move(pipes)), particles(std::move(particles)),
      scoreFloats(std::move(scoreFloats)), score(score), highScore(highScoreRef) {
    restartBirdState = birdState;
}

void GameOverState::update(float dt) {
    overlayAlpha = std::min(1.0f, overlayAlpha + dt * 2.5f);
    for (auto& p : particles) p.update(dt);
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.lifetime <= 0; }), particles.end());
    for (auto& sf : scoreFloats) sf->update(dt);
    scoreFloats.erase(std::remove_if(scoreFloats.begin(), scoreFloats.end(),
        [](const std::shared_ptr<ScoreFloat>& s) { return !s->alive(); }), scoreFloats.end());

    if (birdState.isDying) {
        birdState.velocityY += Config::GRAVITY * 1.5f * dt;
        birdState.posY += birdState.velocityY * dt;
        birdState.tiltAngle += 360.0f * dt;
        
        // Fix #16: Clamp to ground line
        float groundY = static_cast<float>(Config::SCREEN_HEIGHT - Config::GROUND_HEIGHT);
        if (birdState.posY + Config::BIRD_HEIGHT > groundY) {
            birdState.posY = groundY - Config::BIRD_HEIGHT;
            birdState.velocityY = 0.f;
        }
    }

    // Fix #19: Move HighScore save to update()
    if (score > highScore) {
        highScore = score;
        HighScore::save(highScore);
    }
}

void GameOverState::draw(sf::RenderWindow& window, const sf::Font& font) {
    float a = static_cast<unsigned char>(overlayAlpha * 128);
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, a));
    window.draw(overlay);

    for (const auto& p : particles) p.draw(window);

    sf::RectangleShape birdShape;
    birdShape.setSize(sf::Vector2f(Config::BIRD_WIDTH, Config::BIRD_HEIGHT));
    birdShape.setPosition(sf::Vector2f(birdState.posX - Config::BIRD_WIDTH / 2.f,
                                        birdState.posY - Config::BIRD_HEIGHT / 2.f));
    birdShape.setFillColor(sf::Color(255, 220, 50));
    birdShape.setRotation(sf::degrees(birdState.tiltAngle));
    window.draw(birdShape);

    for (const auto& pipe : pipes) pipe.draw(window);

    for (const auto& sf : scoreFloats) sf->draw(window);

    auto scoreText = makeText(font, "Score: " + std::to_string(score),
                              30, Config::TEXT_COLOR,
                              sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 60),
                                           static_cast<float>(Config::SCREEN_HEIGHT / 2 - 80)));
    window.draw(scoreText);

    auto gameOverText = makeText(font, "GAME OVER", 50, Config::GAME_OVER_COLOR,
        sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 150),
                     static_cast<float>(Config::SCREEN_HEIGHT / 2 - 25)));
    window.draw(gameOverText);

    auto restartText = makeText(font, "Press SPACE to Restart", 28,
        Config::TEXT_COLOR,
        sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 130),
                     static_cast<float>(Config::SCREEN_HEIGHT / 2 + 50)));
    window.draw(restartText);

    auto hsText = makeText(font, "High Score: " + std::to_string(highScore),
                           24, Config::TEXT_COLOR,
                           sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH / 2 - 80),
                                        static_cast<float>(Config::SCREEN_HEIGHT / 2 + 80)));
    window.draw(hsText);
}

void GameOverState::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Space) {
        nextActionCode = 1;
    } else if (key == sf::Keyboard::Key::Escape) {
        // Fix #15: Return to menu on Escape
        nextActionCode = 2;
    }
}

BirdState GameOverState::getRestartState() const {
    return restartBirdState;
}

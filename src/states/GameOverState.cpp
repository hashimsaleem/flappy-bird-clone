#include "GameOverState.h"
#include "core/Config.hpp"
#include "core/ConfigValues.hpp"
#include "core/HighScore.hpp"
#include <algorithm>

GameOverState::GameOverState(const ConfigValues& cfg, BirdState birdState, std::vector<Pipe> pipes,
                             std::vector<Particle> particles,
                             std::vector<std::shared_ptr<ScoreFloat>> scoreFloats,
                             int score, int& highScoreRef, int difficulty)
    : cfg(cfg), birdState(birdState), pipes(std::move(pipes)), particles(std::move(particles)),
      scoreFloats(std::move(scoreFloats)), score(score), highScore(highScoreRef), difficulty(difficulty) {
    restartBirdState = birdState;
    restartBirdState.difficulty = difficulty;

     if (score > highScore) {
        highScore = score;
        HighScore::save(highScore);
    }
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

        float groundY = static_cast<float>(cfg.screenHeight - cfg.groundHeight);
        if (birdState.posY + Config::BIRD_HEIGHT > groundY) {
            birdState.posY = groundY - Config::BIRD_HEIGHT;
            birdState.velocityY = 0.f;
        }
    }
}

void GameOverState::draw(sf::RenderWindow& window, const sf::Font& font) {
    float a = static_cast<unsigned char>(overlayAlpha * 255);
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(static_cast<float>(cfg.screenWidth), static_cast<float>(cfg.screenHeight)));
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

    std::string scoreStr = (score != lastScore) ? ("Score: " + std::to_string(score)) : cachedScoreStr;
    if (score != lastScore) {
        cachedScoreStr = scoreStr;
        lastScore = score;
    }
    auto scoreText = makeText(font, scoreStr,
                              30, Config::TEXT_COLOR,
                              sf::Vector2f(static_cast<float>(cfg.screenWidth / 2 - 60),
                                           static_cast<float>(cfg.screenHeight / 2 - 80)));
    window.draw(scoreText);

    auto gameOverText = makeText(font, "GAME OVER", 50, Config::GAME_OVER_COLOR,
        sf::Vector2f(static_cast<float>(cfg.screenWidth / 2 - 150),
                     static_cast<float>(cfg.screenHeight / 2 - 25)));
    window.draw(gameOverText);

    auto restartText = makeText(font, "Press SPACE to Restart", 28,
        Config::TEXT_COLOR,
        sf::Vector2f(static_cast<float>(cfg.screenWidth / 2 - 130),
                     static_cast<float>(cfg.screenHeight / 2 + 50)));
    window.draw(restartText);

    std::string hsStr = (highScore != lastHighScore) ? ("High Score: " + std::to_string(highScore)) : cachedHsStr;
    if (highScore != lastHighScore) {
        cachedHsStr = hsStr;
        lastHighScore = highScore;
    }
    auto hsText = makeText(font, hsStr,
                            24, Config::TEXT_COLOR,
                            sf::Vector2f(static_cast<float>(cfg.screenWidth / 2 - 80),
                                         static_cast<float>(cfg.screenHeight / 2 + 80)));
    window.draw(hsText);

    if (score >= 10) {
        sf::Color medalColor;
        std::string medalLabel;
        if (score >= 50) {
            medalColor = sf::Color(200, 200, 220);
            medalLabel = "PLATINUM";
        } else if (score >= 30) {
            medalColor = sf::Color(255, 215, 0);
            medalLabel = "GOLD";
        } else if (score >= 20) {
            medalColor = sf::Color(192, 192, 192);
            medalLabel = "SILVER";
        } else {
            medalColor = sf::Color(205, 127, 50);
            medalLabel = "BRONZE";
        }
        float cx = static_cast<float>(cfg.screenWidth) / 2.f;
        float cy = static_cast<float>(cfg.screenHeight) / 2.f - 130.f;
        sf::CircleShape medal(20.f, 6);
        medal.setFillColor(medalColor);
        medal.setOutlineColor(sf::Color::White);
        medal.setOutlineThickness(2.f);
        medal.setOrigin({20.f, 20.f});
        medal.setPosition({cx, cy});
        window.draw(medal);

       int medalScore = (score >= 50) ? 50 : (score >= 30) ? 30 : (score >= 20) ? 20 : 10;
        std::string medalStr = (medalScore != lastMedalScore) ? medalLabel : cachedMedalLabel;
        if (medalScore != lastMedalScore) {
            cachedMedalLabel = medalStr;
            lastMedalScore = medalScore;
        }
        auto medalText = makeText(font, medalStr, 18, medalColor,
            sf::Vector2f(cx - 35.f, cy + 25.f));
        window.draw(medalText);
    }
}

void GameOverState::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Space) {
        nextActionCode = StateAction::PlayGame;
    } else if (key == sf::Keyboard::Key::Escape) {
        nextActionCode = StateAction::ReturnToMenu;
    }
}

BirdState GameOverState::getRestartBirdState() const {
    return restartBirdState;
}

#include "PlayState.h"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

PlayState::PlayState(sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                     const sf::Font& fontRef, const std::string& assetDir,
                     float posX, float posY, float vel, int difficulty)
    : soundManager(std::make_unique<SoundManager>(bgmMusic, bgmLoaded)),
      bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
      font(&fontRef), difficulty(difficulty),
      scoreManager(std::make_unique<ScoreManager>(fontRef, difficulty)) {

    currentPipeSpeed = ConfigLoader::getFloat("pipe_speed", Config::PIPE_SPEED);
    currentSpawnInterval = ConfigLoader::getFloat("pipe_spawn_interval", Config::PIPE_SPAWN_INTERVAL);

    if (difficulty == 0) {
        currentPipeSpeed *= 0.7f;
        currentSpawnInterval *= 1.5f;
    } else if (difficulty == 2) {
        currentPipeSpeed *= 1.3f;
        currentSpawnInterval *= 0.7f;
    }

    yDist = std::uniform_real_distribution<float>(Config::PIPE_MIN_Y, Config::PIPE_MAX_Y);
    gapDist = std::uniform_real_distribution<float>(Config::PIPE_GAP_RANGE * 0.6f, Config::PIPE_GAP_RANGE);
    typeDist = std::uniform_int_distribution<int>(0, 4);

    bird.load(assetDir + Config::BIRD_PATH);
    restartPosX = posX;
    restartPosY = posY;
    restartVel = vel;

    pipePool = std::make_unique<ObjectPool<Pipe>>([&]() {
        return Pipe(0, 0, Config::GAP_HEIGHT, Config::PIPE_SPEED);
    });
    powerUpPool = std::make_unique<ObjectPool<PowerUp>>([]() {
        return PowerUp(0, 0, PowerUpType::INVINCIBILITY);
    });
    visualEffects = std::make_unique<VisualEffectManager>();
}

void PlayState::onEnter() {
    bird.reset();
    bird.setRestartPos(restartPosX, restartPosY);
    bird.setRestartVel(restartVel);
    countdownTimer = 3.0f;
    gameStarted = false;
    paused = false;
    quitToMenu = false;
}

void PlayState::handleKeyPress(sf::Keyboard::Key key) {
    if (gameOverTriggered) return;

    if (key == sf::Keyboard::Key::Escape) {
        if (!gameStarted) return;
        paused = !paused;
        return;
    }

    if (paused) {
        if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
            pauseSelected = (pauseSelected - 1 + 2) % 2;
        } else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
            pauseSelected = (pauseSelected + 1) % 2;
        } else if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
            if (pauseSelected == 0) {
                paused = false;
            } else {
                quitToMenu = true;
            }
        }
        return;
    }

    if (key == sf::Keyboard::Key::Space) {
        if (!gameStarted) return;
        bird.flap();
    }
}

void PlayState::triggerGameOver() {
    if (gameOverTriggered) return;
    gameOverTriggered = true;
    gameOverSnapshot = takeSnapshot();

    soundManager->playDeath();
    bird.setDying();

    visualEffects->spawnParticles(bird.getBoundingBox().position, 20, {0.f, 0.f});

    shakeTimer = Config::SCREEN_SHAKE_DURATION;
    shakeIntensity = Config::SCREEN_SHAKE_INTENSITY;

    if (bgmLoaded && bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
        bgmMusic.stop();
}

void PlayState::update(float dt) {
    float effectiveDt = dt * slowMoFactor;

    if (gameOverTriggered) {
        bird.update(effectiveDt);
        return;
    }

    if (!gameStarted) {
        countdownTimer -= dt;
        if (countdownTimer <= 0.0f) {
            gameStarted = true;
        }
        visualEffects->update(effectiveDt, scoreManager->getCurrentPipeSpeed());
        return;
    }

    if (shakeTimer > 0.f) {
        shakeTimer -= dt;
        float decay = shakeTimer / Config::SCREEN_SHAKE_DURATION;
        shakeOffset.x = std::sin(shakeTimer * 30.f) * shakeIntensity * decay;
        shakeOffset.y = std::cos(shakeTimer * 27.f) * shakeIntensity * decay;
    } else {
        shakeOffset = {0.f, 0.f};
    }

  for (auto& sf : scoreManager->getScoreFloats()) sf->update(dt);
    scoreManager->removeExpiredScoreFloats();

    visualEffects->update(effectiveDt, scoreManager->getCurrentPipeSpeed());

    float currentBounceTimer = scoreManager->getScoreBounceTimer();
    if (currentBounceTimer > 0.f) {
        float newTimer = currentBounceTimer - dt;
        scoreManager->setScoreBounceTimer(newTimer);
        float newScale = 1.f + 0.4f * std::min(newTimer / 0.3f, 1.f);
        scoreManager->setScoreScale(newScale > 1.f ? newScale : 1.f);
    } else {
        scoreManager->setScoreScale(1.f);
    }

    bird.update(effectiveDt);

    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
    if (bird.getBoundingBox().position.y < 0 || bird.getBoundingBox().position.y + bird.getBoundingBox().size.y > groundY) {
        triggerGameOver();
        return;
    }

    for (auto it = activePipes.begin(); it != activePipes.end(); ) {
        int idx = *it;
        Pipe& pipe = (*pipePool)[idx];
        pipe.update(effectiveDt);
        float currentGap = ConfigLoader::getFloat("gap_height", Config::GAP_HEIGHT);
        if (pipe.checkCollision(bird.getBoundingBox())) {
            triggerGameOver();
            return;
        }

        float birdX = bird.getX();
        if (!pipe.passed && pipe.getX() < birdX) {
            pipe.passed = true;
            scoreManager->addScore();
            scoreManager->setScoreBounceTimer(0.3f);
            soundManager->playScore();
            scoreManager->pushScoreFloat(*font, sf::Vector2f(bird.getBoundingBox().position.x, bird.getBoundingBox().position.y - 20.f));

            if (scoreManager->getScore() % 5 == 0) {
                // This block is now handled by the hybrid scaling logic below
            }
        }

        if (pipe.isOffScreen()) {
            pipePool->release(idx);
            it = activePipes.erase(it);
        } else {
            ++it;
        }
    }

  spawnTimer += effectiveDt;
    int currentScore = scoreManager->getScore();
    float globalLerp = 0.0f;
    const Config::DifficultyZone* activeZone = &Config::EASY_ZONE;

    if (currentScore <= Config::EASY_ZONE.maxScore) {
        activeZone = &Config::EASY_ZONE;
        globalLerp = static_cast<float>(currentScore) / static_cast<float>(Config::INSANE_ZONE.maxScore);
    } else if (currentScore <= Config::NORMAL_ZONE.maxScore) {
        activeZone = &Config::NORMAL_ZONE;
        globalLerp = static_cast<float>(currentScore) / static_cast<float>(Config::INSANE_ZONE.maxScore);
    } else if (currentScore <= Config::HARD_ZONE.maxScore) {
        activeZone = &Config::HARD_ZONE;
        globalLerp = static_cast<float>(currentScore) / static_cast<float>(Config::INSANE_ZONE.maxScore);
    } else {
        activeZone = &Config::INSANE_ZONE;
        globalLerp = static_cast<float>(currentScore) / static_cast<float>(Config::INSANE_ZONE.maxScore);
    }
    if (globalLerp > 1.0f) globalLerp = 1.0f;
    float targetSpeed = Config::PIPE_SPEED + (Config::PIPE_SPEED_MAX - Config::PIPE_SPEED) * globalLerp;
    float targetInterval = Config::PIPE_SPAWN_INTERVAL - (Config::PIPE_SPAWN_INTERVAL - Config::SPAWN_INTERVAL_MIN) * globalLerp;

    scoreManager->setCurrentPipeSpeed(targetSpeed);
    scoreManager->setCurrentSpawnInterval(targetInterval);

    if (spawnTimer > scoreManager->getCurrentSpawnInterval()) {
        float randomY = yDist(rng);
        float randomGap = gapDist(rng);
        PipeType type = (typeDist(rng) == 0) ? PipeType::MOVING : PipeType::STATIC;
        int idx = pipePool->acquire();
        (*pipePool)[idx].reset(static_cast<float>(Config::SCREEN_WIDTH), randomY, randomGap, scoreManager->getCurrentPipeSpeed(), type);
        activePipes.push_back(idx);
        spawnTimer = 0.f;
    }

    visualEffects->update(effectiveDt, scoreManager->getCurrentPipeSpeed());

    for (auto it = activePowerUps.begin(); it != activePowerUps.end(); ) {
        int idx = *it;
        PowerUp& p = (*powerUpPool)[idx];
        p.update(effectiveDt);
        if (p.checkCollision(bird.getBoundingBox())) {
            if (p.getType() == PowerUpType::INVINCIBILITY) {
                bird.setInvincible(true);
                scoreManager->setScoreBounceTimer(5.0f);
            } else if (p.getType() == PowerUpType::SLOW_MOTION) {
                slowMoFactor = 0.5f;
                scoreManager->setScoreBounceTimer(5.0f);
            }
            powerUpPool->release(idx);
            it = activePowerUps.erase(it);
        } else if (p.isOffScreen()) {
            powerUpPool->release(idx);
            it = activePowerUps.erase(it);
        } else {
            ++it;
        }
    }

    float remainingBounceTimer = scoreManager->getScoreBounceTimer();
    if (remainingBounceTimer > 0.f) {
        float newTimer = remainingBounceTimer - effectiveDt;
        scoreManager->setScoreBounceTimer(newTimer);
        if (newTimer <= 0.f) {
            bird.setInvincible(false);
            slowMoFactor = 1.0f;
        }
    }
}

void PlayState::draw(sf::RenderWindow& window, const sf::Font& font) {
    sf::View originalView = window.getView();
    sf::View shakeView = originalView;
    shakeView.setCenter(originalView.getCenter() + sf::Vector2f(shakeOffset.x, shakeOffset.y));
    window.setView(shakeView);

    visualEffects->draw(window);

    for (int idx : activePipes) (*pipePool)[idx].draw(window);
    for (int idx : activePowerUps) (*powerUpPool)[idx].draw(window);
    bird.draw(window);

    for (const auto& sf : scoreManager->getScoreFloats()) sf->draw(window);

    auto scoreText = makeText(font, "Score: " + std::to_string(scoreManager->getScore()),
                                 30, Config::TEXT_COLOR,
                                 sf::Vector2f(10.f, 10.f));
    scoreText.setScale({scoreManager->getScoreScale(), scoreManager->getScoreScale()});
    window.draw(scoreText);

    auto hsText = makeText(font, "High Score: " + std::to_string(highScore),
                             24, Config::TEXT_COLOR,
                             sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH - 200.f), 10.f));
    window.draw(hsText);

    if (!gameStarted) {
        std::string countStr;
        int n = static_cast<int>(std::ceil(countdownTimer));
        countStr = (n >= 1) ? std::to_string(n) : "GO!";
        auto countText = makeText(font, countStr, 80,
            (n >= 1) ? sf::Color::White : sf::Color::Yellow,
            sf::Vector2f(0.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 100.f));
        centerText(window, countText);
        window.draw(countText);
    }

    if (paused) {
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), static_cast<float>(Config::SCREEN_HEIGHT)));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        auto pauseTitle = makeText(font, "PAUSED", 60, sf::Color::White,
            sf::Vector2f(0.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 100.f));
        centerText(window, pauseTitle);
        window.draw(pauseTitle);

        auto resumeText = makeText(font,
            (pauseSelected == 0 ? "> Resume" : "  Resume"), 30,
            (pauseSelected == 0 ? sf::Color::Yellow : sf::Color::White),
            sf::Vector2f(0.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f));
        centerText(window, resumeText);
        window.draw(resumeText);

        auto quitText = makeText(font,
            (pauseSelected == 1 ? "> Quit" : "  Quit"), 30,
            (pauseSelected == 1 ? sf::Color::Yellow : sf::Color::White),
            sf::Vector2f(0.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f + 50.f));
        centerText(window, quitText);
        window.draw(quitText);
    }

    window.setView(originalView);

    // Debug Overlay
    int currentScore = scoreManager->getScore();
    float lerpFactor = 0.0f;
    const Config::DifficultyZone* activeZone = &Config::INSANE_ZONE;

    if (currentScore <= Config::EASY_ZONE.maxScore) {
        activeZone = &Config::EASY_ZONE;
        lerpFactor = std::min(1.0f, static_cast<float>(currentScore - Config::EASY_ZONE.minScore) / (Config::EASY_ZONE.maxScore - Config::EASY_ZONE.minScore));
    } else if (currentScore <= Config::NORMAL_ZONE.maxScore) {
        activeZone = &Config::NORMAL_ZONE;
        lerpFactor = std::min(1.0f, static_cast<float>(currentScore - Config::NORMAL_ZONE.minScore) / (Config::NORMAL_ZONE.maxScore - Config::NORMAL_ZONE.minScore));
    } else if (currentScore <= Config::HARD_ZONE.maxScore) {
        activeZone = &Config::HARD_ZONE;
        lerpFactor = std::min(1.0f, static_cast<float>(currentScore - Config::HARD_ZONE.minScore) / (Config::HARD_ZONE.maxScore - Config::HARD_ZONE.minScore));
    } else {
        activeZone = &Config::INSANE_ZONE;
        lerpFactor = std::min(1.0f, static_cast<float>(currentScore - Config::INSANE_ZONE.minScore) / (Config::INSANE_ZONE.maxScore - Config::INSANE_ZONE.minScore));
    }

    std::string zoneName = "INSANE";
    if (currentScore <= Config::EASY_ZONE.maxScore) zoneName = "EASY";
    else if (currentScore <= Config::NORMAL_ZONE.maxScore) zoneName = "NORMAL";
    else if (currentScore <= Config::HARD_ZONE.maxScore) zoneName = "HARD";

    auto debugText = makeText(font, "Zone: " + zoneName + " | Lerp: " + std::to_string(lerpFactor).substr(0, 4),
                             18, sf::Color::Yellow,
                             sf::Vector2f(10.f, static_cast<float>(Config::SCREEN_HEIGHT) - 30.f));
    window.draw(debugText);

    return;
}

PlayStateSnapshot PlayState::takeSnapshot() const {
    PlayStateSnapshot snap;
    BirdState bs;
    bs.posX = bird.getX();
    bs.posY = bird.getBoundingBox().position.y + bird.getBoundingBox().size.y / 2.f;
    bs.isDying = bird.isDyingFlag();
    bs.velocityY = bird.getVelocity();
    bs.tiltAngle = 0.0f;
    bs.flapTimer = 0.0f;
    snap.birdState = bs;
    snap.score = scoreManager->getScore();
    snap.difficulty = scoreManager->getDifficulty();

    for (int idx : activePipes) {
        snap.pipes.push_back((*pipePool)[idx]);
    }
    const auto& particles = visualEffects->getParticles();
    snap.particles.insert(snap.particles.end(), particles.begin(), particles.end());

    snap.scoreFloats = scoreManager->getScoreFloats();
    return snap;
}

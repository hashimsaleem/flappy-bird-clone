#include "PlayState.h"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

PlayState::PlayState(sf::Sound* jumpSnd, sf::Sound* scoreSnd, sf::Sound* deathSnd,
                     sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                     const sf::Font& fontRef, const std::string& assetDir,
                     float posX, float posY, float vel, int difficulty)
    : jumpSound(jumpSnd), scoreSound(scoreSnd), deathSound(deathSnd),
      bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
      font(&fontRef), difficulty(difficulty) {
    jumpSound->setVolume(60.f);

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

    deathSound->play();
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
        visualEffects->update(effectiveDt, currentPipeSpeed);
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

    for (auto& sf : scoreFloats) sf->update(effectiveDt);
    scoreFloats.erase(std::remove_if(scoreFloats.begin(), scoreFloats.end(),
        [](const std::shared_ptr<ScoreFloat>& s) { return !s->alive(); }), scoreFloats.end());

    visualEffects->update(effectiveDt, currentPipeSpeed);

    if (scoreBounceTimer > 0.f) {
        scoreBounceTimer -= effectiveDt;
        scoreScale = 1.f + 0.4f * std::min(scoreBounceTimer / 0.3f, 1.f);
        if (scoreBounceTimer <= 0.f) scoreScale = 1.f;
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
            score++;
            scoreBounceTimer = 0.3f;
            scoreSound->play();
            scoreFloats.push_back(std::make_shared<ScoreFloat>(*font, sf::Vector2f(bird.getBoundingBox().position.x, bird.getBoundingBox().position.y - 20.f)));

            if (score % 5 == 0) {
                currentPipeSpeed *= 1.05f;
                currentSpawnInterval *= 0.95f;
                float pipeSpeedMax = ConfigLoader::getFloat("pipe_speed_max", Config::PIPE_SPEED_MAX);
                float spawnIntervalMin = ConfigLoader::getFloat("spawn_interval_min", Config::SPAWN_INTERVAL_MIN);
                if (currentPipeSpeed > pipeSpeedMax) currentPipeSpeed = pipeSpeedMax;
                if (currentSpawnInterval < spawnIntervalMin) currentSpawnInterval = spawnIntervalMin;
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
    if (spawnTimer > currentSpawnInterval) {
        float randomY = yDist(rng);
        float randomGap = gapDist(rng);
        PipeType type = (typeDist(rng) == 0) ? PipeType::MOVING : PipeType::STATIC;
        int idx = pipePool->acquire();
        (*pipePool)[idx].reset(static_cast<float>(Config::SCREEN_WIDTH), randomY, randomGap, currentPipeSpeed, type);
        activePipes.push_back(idx);
        spawnTimer = 0.f;
    }

    visualEffects->update(effectiveDt);

    for (auto it = activePowerUps.begin(); it != activePowerUps.end(); ) {
        int idx = *it;
        PowerUp& p = (*powerUpPool)[idx];
        p.update(effectiveDt);
        if (p.checkCollision(bird.getBoundingBox())) {
            if (p.getType() == PowerUpType::INVINCIBILITY) {
                bird.setInvincible(true);
                scoreBounceTimer = 5.0f;
            } else if (p.getType() == PowerUpType::SLOW_MOTION) {
                slowMoFactor = 0.5f;
                scoreBounceTimer = 5.0f;
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

    if (scoreBounceTimer > 0.f) {
        scoreBounceTimer -= effectiveDt;
        if (scoreBounceTimer <= 0.f) {
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

    for (const auto& sf : scoreFloats) sf->draw(window);

    auto scoreText = makeText(font, "Score: " + std::to_string(score),
                               30, Config::TEXT_COLOR,
                               sf::Vector2f(10.f, 10.f));
    scoreText.setScale({scoreScale, scoreScale});
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
    snap.score = score;
    snap.difficulty = difficulty;

    for (int idx : activePipes) {
        snap.pipes.push_back((*pipePool)[idx]);
    }
    for (int idx : activeParticles) {
        snap.particles.push_back((*particlePool)[idx]);
    }

    snap.scoreFloats = scoreFloats;
    return snap;
}

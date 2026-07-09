#include "PlayState.h"
#include "core/ConfigValues.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

PlayState::PlayState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                      const sf::Font& fontRef,
                      float posX, float posY, float vel, int difficulty,
                      const std::string& assetsBase)
    : cfg(cfg),
      soundManager(std::make_unique<SoundManager>(bgmMusic, bgmLoaded)),
      bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
      font(&fontRef), difficulty(difficulty),
      scoreManager(std::make_unique<ScoreManager>(cfg, fontRef, difficulty)),
      bird(cfg),
      assetsBase(assetsBase) {

    currentPipeSpeed = cfg.pipeSpeed;
    currentSpawnInterval = cfg.pipeSpawnInterval;

    if (difficulty == 0) {
        currentPipeSpeed *= 0.7f;
        currentSpawnInterval *= 1.5f;
    } else if (difficulty == 2) {
        currentPipeSpeed *= 1.3f;
        currentSpawnInterval *= 0.7f;
    }

    yDist = std::uniform_real_distribution<float>(cfg.pipeMinY, cfg.pipeMaxY);
    gapDist = std::uniform_real_distribution<float>(cfg.pipeGapRange * 0.6f, cfg.pipeGapRange);
    typeDist = std::uniform_int_distribution<int>(0, 4);

    bird.load(assetsBase + cfg.birdPath);
    restartPosX = posX;
    restartPosY = posY;
    restartVel = vel;
    powerUpSpawnTimer = 0.f;
    lastPowerUpScore = 0;

    pipePool = std::make_unique<ObjectPool<Pipe>>([&]() {
        return Pipe(0, 0, cfg.gapHeight, cfg.pipeSpeed);
    });
    powerUpPool = std::make_unique<ObjectPool<PowerUp>>([]() {
        return PowerUp(0, 0, PowerUpType::INVINCIBILITY);
    });
    visualEffects = std::make_unique<VisualEffectManager>(cfg);
}

void PlayState::onEnter() {
    bird.reset(cfg);
    bird.setRestartPos(restartPosX, restartPosY);
    bird.setRestartVel(restartVel);
   countdownTimer = 3.0f;
    gameStarted = false;
    paused = false;
    quitToMenu = false;
    soundManager->fadeBGM(40.f, 0.5f);
    powerUpSpawnTimer = 0.f;
    lastPowerUpScore = 0;
    cachedFmod4 = 0.f;
    cachedFmod4Score = -1;
    for (int idx : activePowerUps) {
        powerUpPool->release(idx);
    }
    activePowerUps.clear();
    slowMoFactor = 1.0f;
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
                soundManager->stopBGM();
            }
        }
        return;
    }

    if (key == sf::Keyboard::Key::Space) {
        if (!gameStarted) return;
        bird.flap();
    }
}

void PlayState::updateTheme() {
    int s = scoreManager->getScore();
    if (s < 50) currentTheme = Config::THEME_SKY;
    else if (s < 150) currentTheme = Config::THEME_CAVE;
    else if (s < 300) currentTheme = Config::THEME_LAVA;
    else currentTheme = Config::THEME_SPACE;
}

void PlayState::triggerGameOver() {
    if (gameOverTriggered) return;
    gameOverTriggered = true;
    gameOverSnapshot = takeSnapshot();

    soundManager->playDeath();
    soundManager->fadeBGM(20.f, 1.0f);
    bird.setDying();

    visualEffects->spawnParticles(bird.getBoundingBox().position, 20, {0.f, 0.f});
    visualEffects->spawnSparks(bird.getBoundingBox().position, 15);

    shakeTimer = cfg.screenShakeDuration;
    shakeIntensity = cfg.screenShakeIntensity;

    if (bgmLoaded && bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
        bgmMusic.stop();
}

void PlayState::update(float dt) {
    soundManager->update(dt);
    updateTheme();
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
        float decay = shakeTimer / cfg.screenShakeDuration;
        shakeOffset.x = std::sin(shakeTimer * 30.f) * shakeIntensity * decay;
        shakeOffset.y = std::cos(shakeTimer * 27.f) * shakeIntensity * decay;
    } else {
        shakeOffset = {0.f, 0.f};
    }

  for (auto& sf : scoreManager->getScoreFloats()) sf->update(dt);
    scoreManager->removeExpiredScoreFloats();
    scoreManager->updateBounceTimer(dt);

    bird.update(effectiveDt);

    // Shield particles when invincible
    if (bird.isInvincibleFlag()) {
        visualEffects->spawnSparks(bird.getBoundingBox().position, 2);
    }

    // Dust trail when bird is near ground
    float groundY = static_cast<float>(cfg.screenHeight - cfg.groundHeight);
    if (bird.getBoundingBox().position.y + bird.getBoundingBox().size.y > groundY - 20.f) {
        dustSpawnTimer += dt;
        if (dustSpawnTimer > 0.03f) {
            dustSpawnTimer = 0.f;
            visualEffects->spawnDust({bird.getBoundingBox().position.x, groundY - 5.f}, 1);
        }
    } else {
        dustSpawnTimer = 0.f;
    }

    if (bird.getBoundingBox().position.y < 0 || bird.getBoundingBox().position.y + bird.getBoundingBox().size.y > groundY) {
        triggerGameOver();
        return;
    }

    for (auto it = activePipes.begin(); it != activePipes.end(); ) {
        int idx = *it;
        Pipe& pipe = (*pipePool)[idx];
        pipe.update(effectiveDt);
        float currentGap = cfg.gapHeight;
        if (!bird.isInvincibleFlag() && pipe.checkCollision(bird.getBoundingBox())) {
            triggerGameOver();
            return;
        }

        float birdX = bird.getX();
        if (!pipe.passed && pipe.getX() < birdX) {
            pipe.passed = true;
            scoreManager->addScore();
            scoreManager->setScoreBounceTimer(0.3f);
            soundManager->playScore();
            visualEffects->spawnScoreSparkle({cfg.birdStartX - 30.f, cfg.birdStartY - 30.f}, 3);
            scoreManager->pushScoreFloat(*font, sf::Vector2f(bird.getBoundingBox().position.x, bird.getBoundingBox().position.y - 20.f));

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
    if (currentScore != cachedFmod4Score) {
        cachedFmod4Score = currentScore;
        cachedFmod4 = std::fmod(static_cast<float>(currentScore), 4.0f);
    }
    float fmodScore = cachedFmod4;
    float globalLerp = std::min(1.0f, static_cast<float>(currentScore) / static_cast<float>(Config::INSANE_ZONE.maxScore));
    const Config::DifficultyZone* activeZone = &Config::EASY_ZONE;
    if (currentScore > Config::EASY_ZONE.maxScore) activeZone = &Config::NORMAL_ZONE;
    if (currentScore > Config::NORMAL_ZONE.maxScore) activeZone = &Config::HARD_ZONE;
    if (currentScore > Config::HARD_ZONE.maxScore) activeZone = &Config::INSANE_ZONE;

     float targetSpeed = cfg.pipeSpeed + (cfg.pipeSpeedMax - cfg.pipeSpeed) * globalLerp;
    float targetInterval = cfg.pipeSpawnInterval - (cfg.pipeSpawnInterval - cfg.spawnIntervalMin) * globalLerp;

    scoreManager->setCurrentPipeSpeed(targetSpeed);
    scoreManager->setCurrentSpawnInterval(targetInterval);

    if (spawnTimer > scoreManager->getCurrentSpawnInterval()) {
        float randomY = yDist(rng);
        float randomGap = gapDist(rng);
        PipeType type = (typeDist(rng) == 0) ? PipeType::MOVING : PipeType::STATIC;
        int idx = pipePool->acquire();
        (*pipePool)[idx].reset(static_cast<float>(cfg.screenWidth), randomY, randomGap, scoreManager->getCurrentPipeSpeed(), type);
        activePipes.push_back(idx);
        spawnTimer = 0.f;
    }

    // Power-up spawning: spawn a power-up every 8-12 seconds
    powerUpSpawnTimer += effectiveDt;
    if (powerUpSpawnTimer >= 8.0f + fmodScore) {
        int pidx = powerUpPool->acquire();
        float puType = typeDist(rng);
        PowerUpType puTypeVal = (puType < 2.5f) ? PowerUpType::INVINCIBILITY : PowerUpType::SLOW_MOTION;
        float randomY = yDist(rng);
        (*powerUpPool)[pidx].reset(static_cast<float>(cfg.screenWidth) + 30.f, randomY, puTypeVal);
        activePowerUps.push_back(pidx);
        powerUpSpawnTimer = 0.f;
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
    if (remainingBounceTimer <= 0.f) {
        bird.setInvincible(false);
        slowMoFactor = 1.0f;
    }
}

void PlayState::draw(sf::RenderWindow& window, const sf::Font& font) {
    sf::View originalView = window.getView();
    sf::View shakeView = originalView;
    shakeView.setCenter(originalView.getCenter() + sf::Vector2f(shakeOffset.x, shakeOffset.y));
    window.setView(shakeView);

    // Theme-aware background rendering
    const auto& tc = Config::THEME_COLORS[currentTheme];
    float halfH = static_cast<float>(cfg.screenHeight) / 2.f;
    float groundY = static_cast<float>(cfg.screenHeight - cfg.groundHeight);

    sf::RectangleShape skyTopRect;
    skyTopRect.setSize({static_cast<float>(cfg.screenWidth), halfH});
    skyTopRect.setFillColor(tc.skyTop);
    window.draw(skyTopRect);

    sf::RectangleShape skyBotRect;
    skyBotRect.setSize({static_cast<float>(cfg.screenWidth), halfH});
    skyBotRect.setPosition({0.f, halfH});
    skyBotRect.setFillColor(tc.skyBot);
    window.draw(skyBotRect);

    sf::RectangleShape hills1;
    hills1.setSize({static_cast<float>(cfg.screenWidth) * 2.f, 120.f});
    hills1.setPosition({-std::fmod(shakeOffset.x * 0.1f, static_cast<float>(cfg.screenWidth)), groundY - 120.f});
    hills1.setFillColor(tc.bgLayer);
    window.draw(hills1);

    sf::RectangleShape hills2;
    hills2.setSize({static_cast<float>(cfg.screenWidth) * 2.f, 60.f});
    hills2.setPosition({-std::fmod(shakeOffset.x * 0.15f, static_cast<float>(cfg.screenWidth)), groundY - 60.f});
    sf::Color hills2Color(tc.bgLayer.r + 20, tc.bgLayer.g + 20, tc.bgLayer.b + 10);
    hills2.setFillColor(hills2Color);
    window.draw(hills2);

    sf::RectangleShape groundRect;
    groundRect.setSize({static_cast<float>(cfg.screenWidth), static_cast<float>(cfg.groundHeight)});
    groundRect.setPosition({0.f, groundY});
    groundRect.setFillColor(tc.ground);
    window.draw(groundRect);

    sf::RectangleShape groundTop;
    groundTop.setSize({static_cast<float>(cfg.screenWidth), 8.f});
    groundTop.setPosition({0.f, groundY});
    groundTop.setFillColor(tc.groundTop);
    window.draw(groundTop);

    for (float gx = 0.f; gx < static_cast<float>(cfg.screenWidth); gx += 12.f) {
        float gh = std::sin(gx * 0.5f) * 2.f + 3.f;
        sf::RectangleShape grass;
        grass.setSize({2.f, gh});
        grass.setPosition({gx, groundY - gh});
        grass.setFillColor(tc.grass);
        window.draw(grass);
    }

    if (currentTheme == Config::THEME_SPACE) {
        for (int i = 0; i < 30; i++) {
            float starX = static_cast<float>((i * 137) % cfg.screenWidth);
            float starY = static_cast<float>((i * 97) % (cfg.screenHeight - static_cast<int>(cfg.groundHeight)));
            sf::RectangleShape star;
            star.setSize({2.f, 2.f});
            star.setPosition({starX, starY});
            star.setFillColor(sf::Color(255, 255, 255, 200));
            window.draw(star);
        }
    }

    if (currentTheme == Config::THEME_LAVA) {
        sf::RectangleShape lavaGlow;
        lavaGlow.setSize({static_cast<float>(cfg.screenWidth), 20.f});
        lavaGlow.setPosition({0.f, groundY - 20.f});
        lavaGlow.setFillColor(sf::Color(255, 80, 10, 60));
        window.draw(lavaGlow);
    }

    if (currentTheme == Config::THEME_CAVE) {
        for (float cx = 0.f; cx < static_cast<float>(cfg.screenWidth); cx += 80.f) {
            float stalactiteH = 30.f + std::sin(cx * 0.1f) * 20.f;
            sf::RectangleShape stalactite;
            stalactite.setSize({15.f, stalactiteH});
            stalactite.setPosition({cx, 0.f});
            stalactite.setFillColor(sf::Color(55, 50, 45));
            window.draw(stalactite);
        }
    }

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
                             sf::Vector2f(static_cast<float>(cfg.screenWidth - 200.f), 10.f));
    window.draw(hsText);

    if (!gameStarted) {
        std::string countStr;
        int n = static_cast<int>(std::ceil(countdownTimer));
        countStr = (n >= 1) ? std::to_string(n) : "GO!";
        auto countText = makeText(font, countStr, 80,
            (n >= 1) ? sf::Color::White : sf::Color::Yellow,
            sf::Vector2f(0.f, static_cast<float>(cfg.screenHeight) / 2.f - 100.f));
        centerText(window, countText);
        window.draw(countText);
    }

    if (paused) {
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(cfg.screenWidth), static_cast<float>(cfg.screenHeight)));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        auto pauseTitle = makeText(font, "PAUSED", 60, sf::Color::White,
            sf::Vector2f(0.f, static_cast<float>(cfg.screenHeight) / 2.f - 100.f));
        centerText(window, pauseTitle);
        window.draw(pauseTitle);

        auto resumeText = makeText(font,
            (pauseSelected == 0 ? "> Resume" : "  Resume"), 30,
            (pauseSelected == 0 ? sf::Color::Yellow : sf::Color::White),
            sf::Vector2f(0.f, static_cast<float>(cfg.screenHeight) / 2.f));
        centerText(window, resumeText);
        window.draw(resumeText);

        auto quitText = makeText(font,
            (pauseSelected == 1 ? "> Quit" : "  Quit"), 30,
            (pauseSelected == 1 ? sf::Color::Yellow : sf::Color::White),
            sf::Vector2f(0.f, static_cast<float>(cfg.screenHeight) / 2.f + 50.f));
        centerText(window, quitText);
        window.draw(quitText);
    }

    window.setView(originalView);

  // Debug Overlay
    int currentScore = scoreManager->getScore();
    std::string zoneName = "INSANE";
    if (currentScore <= Config::EASY_ZONE.maxScore) zoneName = "EASY";
    else if (currentScore <= Config::NORMAL_ZONE.maxScore) zoneName = "NORMAL";
    else if (currentScore <= Config::HARD_ZONE.maxScore) zoneName = "HARD";

  auto debugText = makeText(font, "Zone: " + zoneName,
                               18, sf::Color::Yellow,
                               sf::Vector2f(10.f, static_cast<float>(cfg.screenHeight) - 30.f));
    window.draw(debugText);

    auto themeText = makeText(font, Config::THEME_NAMES[currentTheme],
                              16, sf::Color(255, 255, 255, 150),
                              sf::Vector2f(static_cast<float>(cfg.screenWidth) - 80.f, 40.f));
    window.draw(themeText);

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

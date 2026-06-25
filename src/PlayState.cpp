#include "PlayState.h"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include <algorithm>
#include <cmath>

PlayState::PlayState(sf::Sound jumpSnd, sf::Sound scoreSnd, sf::Sound deathSnd,
                     sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                     const sf::Font& fontRef,
                     float posX, float posY, float vel)
    : jumpSound(jumpSnd), scoreSound(scoreSnd), deathSound(deathSnd),
      bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
      font(&fontRef) {
    currentPipeSpeed = ConfigLoader::getFloat("pipe_speed", Config::PIPE_SPEED);
    currentSpawnInterval = ConfigLoader::getFloat("pipe_spawn_interval", Config::PIPE_SPAWN_INTERVAL);

    yDist = std::uniform_real_distribution<float>(Config::PIPE_MIN_Y, Config::PIPE_MAX_Y);
    gapDist = std::uniform_real_distribution<float>(Config::PIPE_GAP_RANGE * 0.6f, Config::PIPE_GAP_RANGE);
    typeDist = std::uniform_int_distribution<int>(0, 4);

    bird.load(Config::BIRD_PATH);
    restartPosX = posX;
    restartPosY = posY;
    restartVel = vel;

    pipePool = std::make_unique<ObjectPool<Pipe>>([&]() {
        return Pipe(0, 0, Config::GAP_HEIGHT, Config::PIPE_SPEED);
    });
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void PlayState::onEnter() {
    bird.reset();
    bird.load(Config::BIRD_PATH);
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

    deathSound.play();
    bird.setDying();

    sf::FloatRect birdBounds = bird.getBoundingBox();
    for (int i = 0; i < 20; ++i) {
        float vx = (static_cast<float>(typeDist(rng)) - 2.0f) * 200.0f;
        float vy = (static_cast<float>(typeDist(rng)) - 2.0f) * 200.0f;
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setPosition(birdBounds.position);
        p.velocity = {vx, vy};
        p.lifetime = 1.0f;
        activeParticles.push_back(idx);
    }

    shakeTimer = Config::SCREEN_SHAKE_DURATION;
    shakeIntensity = Config::SCREEN_SHAKE_INTENSITY;

    if (bgmLoaded && bgmMusic.getStatus() == sf::SoundSource::Status::Playing)
        bgmMusic.stop();
}

void PlayState::drawSky(sf::RenderWindow& window, float dt) {
    skyTimer += dt;
    float cyclePos = std::fmod(skyTimer, Config::SKY_CYCLE_INTERVAL) / Config::SKY_CYCLE_INTERVAL;
    int idx1 = static_cast<int>(cyclePos * Config::SKY_COLOR_COUNT) % Config::SKY_COLOR_COUNT;
    int idx2 = (idx1 + 1) % Config::SKY_COLOR_COUNT;
    float t = (cyclePos * Config::SKY_COLOR_COUNT) - idx1;

    auto lerp = [](float a, float b, float t) -> unsigned char {
        return static_cast<unsigned char>(a * (1 - t) + b * t);
    };

    sf::Color top(
        lerp(Config::SKY_TOP[idx1].r, Config::SKY_TOP[idx2].r, t),
        lerp(Config::SKY_TOP[idx1].g, Config::SKY_TOP[idx2].g, t),
        lerp(Config::SKY_TOP[idx1].b, Config::SKY_TOP[idx2].b, t)
    );
    sf::Color bot(
        lerp(Config::SKY_BOT[idx1].r, Config::SKY_BOT[idx2].r, t),
        lerp(Config::SKY_BOT[idx1].g, Config::SKY_BOT[idx2].g, t),
        lerp(Config::SKY_BOT[idx1].b, Config::SKY_BOT[idx2].b, t)
    );

    float halfH = static_cast<float>(Config::SCREEN_HEIGHT) / 2.f;
    sf::RectangleShape topRect;
    topRect.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), halfH));
    topRect.setFillColor(top);
    window.draw(topRect);

    sf::RectangleShape botRect;
    botRect.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), halfH));
    botRect.setPosition(sf::Vector2f(0.f, halfH));
    botRect.setFillColor(bot);
    window.draw(botRect);
}

void PlayState::drawGround(sf::RenderWindow& window, float dt) {
    float totalW = static_cast<float>(Config::SCREEN_WIDTH);
    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
    float scroll = std::fmod(groundScrollOffset, Config::GROUND_TILE_W);

    sf::RectangleShape base;
    base.setSize(sf::Vector2f(totalW, static_cast<float>(Config::GROUND_HEIGHT)));
    base.setPosition(sf::Vector2f(0.f, groundY));
    base.setFillColor(Config::GROUND_COLOR);
    window.draw(base);

    for (float x = -scroll - Config::GROUND_TILE_W; x < totalW + Config::GROUND_TILE_W; x += Config::GROUND_TILE_W) {
        int idx = static_cast<int>(std::floor((x + scroll) / Config::GROUND_TILE_W)) % Config::GROUND_TILE_COUNT;
        if (idx < 0) idx += Config::GROUND_TILE_COUNT;
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(static_cast<float>(Config::GROUND_TILE_W), 8.f));
        tile.setPosition(sf::Vector2f(x, groundY));
        tile.setFillColor(Config::GROUND_TOP_COLORS[idx]);
        window.draw(tile);
    }

    float grassY = groundY - 2.f;
    for (float x = -scroll; x < totalW; x += 12.f) {
        float h = std::sin(x * 0.5f) * 2.f + 3.f;
        sf::RectangleShape blade;
        blade.setSize(sf::Vector2f(2.f, h));
        blade.setPosition(sf::Vector2f(x, grassY - h));
        blade.setFillColor(sf::Color(80, 140, 60));
        window.draw(blade);
    }
}

void PlayState::update(float dt) {
    if (paused || quitToMenu) return;

    if (gameOverTriggered) {
        bird.update(dt);
        return;
    }

    if (!gameStarted) {
        countdownTimer -= dt;
        if (countdownTimer <= 0.0f) {
            gameStarted = true;
        }
        skyTimer += dt;
        groundScrollOffset += currentPipeSpeed * dt;
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

    for (auto& sf : scoreFloats) sf->update(dt);
    scoreFloats.erase(std::remove_if(scoreFloats.begin(), scoreFloats.end(),
        [](const std::shared_ptr<ScoreFloat>& s) { return !s->alive(); }), scoreFloats.end());

    skyTimer += dt;
    groundScrollOffset += currentPipeSpeed * dt;

    bird.update(dt);

    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
    if (bird.getBoundingBox().position.y < 0 || bird.getBoundingBox().position.y + bird.getBoundingBox().size.y > groundY) {
        triggerGameOver();
        return;
    }

    for (auto it = activePipes.begin(); it != activePipes.end(); ) {
        int idx = *it;
        Pipe& pipe = (*pipePool)[idx];
        pipe.update(dt);
        float currentGap = ConfigLoader::getFloat("gap_height", Config::GAP_HEIGHT);
        if (pipe.checkCollision(bird.getBoundingBox())) {
            triggerGameOver();
            return;
        }

        float birdX = bird.getX();
        if (!pipe.passed && pipe.getX() < birdX) {
            pipe.passed = true;
            score++;
            scoreSound.play();
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

    spawnTimer += dt;
    if (spawnTimer > currentSpawnInterval) {
        float randomY = yDist(rng);
        float randomGap = gapDist(rng);
        PipeType type = (typeDist(rng) == 0) ? PipeType::MOVING : PipeType::STATIC;
        int idx = pipePool->acquire();
        (*pipePool)[idx].reset(static_cast<float>(Config::SCREEN_WIDTH), randomY, randomGap, currentPipeSpeed, type);
        activePipes.push_back(idx);
        spawnTimer = 0.f;
    }

    for (auto it = activeParticles.begin(); it != activeParticles.end(); ) {
        int idx = *it;
        Particle& p = (*particlePool)[idx];
        p.update(dt);
        if (p.lifetime <= 0) {
            particlePool->release(idx);
            it = activeParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void PlayState::draw(sf::RenderWindow& window, const sf::Font& font) {
    window.pushGLStates();

    sf::View originalView = window.getView();
    sf::View shakeView = originalView;
    shakeView.setCenter(originalView.getCenter() + sf::Vector2f(shakeOffset.x, shakeOffset.y));
    window.setView(shakeView);

    drawSky(window, 0.f);

    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
    sf::RectangleShape bgLayer;
    bgLayer.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), 150.f));
    bgLayer.setPosition(sf::Vector2f(bgOffset, groundY - 150.f));
    bgLayer.setFillColor(sf::Color(100, 150, 100));
    window.draw(bgLayer);

    bgLayer.setPosition(sf::Vector2f(bgOffset + static_cast<float>(Config::SCREEN_WIDTH), groundY - 150.f));
    window.draw(bgLayer);

    for (int idx : activePipes) (*pipePool)[idx].draw(window);

    bird.draw(window);

    for (const auto& sf : scoreFloats) sf->draw(window);

    auto scoreText = makeText(font, "Score: " + std::to_string(score),
                              30, Config::TEXT_COLOR,
                              sf::Vector2f(10.f, 10.f));
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
    window.popGLStates();
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

    for (int idx : activePipes) {
        snap.pipes.push_back((*pipePool)[idx]);
    }
    for (int idx : activeParticles) {
        snap.particles.push_back((*particlePool)[idx]);
    }

    snap.scoreFloats = scoreFloats;
    return snap;
}

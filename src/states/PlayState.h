#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <random>
#include "GameState.h"
#include "entities/Bird.hpp"
#include "entities/Pipe.hpp"
#include "entities/PowerUp.hpp"
#include "scoring/ScoreFloat.h"
#include "entities/BirdState.h"
#include "core/ObjectPool.h"
#include "systems/VisualEffectManager.hpp"
#include "audio/SoundManager.hpp"
#include "scoring/ScoreManager.hpp"

struct PlayStateSnapshot {
    BirdState birdState;
    int score;
    int difficulty;
    std::vector<Pipe> pipes;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    float currentPipeSpeed;
    float currentSpawnInterval;
    float shakeTimer;
    float shakeIntensity;
    float spawnTimer;
    std::vector<Particle> particles;
};

class PlayState : public GameState {
public:
     PlayState(sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                      const sf::Font& fontRef, const std::string& assetDir,
                      float posX = Config::BIRD_START_X, float posY = Config::BIRD_START_Y,
                      float vel = 0.0f, int difficulty = 1);

    void update(float dt) override;
    void draw(sf::RenderWindow& window, const sf::Font& font) override;
    void handleKeyPress(sf::Keyboard::Key key) override;
    void onEnter() override;

    StateAction nextAction() const override {
        if (quitToMenu) return StateAction::ReturnToMenu;
        if (gameOverTriggered) return StateAction::GameOver;
        return StateAction::None;
    }
    int selectedDifficulty() const override { return scoreManager->getDifficulty(); }
    PlayStateSnapshot takeSnapshot() const;

private:
    void triggerGameOver();

    std::unique_ptr<SoundManager> soundManager;
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;

    Bird bird;
    std::vector<int> activePipes;
    std::vector<int> activePowerUps;
    float slowMoFactor = 1.0f;

    std::unique_ptr<ObjectPool<Pipe>> pipePool;
    std::unique_ptr<ObjectPool<PowerUp>> powerUpPool;
    std::unique_ptr<VisualEffectManager> visualEffects;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> yDist;
    std::uniform_real_distribution<float> gapDist;
    std::uniform_int_distribution<int> typeDist;

    int difficulty = 1;
    std::unique_ptr<ScoreManager> scoreManager;
    bool paused = false;
    int pauseSelected = 0;
    bool quitToMenu = false;
    bool gameStarted = false;
    float countdownTimer = 3.0f;
    PlayStateSnapshot gameOverSnapshot;
    float restartPosX = Config::BIRD_START_X;
    float restartPosY = Config::BIRD_START_Y;
    float restartVel = 0.0f;

    float shakeTimer = 0.f;
    float shakeIntensity = 0.f;
    float spawnTimer = 0.f;
    sf::Vector2f shakeOffset = {0.f, 0.f};
    float currentPipeSpeed;
    float dustSpawnTimer = 0.f;
    float currentSpawnInterval;
    bool gameOverTriggered = false;

public:
    std::unique_ptr<ScoreManager>& getScoreManager() { return scoreManager; }
};

#endif

#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <random>
#include "GameState.h"
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Particle.hpp"
#include "ScoreFloat.h"
#include "BirdState.h"


struct PlayStateSnapshot {
    BirdState birdState;
    int score;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
};

class PlayState : public GameState {
public:
    PlayState(sf::Sound jumpSnd, sf::Sound scoreSnd, sf::Sound deathSnd,
              sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
              const sf::Font& fontRef,
              float posX = Config::BIRD_START_X, float posY = Config::BIRD_START_Y,
              float vel = 0.0f);

    void update(float dt) override;
    void draw(sf::RenderWindow& window, const sf::Font& font) override;
    void handleKeyPress(sf::Keyboard::Key key) override;
    void onEnter() override;

    void getSnapshot(PlayStateSnapshot& out) const;
    int nextAction() const { return nextActionCode; }
    void setRestartState(float posX, float posY, float vel);

private:
    void triggerGameOver();
    void drawGround(sf::RenderWindow& window, float dt);
    void drawSky(sf::RenderWindow& window, float dt);

    sf::Sound jumpSound;
    sf::Sound scoreSound;
    sf::Sound deathSound;
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;

    Bird bird;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;

    float groundScrollOffset = 0.f;
    float bgOffset = 0.f;
    float spawnTimer = 0.f;
    float currentPipeSpeed = Config::PIPE_SPEED;
    float currentSpawnInterval = Config::PIPE_SPAWN_INTERVAL;
    float shakeTimer = 0.f;
    float shakeIntensity = 0.f;
    sf::Vector2f shakeOffset = {0.f, 0.f};
    int score = 0;
    float skyTimer = 0.f;

    std::default_random_engine rng;
    std::uniform_real_distribution<float> yDist;
    std::uniform_real_distribution<float> gapDist;
    std::uniform_int_distribution<int> typeDist;

    int nextActionCode = 0;
    float restartPosX = Config::BIRD_START_X;
    float restartPosY = Config::BIRD_START_Y;
    float restartVel = 0.0f;
};

#endif // PLAYSTATE_H

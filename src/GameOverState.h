#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.h"
#include "Pipe.hpp"
#include "Particle.hpp"
#include "ScoreFloat.h"
#include "BirdState.h"

class GameOverState : public GameState {
public:
    GameOverState(BirdState birdState, std::vector<Pipe> pipes,
                  std::vector<Particle> particles,
                  std::vector<std::shared_ptr<ScoreFloat>> scoreFloats,
                  int score, int& highScoreRef);

    void update(float dt) override;
    void draw(sf::RenderWindow& window, const sf::Font& font) override;
    void handleKeyPress(sf::Keyboard::Key key) override;
    int nextAction() const { return nextActionCode; }
    BirdState getRestartState() const;

private:
    BirdState birdState;
    BirdState restartBirdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int score;
    int& highScore;
    float overlayAlpha = 0.f;
    int nextActionCode = 0;
};

#endif // GAMEOVERSTATE_H

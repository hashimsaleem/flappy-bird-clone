#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameState.h"
#include "entities/Pipe.hpp"
#include "visual/Particle.hpp"
#include "scoring/ScoreFloat.h"
#include "entities/BirdState.h"

class GameOverState : public GameState {
public:
    GameOverState(BirdState birdState, std::vector<Pipe> pipes,
                  std::vector<Particle> particles,
                  std::vector<std::shared_ptr<ScoreFloat>> scoreFloats,
                  int score, int& highScoreRef, int difficulty = 1);

    void update(float dt) override;
    void draw(sf::RenderWindow& window, const sf::Font& font) override;
    void handleKeyPress(sf::Keyboard::Key key) override;
    StateAction nextAction() const override { return nextActionCode; }
    BirdState getRestartBirdState() const override;
    BirdState getBirdState() const { return birdState; }
    int selectedDifficulty() const override { return difficulty; }

private:
    BirdState birdState;
    BirdState restartBirdState;
    std::vector<Pipe> pipes;
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
    int score;
    int& highScore;
    int difficulty;
    float overlayAlpha = 0.f;
    StateAction nextActionCode = StateAction::None;
    bool highScoreSaved = false;
};

#endif

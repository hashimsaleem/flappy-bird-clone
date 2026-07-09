#ifndef SCORE_MANAGER_HPP
#define SCORE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "ScoreFloat.h"
#include "core/ConfigValues.hpp"

class ScoreManager {
public:
    ScoreManager(const ConfigValues& cfg, const sf::Font& font, int initialDifficulty);

    void addScore();
    void updateBounceTimer(float dt);
    void resetBounceTimer() { scoreBounceTimer = 0.f; }
    
    int getScore() const;
    int getDifficulty() const;
    float getCurrentPipeSpeed() const;
    float getCurrentSpawnInterval() const;

    // Helpers for PlayState
    const std::vector<std::shared_ptr<ScoreFloat>>& getScoreFloats() const;
    void removeExpiredScoreFloats();
    void pushScoreFloat(const sf::Font& font, sf::Vector2f position);
    void setCurrentPipeSpeed(float speed);
    void setCurrentSpawnInterval(float interval);

    float getScoreScale() const;
    void setScoreBounceTimer(float timer);
    float getScoreBounceTimer() const;

private:
    int score = 0;
    int difficulty = 1;
    float currentPipeSpeed;
    float currentSpawnInterval;
    float scoreBounceTimer = 0.f;
    float scoreScale = 1.f;

    const sf::Font* font;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
};

#endif

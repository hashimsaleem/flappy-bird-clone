#include "ScoreManager.hpp"
#include <algorithm>
#include <cmath>

ScoreManager::ScoreManager(const ConfigValues& cfg, const sf::Font& font, int initialDifficulty)
    : font(&font), difficulty(initialDifficulty) {
    currentPipeSpeed = cfg.pipeSpeed;
    currentSpawnInterval = cfg.pipeSpawnInterval;

    if (difficulty == 0) {
        currentPipeSpeed *= 0.7f;
        currentSpawnInterval *= 1.5f;
    } else if (difficulty == 2) {
        currentPipeSpeed *= 1.3f;
        currentSpawnInterval *= 0.7f;
    }
}

void ScoreManager::update(float dt) {
    for (auto& sf : scoreFloats) sf->update(dt);
    removeExpiredScoreFloats();

    if (scoreBounceTimer > 0.f) {
        scoreBounceTimer -= dt;
        scoreScale = 1.f + 0.4f * std::min(scoreBounceTimer / 0.3f, 1.f);
        if (scoreBounceTimer <= 0.f) scoreScale = 1.f;
    }
}

void ScoreManager::addScore() {
    score++;
    scoreBounceTimer = 0.3f;
}

void ScoreManager::removeExpiredScoreFloats() {
    scoreFloats.erase(std::remove_if(scoreFloats.begin(), scoreFloats.end(),
        [](const std::shared_ptr<ScoreFloat>& s) { return !s->alive(); }), scoreFloats.end());
}

void ScoreManager::pushScoreFloat(const sf::Font& font, sf::Vector2f position) {
    scoreFloats.push_back(std::make_shared<ScoreFloat>(font, position));
}

int ScoreManager::getScore() const { return score; }
int ScoreManager::getDifficulty() const { return difficulty; }
float ScoreManager::getCurrentPipeSpeed() const { return currentPipeSpeed; }
float ScoreManager::getCurrentSpawnInterval() const { return currentSpawnInterval; }
void ScoreManager::setCurrentPipeSpeed(float speed) { currentPipeSpeed = speed; }
void ScoreManager::setCurrentSpawnInterval(float interval) { currentSpawnInterval = interval; }

float ScoreManager::getScoreScale() const { return scoreScale; }
void ScoreManager::setScoreScale(float scale) { scoreScale = scale; }
void ScoreManager::setScoreBounceTimer(float timer) { scoreBounceTimer = timer; }
float ScoreManager::getScoreBounceTimer() const { return scoreBounceTimer; }

std::vector<std::shared_ptr<ScoreFloat>> ScoreManager::getScoreFloats() const {
    return scoreFloats;
}

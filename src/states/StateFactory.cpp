#include "StateFactory.h"
#include "core/ConfigValues.hpp"

std::unique_ptr<GameState> StateFactory::createMenuState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                              const sf::Font& fontRef, const std::string& assetsBase) {
    return std::make_unique<MenuState>(cfg, bgmMusic, bgmLoaded, highScoreRef, fontRef, assetsBase);
}

std::unique_ptr<GameState> StateFactory::createPlayState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                              const sf::Font& fontRef,
                                                              float posX, float posY, float vel, int difficulty,
                                                              const std::string& assetsBase) {
    return std::make_unique<PlayState>(cfg, bgmMusic, bgmLoaded, highScoreRef, fontRef, posX, posY, vel, difficulty, assetsBase);
}


std::unique_ptr<GameState> StateFactory::createGameOverState(const ConfigValues& cfg, PlayStateSnapshot snap, int score, int& highScoreRef) {
    return std::make_unique<GameOverState>(cfg, std::move(snap.birdState), std::move(snap.pipes),
                                           std::move(snap.particles),
                                           std::move(snap.scoreFloats), score, highScoreRef, snap.difficulty);
}

std::unique_ptr<GameState> StateFactory::createHighScoreScreenState() {
    return std::make_unique<HighScoreScreenState>();
}

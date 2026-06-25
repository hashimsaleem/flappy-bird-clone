#include "StateFactory.h"
#include "Config.hpp"

std::unique_ptr<GameState> StateFactory::createMenuState(sf::Sound& jumpSnd, sf::Sound& scoreSnd, sf::Sound& deathSnd,
                                                            sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                            const sf::Font& fontRef) {
    return std::make_unique<MenuState>(jumpSnd, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScoreRef, fontRef);
}

std::unique_ptr<GameState> StateFactory::createPlayState(sf::Sound& jumpSnd, sf::Sound& scoreSnd, sf::Sound& deathSnd,
                                                            sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                            const sf::Font& fontRef,
                                                            float posX, float posY, float vel) {
    return std::make_unique<PlayState>(jumpSnd, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScoreRef, fontRef, posX, posY, vel);
}

std::unique_ptr<GameState> StateFactory::createGameOverState(PlayStateSnapshot snap, int score, int& highScoreRef) {
    return std::make_unique<GameOverState>(std::move(snap.birdState), std::move(snap.pipes),
                                           std::move(snap.particles),
                                           std::move(snap.scoreFloats), score, highScoreRef);
}

std::unique_ptr<GameState> StateFactory::createHighScoreScreenState() {
    return std::make_unique<HighScoreScreenState>();
}

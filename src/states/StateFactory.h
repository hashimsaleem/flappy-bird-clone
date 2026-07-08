#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "core/ConfigValues.hpp"
#include "GameState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "GameOverState.h"
#include "HighScoreScreenState.h"

class StateFactory {
public:
    static std::unique_ptr<GameState> createMenuState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                        const sf::Font& fontRef);
    static std::unique_ptr<GameState> createPlayState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                                                        const sf::Font& fontRef,
                                                        float posX = 50.0f, float posY = 300.0f,
                                                        float vel = 0.0f, int difficulty = 1);

    static std::unique_ptr<GameState> createGameOverState(PlayStateSnapshot snap, int score, int& highScoreRef);
    static std::unique_ptr<GameState> createHighScoreScreenState();
};

#endif

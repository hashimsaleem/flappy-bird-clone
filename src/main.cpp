#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include "ResourceManager.hpp"
#include "HighScore.hpp"
#include "GameState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "GameOverState.h"
#include "HighScoreScreenState.h"

// Uncomment to enable debug console output
// #define DEBUG

int main() {
    // Load config
    std::string configPath = std::string(Config::ASSETS_DIR) + Config::CONFIG_PATH;
    ConfigLoader::load(configPath);

    // Apply config overrides
    unsigned int screenW = Config::SCREEN_WIDTH;
    unsigned int screenH = Config::SCREEN_HEIGHT;
    {
        int cw = ConfigLoader::getInt("screen_width", 0);
        int ch = ConfigLoader::getInt("screen_height", 0);
        if (cw > 0 && ch > 0) { screenW = static_cast<unsigned int>(cw); screenH = static_cast<unsigned int>(ch); }
    }

    sf::RenderWindow window(sf::VideoMode({screenW, screenH}), "Flappy Clone SFML");
    window.setFramerateLimit(Config::TARGET_FPS);

    const sf::Font& font = ResourceManager::getFont(Config::FONT_PATH, 30);

    // Sounds
    sf::Sound jumpSound = ResourceManager::getSound(Config::JUMP_SND);
    sf::Sound scoreSound = ResourceManager::getSound(Config::SCORE_SND);
    sf::Sound deathSound = ResourceManager::getSound(Config::DEATH_SND);

    // Background music
    sf::Music bgmMusic;
    bool bgmLoaded = bgmMusic.openFromFile(Config::BG_MUSIC);
    if (!bgmLoaded) {
        bgmLoaded = bgmMusic.openFromFile("assets/bgm.ogg");
    }
#ifdef DEBUG
    std::cerr << "[bgm] bgmLoaded=" << bgmLoaded << "\n";
#endif
    if (bgmLoaded) {
        bgmMusic.setLooping(true);
        bgmMusic.setVolume(40.f);
        bgmMusic.play();
    }

    int highScore = HighScore::load();

    // State machine
    std::unique_ptr<GameState> state = nullptr;

    auto createMenuState = [&]() {
        state = std::make_unique<MenuState>();
    };
    auto createPlayState = [&]() {
        state = std::make_unique<PlayState>(jumpSound, scoreSound, deathSound, bgmMusic, bgmLoaded, highScore, font);
        static_cast<PlayState*>(state.get())->onEnter();
    };
    auto createGameOverState = [&](PlayStateSnapshot snap, int score) {
        state = std::make_unique<GameOverState>(std::move(snap.birdState), std::move(snap.pipes),
                                                 std::move(snap.particles),
                                                 std::move(snap.scoreFloats), score, highScore);
    };
    auto createHighScoreScreen = [&]() {
        state = std::make_unique<HighScoreScreenState>();
    };

    createMenuState();

    sf::Clock gameClock;

    while (window.isOpen()) {
        sf::Time elapsed = gameClock.restart();
        float dt = elapsed.asSeconds();

        // Clamp dt to avoid physics explosions on tab loss
        if (dt > 0.1f) dt = 0.1f;

        // --- Input ---
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (auto* ms = dynamic_cast<MenuState*>(state.get())) {
                    ms->handleKeyPress(keyPressed->code);
                    if (ms->selectedOption >= 0) {
                        if (ms->selectedOption == 0) { createPlayState(); }
                        else if (ms->selectedOption == 1) { createHighScoreScreen(); }
                        else { window.close(); }
                        ms->selectedOption = -1;
                    }
                } else if (auto* hs = dynamic_cast<HighScoreScreenState*>(state.get())) {
                    hs->handleKeyPress(keyPressed->code);
                    if (hs->selectedOption >= 0) { createMenuState(); hs->selectedOption = -1; }
                } else {
                    state->handleKeyPress(keyPressed->code);
                }
            }
        }

        // --- Update ---
        state->update(dt);

        // Check for state transitions
        if (auto* ps = dynamic_cast<PlayState*>(state.get())) {
            int action = ps->nextAction();
            if (action == 1) {
                PlayStateSnapshot snap;
                ps->getSnapshot(snap);
                int currentScore = snap.score;
                createGameOverState(std::move(snap), currentScore);
            } else if (action == 3) {
                window.close();
            }
        }
        if (auto* gs = dynamic_cast<GameOverState*>(state.get())) {
            if (gs->nextAction() == 1) {
                BirdState rs = gs->getRestartState();
                state = std::make_unique<PlayState>(jumpSound, scoreSound, deathSound, bgmMusic, bgmLoaded, highScore, font,
                                                      rs.posX, rs.posY, rs.velocityY);
                static_cast<PlayState*>(state.get())->onEnter();
            } else if (gs->nextAction() == 2) {
                createMenuState();
            }
        }

        // --- Draw ---
        window.clear(Config::SKY_COLOR);
        state->draw(window, font);
        window.display();
    }

    return 0;
}

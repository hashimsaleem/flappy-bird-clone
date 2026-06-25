#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include "ResourceManager.hpp"
#include "HighScore.hpp"
#include "GameState.h"
#include "PlayState.h"
#include "StateFactory.h"

static std::string getExeDir() {
    char buf[4096];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        return std::filesystem::path(buf).parent_path().string() + "/";
    }
    return "./";
}

int main() {
    std::string exeDir = getExeDir();

    ConfigLoader::load(exeDir + Config::ASSETS_DIR + Config::CONFIG_PATH);

    unsigned int screenW = Config::SCREEN_WIDTH;
    unsigned int screenH = Config::SCREEN_HEIGHT;
    {
        int cw = ConfigLoader::getInt("screen_width", 0);
        int ch = ConfigLoader::getInt("screen_height", 0);
        if (cw > 0 && ch > 0) { screenW = static_cast<unsigned int>(cw); screenH = static_cast<unsigned int>(ch); }
    }

    sf::RenderWindow window(sf::VideoMode({screenW, screenH}), "Flappy Clone SFML");
    window.setFramerateLimit(Config::TARGET_FPS);

    ResourceManager resMgr;
    std::string fontPath = exeDir + Config::FONT_PATH;
    const sf::Font& font = resMgr.getFont(fontPath, 30);

    sf::Sound& jumpSound = resMgr.getSound(exeDir + Config::JUMP_SND);
    sf::Sound& scoreSnd = resMgr.getSound(exeDir + Config::SCORE_SND);
    sf::Sound& deathSnd = resMgr.getSound(exeDir + Config::DEATH_SND);

    sf::Music bgmMusic;
    bool bgmLoaded = bgmMusic.openFromFile(exeDir + Config::BG_MUSIC);
    if (bgmLoaded) {
        bgmMusic.setLooping(true);
        bgmMusic.setVolume(40.f);
        bgmMusic.play();
    }

    int highScore = HighScore::load();

    std::unique_ptr<GameState> state = StateFactory::createMenuState(jumpSound, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font);
    state->onEnter();

    sf::Clock gameClock;
    float accumulator = 0.f;
    const float fixedDt = 1.f / static_cast<float>(Config::TARGET_FPS);

    while (window.isOpen()) {
        float frameTime = gameClock.restart().asSeconds();
        if (frameTime > 0.1f) frameTime = 0.1f;
        accumulator += frameTime;

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                state->handleKeyPress(keyPressed->code);
            }
        }

        while (accumulator >= fixedDt) {
            state->update(fixedDt);
            accumulator -= fixedDt;
        }

        auto action = state->nextAction();
        if (action != StateAction::None) {
            std::unique_ptr<GameState> next;
            switch (action) {
            case StateAction::PlayGame: {
                BirdState rs = state->getRestartBirdState();
                next = StateFactory::createPlayState(jumpSound, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font,
                                                      rs.posX, rs.posY, rs.velocityY);
                break;
            }
            case StateAction::ShowHighScore:
                next = StateFactory::createHighScoreScreenState();
                break;
            case StateAction::GameOver: {
                auto& ps = static_cast<PlayState&>(*state);
                PlayStateSnapshot snap = ps.takeSnapshot();
                next = StateFactory::createGameOverState(std::move(snap), snap.score, highScore);
                break;
            }
            case StateAction::ReturnToMenu:
                next = StateFactory::createMenuState(jumpSound, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font);
                break;
            case StateAction::Exit:
                window.close();
                break;
            default:
                break;
            }
            if (next) {
                next->onEnter();
                state = std::move(next);
            }
        }

        window.clear(Config::SKY_COLOR);
        state->draw(window, font);
        window.display();
    }

    return 0;
}

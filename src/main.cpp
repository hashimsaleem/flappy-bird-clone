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

    ResourceManager& resMgr = ResourceManager::getInstance();
    std::string fontPath = exeDir + Config::FONT_PATH;
    const sf::Font& font = resMgr.getFont(fontPath, 30);

    sf::Sound& jumpSoundRef = resMgr.getSound(exeDir + Config::JUMP_SND);
    sf::Sound& scoreSndRef = resMgr.getSound(exeDir + Config::SCORE_SND);
    sf::Sound& deathSndRef = resMgr.getSound(exeDir + Config::DEATH_SND);
    sf::Sound* jumpSound = &jumpSoundRef;
    sf::Sound* scoreSnd = &scoreSndRef;
    sf::Sound* deathSnd = &deathSndRef;

    sf::Music bgmMusic;
    bool bgmLoaded = bgmMusic.openFromFile(exeDir + Config::BG_MUSIC);
    if (bgmLoaded) {
        bgmMusic.setLooping(true);
        bgmMusic.setVolume(40.f);
        bgmMusic.play();
    }

    HighScore::setPath(exeDir + "highscore.dat");
    int highScore = HighScore::load();

    std::unique_ptr<GameState> state = StateFactory::createMenuState(jumpSound, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font);
    state->onEnter();

    sf::Clock gameClock;
    sf::Clock fpsClock;
    float accumulator = 0.f;
    const float fixedDt = 1.f / static_cast<float>(Config::TARGET_FPS);

    bool fullscreen = false;
    bool showFps = false;
    int fps = 0;
    int frameCount = 0;
    float fadeAlpha = 0.f;

    while (window.isOpen()) {
        float frameTime = gameClock.restart().asSeconds();
        if (frameTime > 0.1f) frameTime = 0.1f;
        accumulator += frameTime;

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::F1) {
                    showFps = !showFps;
                } else if (keyPressed->code == sf::Keyboard::Key::F11) {
                    fullscreen = !fullscreen;
                    window.create(
                        fullscreen ? sf::VideoMode::getDesktopMode() : sf::VideoMode({screenW, screenH}),
                        "Flappy Clone SFML",
                        fullscreen ? sf::State::Fullscreen : sf::State::Windowed
                    );
                    window.setFramerateLimit(Config::TARGET_FPS);
                } else {
                    state->handleKeyPress(keyPressed->code);
                }
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
                int diff = state->selectedDifficulty();
                next = StateFactory::createPlayState(jumpSound, scoreSnd, deathSnd, bgmMusic, bgmLoaded, highScore, font, exeDir,
                                                       rs.posX, rs.posY, rs.velocityY, diff);
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
                fadeAlpha = 255.f;
            }
        }

        window.clear(Config::SKY_COLOR);
        state->draw(window, font);

        if (showFps) {
            auto fpsText = GameState::makeText(font, "FPS: " + std::to_string(fps), 18,
                sf::Color(200, 200, 200),
                sf::Vector2f(static_cast<float>(screenW) - 90.f, 5.f));
            window.draw(fpsText);
        }
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = frameCount;
            frameCount = 0;
            fpsClock.restart();
        }

        if (fadeAlpha > 0.f) {
            fadeAlpha = std::max(0.f, fadeAlpha - 600.f * fixedDt);
            sf::RectangleShape fadeRect(sf::Vector2f(static_cast<float>(screenW), static_cast<float>(screenH)));
            fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(fadeAlpha)));
            window.draw(fadeRect);
        }

        window.display();
    }

    return 0;
}

#include "SettingsState.h"
#include <algorithm>

SettingsState::SettingsState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
                             const sf::Font& fontRef, const std::string& assetsBase)
    : cfg(cfg),
      soundManager(std::make_unique<SoundManager>(bgmMusic, bgmLoaded)),
      bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
      font(&fontRef), assetsBase(assetsBase) {
}

void SettingsState::update(float dt) {
    soundManager->update(dt);
    timer += dt;
    hoverAlpha = std::min(1.f, hoverAlpha + dt * 5.f);
}

void SettingsState::draw(sf::RenderWindow& window, const sf::Font& fontRef) {
    (void)fontRef;
    float W = static_cast<float>(cfg.screenWidth);
    float H = static_cast<float>(cfg.screenHeight);
    float groundY = static_cast<float>(cfg.screenHeight - cfg.groundHeight);

    sf::RectangleShape overlay;
    overlay.setSize({W, H});
    overlay.setFillColor(sf::Color(10, 10, 20, 220));
    window.draw(overlay);

    auto titleText = makeText(*font, "SETTINGS", 48, sf::Color(255, 255, 255),
        sf::Vector2f(W / 2.f - 100.f, 40.f));
    window.draw(titleText);

    static const char* labels[] = {
        "Music Volume", "SFX Volume", "Difficulty", "Target FPS", "Particle Quality", "Fullscreen", "Back to Menu"
    };
    int optionCount = 7;
    float startY = 120.f;
    float rowH = 55.f;

    for (int i = 0; i < optionCount; i++) {
        float y = startY + static_cast<float>(i) * rowH;
        float pillW = 500.f;
        float pillH = 40.f;
        float pillX = W / 2.f - pillW / 2.f;

        sf::RectangleShape pill({pillW, pillH});
        pill.setPosition({pillX, y});
        if (i == selectedOption) {
            pill.setFillColor(sf::Color(102, 126, 234, 180 + static_cast<int>(hoverAlpha * 75)));
            pill.setOutlineColor(sf::Color(255, 255, 255, 150));
            pill.setOutlineThickness(2.f);
        } else {
            pill.setFillColor(sf::Color(255, 255, 255, 40));
            pill.setOutlineColor(sf::Color(255, 255, 255, 60));
            pill.setOutlineThickness(1.f);
        }
        window.draw(pill);

        std::string valueStr;
        if (i == 0) valueStr = std::to_string(static_cast<int>(audioVolume)) + "%";
        else if (i == 1) valueStr = std::to_string(static_cast<int>(sfxVolume)) + "%";
        else if (i == 2) valueStr = (difficulty == 0 ? "Easy" : difficulty == 1 ? "Medium" : "Hard");
        else if (i == 3) valueStr = std::to_string(targetFPS);
        else if (i == 4) valueStr = (particleQuality == 0 ? "Low" : particleQuality == 1 ? "Medium" : "High");
        else if (i == 5) valueStr = fullscreen ? "Yes" : "No";
        else if (i == 6) valueStr = "";

        if (!valueStr.empty()) {
            auto valText = makeText(*font, valueStr, 16, sf::Color(200, 200, 200),
                sf::Vector2f(pillX + pillW - 80.f, y + 13.f));
            window.draw(valText);
        }

        auto label = makeText(*font, labels[i], 16, sf::Color::White,
            sf::Vector2f(pillX + 15.f, y + 13.f));
        window.draw(label);
    }
}

void SettingsState::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Escape) {
        nextActionCode = StateAction::ReturnToMenu;
        return;
    }
    if (key == sf::Keyboard::Key::Up) {
        selectedOption = (selectedOption - 1 + 7) % 7;
    } else if (key == sf::Keyboard::Key::Down) {
        selectedOption = (selectedOption + 1) % 7;
    } else if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
        if (selectedOption == 0) {
            audioVolume = std::min(100.f, audioVolume + 10.f);
            bgmMusic.setVolume(audioVolume);
            soundManager->setBGMVolume(audioVolume);
        } else if (selectedOption == 1) {
            sfxVolume = std::min(100.f, sfxVolume + 10.f);
        } else if (selectedOption == 2) {
            difficulty = (difficulty + 1) % 3;
        } else if (selectedOption == 3) {
            targetFPS = (targetFPS == 60) ? 120 : 60;
        } else if (selectedOption == 4) {
            particleQuality = (particleQuality + 1) % 3;
        } else if (selectedOption == 5) {
            fullscreen = !fullscreen;
        } else if (selectedOption == 6) {
            nextActionCode = StateAction::ReturnToMenu;
        }
    } else if (key == sf::Keyboard::Key::Left) {
        if (selectedOption == 0) {
            audioVolume = std::max(0.f, audioVolume - 10.f);
            bgmMusic.setVolume(audioVolume);
            soundManager->setBGMVolume(audioVolume);
        } else if (selectedOption == 1) {
            sfxVolume = std::max(0.f, sfxVolume - 10.f);
        } else if (selectedOption == 2) {
            difficulty = (difficulty - 1 + 3) % 3;
        } else if (selectedOption == 3) {
            targetFPS = (targetFPS == 120) ? 60 : 120;
        } else if (selectedOption == 4) {
            particleQuality = (particleQuality - 1 + 3) % 3;
        } else if (selectedOption == 5) {
            fullscreen = !fullscreen;
        }
    } else if (key == sf::Keyboard::Key::Right) {
        if (selectedOption == 0) {
            audioVolume = std::min(100.f, audioVolume + 10.f);
            bgmMusic.setVolume(audioVolume);
            soundManager->setBGMVolume(audioVolume);
        } else if (selectedOption == 1) {
            sfxVolume = std::min(100.f, sfxVolume + 10.f);
        } else if (selectedOption == 2) {
            difficulty = (difficulty + 1) % 3;
        } else if (selectedOption == 3) {
            targetFPS = (targetFPS == 60) ? 120 : 60;
        } else if (selectedOption == 4) {
            particleQuality = (particleQuality + 1) % 3;
        } else if (selectedOption == 5) {
            fullscreen = !fullscreen;
        }
    }
}

void SettingsState::handleMouseClick(sf::Vector2f pos) {
    float W = static_cast<float>(cfg.screenWidth);
    float H = static_cast<float>(cfg.screenHeight);
    (void)H;
    float startY = 120.f;
    float rowH = 55.f;
    float pillW = 500.f;
    float pillH = 40.f;

    for (int i = 0; i < 7; i++) {
        float y = startY + static_cast<float>(i) * rowH;
        float pillX = W / 2.f - pillW / 2.f;
        if (pos.x >= pillX && pos.x <= pillX + pillW &&
            pos.y >= y && pos.y <= y + pillH) {
            selectedOption = i;
            handleKeyPress(sf::Keyboard::Key::Enter);
            return;
        }
    }
}

void SettingsState::onEnter() {
    soundManager->setBGMVolume(audioVolume);
}

#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>
#include "GameState.h"
#include "core/ConfigValues.hpp"
#include "core/Config.hpp"
#include "audio/SoundManager.hpp"
#include "visual/CloudSystem.hpp"
#include "visual/EnvironmentRenderer.hpp"


class MenuState : public GameState {
public:
    MenuState(const ConfigValues& cfg, sf::Music& bgmMusic, bool bgmLoaded, int& highScoreRef,
               const sf::Font& fontRef)
        : cfg(cfg),
          soundManager(std::make_unique<SoundManager>(bgmMusic, bgmLoaded)),
          bgmMusic(bgmMusic), bgmLoaded(bgmLoaded), highScore(highScoreRef),
          font(&fontRef),
          cloudSystem(std::make_unique<CloudSystem>(cfg)),
          environment(std::make_unique<EnvironmentRenderer>(cfg)) {
        volume = bgmMusic.getVolume();
    }

    void update(float dt) override {
        skyTimer += dt;
        bobTimer += dt;
        birdBobTimer += dt;
        birdFlapTimer += dt;
        buttonPulseTimer += dt;
        birdArcTimer += dt;
        cloudSystem->update(dt);
        if (selectedOption >= 0) {
            nextActionCode = StateAction::PlayGame;
        }
    }

    void draw(sf::RenderWindow& window, const sf::Font& fontRef) override {
        (void)fontRef;
        float W = static_cast<float>(cfg.screenWidth);
        float H = static_cast<float>(cfg.screenHeight);
        float groundY = static_cast<float>(cfg.screenHeight - cfg.groundHeight);

        // 1. Gradient sky background
        drawGradientSky(window, W, H);

        // 2. Sun glow
        drawSunGlow(window, W, H);

        // 3. Parallax clouds
        cloudSystem->draw(window);

        // 4. Background hills
        drawHills(window, W, H, groundY);

        // 5. Ground
        drawGround(window, W, H, groundY);

        // 6. Title with glow
        float titleY = H * 0.18f + std::sin(bobTimer * 1.5f) * 6.f;
        drawTitleGlow(window, W, titleY);
        auto titleText = makeText(fontRef, "FLAPPY BIRD", 56, Config::TEXT_COLOR,
            {W / 2.f - 130.f, titleY - 20.f});
        window.draw(titleText);

        // 7. Animated bird
        drawAnimatedBird(window, W);

        // 8. High score pill
        float hsY = H * 0.52f;
        drawHighScorePill(window, W, hsY);

        // 9. Play button
        float btnY = H * 0.60f;
        drawPlayButton(window, W, btnY);

        // 10. Difficulty pills
        float diffY = H * 0.73f;
        drawDifficultyPills(window, W, diffY);

        // 11. Volume bar
        float volY = H * 0.85f;
        drawVolumeBar(window, W, volY);
    }

    void handleKeyPress(sf::Keyboard::Key key) override {
        if (key == sf::Keyboard::Key::Num1) {
            selectedOption = 0;
            nextActionCode = StateAction::PlayGame;
        } else if (key == sf::Keyboard::Key::Num2) {
            selectedOption = 1;
            nextActionCode = StateAction::ShowHighScore;
        } else if (key == sf::Keyboard::Key::Num3) {
            difficulty = 0;
        } else if (key == sf::Keyboard::Key::Num4) {
            difficulty = 1;
        } else if (key == sf::Keyboard::Key::Num5) {
            difficulty = 2;
        } else if (key == sf::Keyboard::Key::Equal || key == sf::Keyboard::Key::Add) {
            volume = std::min(100.f, volume + 10.f);
            bgmMusic.setVolume(static_cast<float>(volume));
        } else if (key == sf::Keyboard::Key::Hyphen || key == sf::Keyboard::Key::Subtract) {
            volume = std::max(0.f, volume - 10.f);
            bgmMusic.setVolume(static_cast<float>(volume));
        } else if (key == sf::Keyboard::Key::Escape) {
            nextActionCode = StateAction::Exit;
        }
    }

    void handleMouseClick(sf::Vector2f pos) override {
        float W = static_cast<float>(cfg.screenWidth);
        float H = static_cast<float>(cfg.screenHeight);
        float btnY = H * 0.60f;
        float btnW = 200.f;
        float btnH = 50.f;
        float btnX = W / 2.f - btnW / 2.f;

        if (pos.x >= btnX && pos.x <= btnX + btnW &&
            pos.y >= btnY - btnH / 2.f && pos.y <= btnY + btnH / 2.f) {
            selectedOption = 0;
        }
    }

    void onEnter() override {
        birdTexture.loadFromFile(cfg.birdPath);
        if (birdSprite) {
            birdSprite->setTexture(birdTexture);
        }
    }

    StateAction nextAction() const override { return nextActionCode; }
    int selectedDifficulty() const override { return difficulty; }
    float getVolume() const { return volume; }

private:
    sf::Music& bgmMusic;
    bool bgmLoaded;
    int& highScore;
    const sf::Font* font = nullptr;

    std::unique_ptr<SoundManager> soundManager;
    int selectedOption = -1;
    StateAction nextActionCode = StateAction::None;
    ConfigValues cfg;
    float volume = 50.f;
    int difficulty = 1;

    // Visual timers
    float skyTimer = 0.f;
    float bobTimer = 0.f;
    float birdBobTimer = 0.f;
    float birdFlapTimer = 0.f;
    float buttonPulseTimer = 0.f;
    float birdArcTimer = 0.f;

    // Visual systems
    std::unique_ptr<CloudSystem> cloudSystem;
    std::unique_ptr<EnvironmentRenderer> environment;

    // Bird sprite
    sf::Texture birdTexture;
    std::unique_ptr<sf::Sprite> birdSprite = nullptr;

    // Color lerp helper
    static sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
        t = std::clamp(t, 0.f, 1.f);
        return sf::Color(
            static_cast<unsigned char>(a.r + (b.r - a.r) * t),
            static_cast<unsigned char>(a.g + (b.g - a.g) * t),
            static_cast<unsigned char>(a.b + (b.b - a.b) * t),
            static_cast<unsigned char>(a.a + (b.a - a.a) * t)
        );
    }

    void drawGradientSky(sf::RenderWindow& window, float W, float H) {
        float cyclePos = std::fmod(skyTimer, static_cast<float>(Config::SKY_CYCLE_INTERVAL)) / static_cast<float>(Config::SKY_CYCLE_INTERVAL);
        int idx1 = static_cast<int>(cyclePos * static_cast<float>(Config::SKY_COLOR_COUNT)) % Config::SKY_COLOR_COUNT;
        int idx2 = (idx1 + 1) % Config::SKY_COLOR_COUNT;
        float t = (cyclePos * static_cast<float>(Config::SKY_COLOR_COUNT)) - idx1;

        sf::Color top = lerpColor(Config::SKY_TOP[idx1], Config::SKY_TOP[idx2], t);
        sf::Color bot = lerpColor(Config::SKY_BOT[idx1], Config::SKY_BOT[idx2], t);

        sf::RectangleShape topRect;
        topRect.setSize({W, H / 2.f});
        topRect.setFillColor(top);
        window.draw(topRect);

        sf::RectangleShape botRect;
        botRect.setSize({W, H / 2.f});
        botRect.setPosition({0.f, H / 2.f});
        botRect.setFillColor(bot);
        window.draw(botRect);
    }

    void drawSunGlow(sf::RenderWindow& window, float W, float H) {
        float sunX = W * 0.75f;
        float sunY = H * 0.08f + std::sin(skyTimer * 0.3f) * 10.f;
        sf::CircleShape glow(80.f);
        glow.setFillColor(sf::Color(255, 200, 50, 20));
        glow.setPosition({sunX - 80.f, sunY - 80.f});
        window.draw(glow);

        sf::CircleShape sun(40.f);
        sun.setFillColor(sf::Color(255, 230, 100, 200));
        sun.setPosition({sunX - 40.f, sunY - 40.f});
        window.draw(sun);
    }

    void drawHills(sf::RenderWindow& window, float W, float H, float groundY) {
        // Back layer
        sf::RectangleShape hill;
        hill.setSize({W, 120.f});
        hill.setFillColor(sf::Color(80, 130, 80));
        hill.setPosition({0.f, groundY - 120.f});
        window.draw(hill);

        // Front layer
        sf::RectangleShape hill2;
        hill2.setSize({W, 60.f});
        hill2.setFillColor(sf::Color(100, 160, 100));
        hill2.setPosition({0.f, groundY - 60.f});
        window.draw(hill2);
    }

    void drawGround(sf::RenderWindow& window, float W, float H, float groundY) {
        sf::RectangleShape base;
        base.setSize({W, static_cast<float>(cfg.groundHeight)});
        base.setPosition({0.f, groundY});
        base.setFillColor(Config::GROUND_COLOR);
        window.draw(base);

        for (float x = -16.f; x < W + 16.f; x += static_cast<float>(Config::GROUND_TILE_W)) {
            sf::RectangleShape tile;
            tile.setSize({static_cast<float>(Config::GROUND_TILE_W), 8.f});
            tile.setPosition({x, groundY});
            tile.setFillColor(Config::GROUND_TOP);
            window.draw(tile);
        }

        for (float x = 0.f; x < W; x += 12.f) {
            float h = std::sin(x * 0.5f) * 2.f + 3.f;
            sf::RectangleShape blade;
            blade.setSize({2.f, h});
            blade.setPosition({x, groundY - h});
            blade.setFillColor(sf::Color(80, 140, 60));
            window.draw(blade);
        }
    }

    void drawTitleGlow(sf::RenderWindow& window, float W, float titleY) {
        // Glow layers
        sf::RectangleShape glow1;
        glow1.setSize({320.f, 70.f});
        glow1.setPosition({W / 2.f - 160.f, titleY - 35.f});
        glow1.setFillColor(sf::Color(255, 200, 50, 15));
        window.draw(glow1);

        sf::RectangleShape glow2;
        glow2.setSize({280.f, 50.f});
        glow2.setPosition({W / 2.f - 140.f, titleY - 25.f});
        glow2.setFillColor(sf::Color(255, 200, 50, 30));
        window.draw(glow2);
    }

    void drawAnimatedBird(sf::RenderWindow& window, float W) {
        float birdX = W / 2.f + std::sin(birdArcTimer * 0.8f) * 40.f;
        float birdY = static_cast<float>(cfg.screenHeight) * 0.38f + std::sin(birdBobTimer * 1.5f) * 15.f;

        if (!birdSprite) {
            // Fallback: draw a yellow circle
            sf::CircleShape fallback(24.f);
            fallback.setFillColor(sf::Color(255, 215, 0));
            fallback.setOrigin({24.f, 24.f});
            fallback.setPosition({birdX, birdY});
            window.draw(fallback);
        } else {
            birdSprite->setOrigin({20.f, 20.f});
            float flapScale = 1.0f + std::sin(birdFlapTimer * 8.0f) * 0.05f;
            birdSprite->setScale({flapScale, 1.f / flapScale});
            birdSprite->setPosition({birdX, birdY});
            window.draw(*birdSprite);
        }
    }

    void drawHighScorePill(sf::RenderWindow& window, float W, float hsY) {
        std::string hsText = "High Score: " + std::to_string(highScore);
        sf::Text hsMeasure(*font, hsText, 16);
        float hsWidth = hsMeasure.getLocalBounds().size.x;
        float pillW = hsWidth + 40.f;
        float pillH = 32.f;

        sf::RectangleShape pill(sf::Vector2f(pillW, pillH));
        pill.setPosition(sf::Vector2f(W / 2.f - pillW / 2.f, hsY - pillH / 2.f));
        pill.setFillColor(sf::Color(0, 0, 0, 60));
        pill.setOutlineColor(sf::Color(255, 215, 0, 100));
        pill.setOutlineThickness(1.f);
        window.draw(pill);

        auto hsTextObj = makeText(*font, hsText, 16, sf::Color(255, 215, 0, 180),
            sf::Vector2f(W / 2.f - (hsWidth / 2.f) - 20.f, hsY - 8.f));
        window.draw(hsTextObj);
    }

    void drawPlayButton(sf::RenderWindow& window, float W, float btnY) {
        float btnW = 200.f;
        float btnH = 50.f;
        float pulse = std::sin(buttonPulseTimer * 2.0f) * 4.f;

        // Glow
        sf::RectangleShape glow({btnW + pulse + 20.f, btnH + pulse + 20.f});
        glow.setPosition({W / 2.f - (btnW + pulse + 20.f) / 2.f, btnY - (btnH + pulse + 20.f) / 2.f});
        glow.setFillColor(sf::Color(102, 126, 234, 30));
        window.draw(glow);

        // Button body
        sf::RectangleShape btn({btnW, btnH});
        btn.setPosition({W / 2.f - btnW / 2.f, btnY - btnH / 2.f});
        btn.setFillColor(sf::Color(102, 126, 234));
        btn.setOutlineColor(sf::Color(255, 255, 255, 100));
        btn.setOutlineThickness(2.f);
        window.draw(btn);

        // Button text
        auto btnText = makeText(*font, "PLAY GAME", 20, sf::Color::White,
            {W / 2.f - 55.f, btnY - 10.f});
        window.draw(btnText);
    }

    void drawDifficultyPills(sf::RenderWindow& window, float W, float diffY) {
        static const char* names[] = {"Easy", "Medium", "Hard"};
        float pillW = 70.f;
        float pillH = 32.f;
        float gap = 16.f;
        float totalW = 3.f * pillW + 2.f * gap;
        float startX = W / 2.f - totalW / 2.f;

        for (int i = 0; i < 3; i++) {
            float px = startX + static_cast<float>(i) * (pillW + gap);
            float py = diffY - pillH / 2.f;

            sf::RectangleShape pill({pillW, pillH});
            if (i == difficulty) {
                pill.setFillColor(sf::Color(240, 147, 251));
                pill.setOutlineColor(sf::Color(255, 255, 255, 120));
                pill.setOutlineThickness(1.5f);
            } else {
                pill.setFillColor(sf::Color(255, 255, 255, 40));
                pill.setOutlineColor(sf::Color(255, 255, 255, 60));
                pill.setOutlineThickness(1.f);
            }
            pill.setPosition({px, py});
            window.draw(pill);

            auto pillText = makeText(*font, names[i], 13, i == difficulty ? sf::Color::White : sf::Color(255, 255, 255, 150),
                {px + (pillW - 30.f) / 2.f, py + (pillH - 14.f) / 2.f});
            window.draw(pillText);
        }
    }

    void drawVolumeBar(sf::RenderWindow& window, float W, float volY) {
        float barW = 100.f;
        float barH = 6.f;
        float barX = W / 2.f - barW / 2.f;
        float fillW = (volume / 100.f) * barW;

        auto volText = makeText(*font, "VOL", 14, sf::Color(180, 180, 180),
            {barX - 30.f, volY - 7.f});
        window.draw(volText);

        sf::RectangleShape track({barW, barH});
        track.setPosition({barX, volY - barH / 2.f});
        track.setFillColor(sf::Color(255, 255, 255, 60));
        window.draw(track);

        sf::RectangleShape fill({fillW, barH});
        fill.setPosition({barX, volY - barH / 2.f});
        fill.setFillColor(sf::Color(102, 126, 234));
        window.draw(fill);

        auto volPct = makeText(*font, std::to_string(static_cast<int>(volume)) + "%", 14,
            sf::Color(180, 180, 180), {barX + barW + 10.f, volY - 7.f});
        window.draw(volPct);
    }
};

#endif

#include <gtest/gtest.h>
#include "visual/EnvironmentRenderer.hpp"
#include "core/Config.hpp"
#include "core/ConfigValues.hpp"
#include <SFML/Graphics.hpp>

// --- EnvironmentRenderer constructs without crash ---

TEST(EnvironmentRendererTest, Constructs) {
    ConfigValues cfg;
    EnvironmentRenderer renderer(cfg);
    EXPECT_NO_THROW({});
}

// --- draw does not crash (with dummy render window) ---

TEST(EnvironmentRendererTest, DrawDoesNotCrash) {
    ConfigValues cfg;
    EnvironmentRenderer renderer(cfg);
    // Create a dummy render window (headless-safe for non-interactive)
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    // The draw function should not crash even with a valid window
    EXPECT_NO_THROW(renderer.draw(window, 0.f, 0.f));
}

// --- draw with scroll offset ---

TEST(EnvironmentRendererTest, DrawWithScrollOffset) {
    ConfigValues cfg;
    EnvironmentRenderer renderer(cfg);
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    EXPECT_NO_THROW(renderer.draw(window, 500.f, 0.f));
}

// --- draw with sky timer ---

TEST(EnvironmentRendererTest, DrawWithSkyTimer) {
    ConfigValues cfg;
    EnvironmentRenderer renderer(cfg);
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    EXPECT_NO_THROW(renderer.draw(window, 0.f, 10.0f));
}

// --- draw with both scroll and sky timer ---

TEST(EnvironmentRendererTest, DrawWithBothParameters) {
    ConfigValues cfg;
    EnvironmentRenderer renderer(cfg);
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned>(Config::SCREEN_WIDTH), static_cast<unsigned>(Config::SCREEN_HEIGHT)}), "Test");
    window.clear();
    EXPECT_NO_THROW(renderer.draw(window, 250.f, 5.0f));
}

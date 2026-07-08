#ifndef ENVIRONMENT_RENDERER_HPP
#define ENVIRONMENT_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "core/ConfigValues.hpp"

class EnvironmentRenderer {
public:
    explicit EnvironmentRenderer(const ConfigValues& cfg);
    void draw(sf::RenderWindow& window, float scrollOffset, float skyTimer);

private:
    float bgOffset;
    ConfigValues cfg;
};

#endif // ENVIRONMENT_RENDERER_HPP

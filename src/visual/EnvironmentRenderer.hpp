#ifndef ENVIRONMENT_RENDERER_HPP
#define ENVIRONMENT_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"

class EnvironmentRenderer {
public:
    EnvironmentRenderer() : bgOffset(0.f) {}
    void draw(sf::RenderWindow& window, float scrollOffset, float skyTimer);

private:
    float bgOffset;
};

#endif // ENVIRONMENT_RENDERER_HPP

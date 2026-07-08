#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "core/ConfigValues.hpp"

struct Cloud {
    float x, y, speed, radius, alpha;
};

class CloudSystem {
public:
    explicit CloudSystem(const ConfigValues& cfg);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    size_t getCloudCount() const { return clouds.size(); }
private:
    std::vector<Cloud> clouds;
    float cloudOffset = 0.f;
    ConfigValues cfg;
};

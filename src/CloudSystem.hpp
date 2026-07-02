#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Cloud {
    float x, y, speed, radius, alpha;
};

class CloudSystem {
public:
    CloudSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
private:
    std::vector<Cloud> clouds;
    float cloudOffset = 0.f;
};

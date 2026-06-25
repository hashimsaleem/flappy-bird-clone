#include "Pipe.hpp"
#include <cmath>
#include <algorithm>

Pipe::Pipe(float x, float y, float gapHeight, float speed, PipeType type)
    : velocityX(-speed), type(type), baseY(y), gapHeightValue(gapHeight) {
    
    float topPipeHeight = 400.f - (gapHeight / 2.f);
    float bottomPipeHeight = 400.f - (gapHeight / 2.f);

    topPipe.setSize({60.f, topPipeHeight});
    topPipe.setPosition({x, (y - gapHeight / 2.f) - topPipeHeight});
    topPipe.setFillColor(sf::Color(34, 139, 34)); // Forest green

    bottomPipe.setSize({60.f, bottomPipeHeight});
    bottomPipe.setPosition({x, y + gapHeight / 2.f});
    bottomPipe.setFillColor(sf::Color(34, 139, 34));
}

void Pipe::reset(float x, float y, float gapHeight, float speed, PipeType type) {
    velocityX = -speed;
    this->type = type;
    baseY = y;
    gapHeightValue = gapHeight;
    passed = false;

    float topPipeHeight = 400.f - (gapHeight / 2.f);
    float bottomPipeHeight = 400.f - (gapHeight / 2.f);

    topPipe.setSize({60.f, topPipeHeight});
    topPipe.setPosition({x, (y - gapHeight / 2.f) - topPipeHeight});
    topPipe.setFillColor(sf::Color(34, 139, 34));

    bottomPipe.setSize({60.f, bottomPipeHeight});
    bottomPipe.setPosition({x, y + gapHeight / 2.f});
    bottomPipe.setFillColor(sf::Color(34, 139, 34));
}

void Pipe::update(float dt) {
    if (type == PipeType::MOVING) {
        oscillationTimer += dt;
        float offset = std::sin(oscillationTimer * 2.0f) * 50.f;
        topPipe.move({velocityX * dt, offset});
        bottomPipe.move({velocityX * dt, offset});
    } else {
        topPipe.move({velocityX * dt, 0.f});
        bottomPipe.move({velocityX * dt, 0.f});
    }
}

void Pipe::draw(sf::RenderWindow& window) const {
    window.draw(topPipe);
    window.draw(bottomPipe);
}

bool Pipe::checkCollision(sf::FloatRect birdBounds) const {
    sf::FloatRect topBounds = topPipe.getGlobalBounds();
    sf::FloatRect bottomBounds = bottomPipe.getGlobalBounds();

    return (birdBounds.position.x < topBounds.position.x + topBounds.size.x &&
            birdBounds.position.x + birdBounds.size.x > topBounds.position.x &&
            birdBounds.position.y < topBounds.position.y + topBounds.size.y &&
            birdBounds.position.y + birdBounds.size.y > topBounds.position.y) ||
           (birdBounds.position.x < bottomBounds.position.x + bottomBounds.size.x &&
            birdBounds.position.x + birdBounds.size.x > bottomBounds.position.x &&
            birdBounds.position.y < bottomBounds.position.y + bottomBounds.size.y &&
            birdBounds.position.y + birdBounds.size.y > bottomBounds.position.y);
}

bool Pipe::isOffScreen() const {
    return topPipe.getPosition().x + topPipe.getSize().x < 0;
}

float Pipe::getX() const {
    return topPipe.getPosition().x;
}

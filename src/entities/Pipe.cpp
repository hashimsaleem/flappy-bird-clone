#include "Pipe.hpp"
#include "core/MathUtils.hpp"
#include <cmath>
#include <algorithm>

Pipe::Pipe(float x, float y, float gapHeight, float speed, PipeType type) {
    reset(x, y, gapHeight, speed, type);
}

void Pipe::reset(float x, float y, float gapHeight, float speed, PipeType type) {
    velocityX = -speed;
    this->type = type;
    baseY = y;
    gapHeightValue = gapHeight;
    passed = false;

    topPipe.setSize({Config::PIPE_WIDTH, Config::PIPE_HEIGHT});
    topPipe.setPosition({x, (y - gapHeight / 2.f) - Config::PIPE_HEIGHT});
    topPipe.setFillColor(Config::PIPE_COLOR);

    bottomPipe.setSize({Config::PIPE_WIDTH, Config::PIPE_HEIGHT});
    bottomPipe.setPosition({x, y + gapHeight / 2.f});
    bottomPipe.setFillColor(Config::PIPE_COLOR);
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
    return aabbOverlap(birdBounds, topPipe.getGlobalBounds()) ||
           aabbOverlap(birdBounds, bottomPipe.getGlobalBounds());
}

bool Pipe::isOffScreen() const {
    return topPipe.getPosition().x + topPipe.getSize().x < 0;
}

float Pipe::getX() const {
    return topPipe.getPosition().x;
}

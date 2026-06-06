#include "Pipe.hpp"
#include <cmath>

Pipe::Pipe(float x, float y, float gapHeight, float speed, PipeType type) 
    : velocityX(-speed), type(type), baseY(y), gapHeightValue(gapHeight) {

    // Setting up the bottom pipe
    bottomPipe.setSize({Config::PIPE_WIDTH, Config::PIPE_HEIGHT});
    bottomPipe.setPosition({x, y + gapHeight / 2.0f});
    bottomPipe.setFillColor(Config::PIPE_COLOR);

    // Setting up the top pipe
    topPipe.setSize({Config::PIPE_WIDTH, Config::PIPE_HEIGHT});
    topPipe.setPosition({x, y - gapHeight / 2.0f - Config::PIPE_HEIGHT});
    topPipe.setFillColor(Config::PIPE_COLOR);
}

void Pipe::update(float dt) {
    float deltaX = velocityX * dt;
    bottomPipe.move({deltaX, 0.0f});
    topPipe.move({deltaX, 0.0f});

    if (type == PipeType::MOVING) {
        oscillationTimer += dt;
        // Oscillate Y position using a sine wave: amplitude 100, frequency 1Hz
        float offsetY = std::sin(oscillationTimer * 2.0f * 3.14159f) * 100.0f;
        
        // We need to update positions relative to baseY
        float currentX = bottomPipe.getPosition().x;
        bottomPipe.setPosition({currentX, baseY + gapHeightValue / 2.0f + offsetY});
        topPipe.setPosition({currentX, baseY - gapHeightValue / 2.0f - Config::PIPE_HEIGHT + offsetY});
    }
}

void Pipe::draw(sf::RenderWindow& window) const {
    window.draw(topPipe);
    window.draw(bottomPipe);
}

bool Pipe::checkCollision(sf::FloatRect birdBounds) const {
    // Check if bird intersects with either topPipe or bottomPipe
    return topPipe.getGlobalBounds().findIntersection(birdBounds).has_value() ||
           bottomPipe.getGlobalBounds().findIntersection(birdBounds).has_value();
}

bool Pipe::isOffScreen() const {
    // If the right side of the pipe is off screen (assuming SCREEN_WIDTH)
    return bottomPipe.getPosition().x + Config::PIPE_WIDTH < 0;
}

float Pipe::getX() const {
    return bottomPipe.getPosition().x;
}

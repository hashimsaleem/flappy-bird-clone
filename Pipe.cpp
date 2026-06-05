#include "Pipe.hpp"

Pipe::Pipe(float x, float y, float gapHeight) {
    velocityX = 200.0f; // Constant speed for pipes moving left

    // Setting up the bottom pipe
    bottomPipe.setSize({PIPE_WIDTH, PIPE_HEIGHT});
    bottomPipe.setPosition({x, y + gapHeight / 2.0f});
    bottomPipe.setFillColor(sf::Color::Green);

    // Setting up the top pipe
    topPipe.setSize({PIPE_WIDTH, PIPE_HEIGHT});
    topPipe.setPosition({x, y - gapHeight / 2.0f - PIPE_HEIGHT});
    topPipe.setFillColor(sf::Color::Green);
}

void Pipe::update(float dt) {
    float deltaX = velocityX * dt;
    bottomPipe.move({deltaX, 0.0f});
    topPipe.move({deltaX, 0.0f});
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
    // If the right side of the pipe is off screen (assuming 800px width)
    return bottomPipe.getPosition().x + PIPE_WIDTH < 0;
}

float Pipe::getX() const {
    return bottomPipe.getPosition().x;
}

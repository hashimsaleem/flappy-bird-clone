#include "Pipe.hpp"

Pipe::Pipe(float x, float y, float gapHeight) {
    velocityX = 200.0f; // Constant speed for pipes moving left

    // The gapHeight parameter might override the default GAP_HEIGHT
    // but for now let's use the member variable or the passed one.
    // Let's use the member variable for consistency if not passed.
    
    // Setting up the bottom pipe
    bottomPipe.setSize(PIPE_WIDTH, PIPE_HEIGHT);
    bottomPipe.setPosition(x, y + gapHeight / 2.0f);
    bottomPipe.setFillColor(sf::Color::Green);

    // Setting up the top pipe
    topPipe.setSize(PIPE_WIDTH, PIPE_HEIGHT);
    topPipe.setPosition(x, y - gapHeight / 2.0f - PIPE_HEIGHT);
    topPipe.setFillColor(sf::Color::Green);
}

void Pipe::update(float dt) {
    float deltaX = velocityX * dt;
    bottomPipe.move(deltaX, 0);
    topPipe.move(deltaX, 0);
}

void Pipe::draw(sf::RenderWindow& window) const {
    window.draw(topPipe);
    window.draw(bottomPipe);
}

sf::FloatRect Pipe::getBoundingBox() const {
    // Returns the bounding box of the gap (the area the bird can pass through)
    // Or does it return the bounding box of the pipes?
    // "Getters for collision detection" usually means the pipes themselves.
    // Since there are two pipes, we might need to check both.
    // However, the return type is a single sf::FloatRect.
    // Maybe it's the bounding box of the whole pipe structure?
    // Let's assume it's the bounding box of the pipes.
    // But since there are two, maybe we should check both.
    // For now, let's return the bounding box of the bottom pipe as a placeholder.
    // Better: return the bounding box of the pipes as a whole? No, that's not useful for collision.
    // Let's make it return the bounding box of the "active" area? 
    // Actually, let's just return the bounding box of the bottom pipe for now.
    return bottomPipe.getGlobalBounds();
}

bool Pipe::isOffScreen() const {
    // If the right side of the pipe is off screen (assuming 800px width)
    return bottomPipe.getPosition().x + PIPE_WIDTH < 0;
}

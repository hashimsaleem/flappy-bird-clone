#include "Bird.hpp"

// NOTE: For a real game, you would load this texture once during setup. 
// We use a placeholder assumption here for initialization purposes.
Bird::Bird() : velocityY(0.0f), posY(300.0f) {
    sprite.setTextureRect(sf::IntRect(0, 0, 50, 50)); // Placeholder size
}

void Bird::load(const std::string& texturePath) {
    if (texturePath.empty()) return;
    // In a real scenario: if (!texture.loadFromFile(texturePath)) ...
    // We will assume the image is loaded correctly for now.
    sprite.setTexture(texture); 
    // Assuming size of 50x50 for simplicity without actual assets
    sprite.setScale(2.f, 2.f); 
}

void Bird::update(float dt) {
    // Apply gravity: velocity increases downwards over time (negative direction)
    velocityY += GRAVITY * dt;

    // Calculate new position delta: distance = velocity * time
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    this->posY += deltaY;

    // Update SFML sprite position based on physics calculations
    sprite.setPosition(50, posY); 
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = JUMP_STRENGTH;
}

void Bird::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}
#include "Bird.hpp"
#include <iostream>

Bird::Bird() : velocityY(0.0f), posY(300.0f) {
    // Placeholder: Initial position
    sprite.setPosition({50.f, posY});
}

void Bird::load(const std::string& texturePath) {
    if (texture.loadFromFile(texturePath)) {
        sprite.setTexture(texture);
    } else {
        std::cerr << "Error loading bird texture from " << texturePath << std::endl;
    }
}

void Bird::update(float dt) {
    // Apply gravity: velocity increases downwards over time
    velocityY += GRAVITY * dt;

    // Calculate new position delta
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    this->posY += deltaY;

    // Update the sprite position based on physics calculations
    sprite.setPosition({50.f, posY}); 
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = JUMP_STRENGTH;
}

void Bird::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Bird::getBoundingBox() const {
    return sprite.getGlobalBounds();
}

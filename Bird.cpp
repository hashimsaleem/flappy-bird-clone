#include "Bird.hpp"

Bird::Bird() : velocityY(0.0f), posY(300.0f) {
    shape.setSize({50.f, 50.f});
    shape.setFillColor(sf::Color::Yellow);
}

void Bird::load(const std::string& texturePath) {
    // Placeholder: Since we are using a RectangleShape for now, we don't need to load a texture.
    // In a future version, we will switch back to sf::Sprite.
}

void Bird::update(float dt) {
    // Apply gravity: velocity increases downwards over time (negative direction)
    velocityY += GRAVITY * dt;

    // Calculate new position delta: distance = velocity * time
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    this->posY += deltaY;

    // Update the shape position based on physics calculations
    shape.setPosition({50.f, posY}); 
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = JUMP_STRENGTH;
}

void Bird::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}
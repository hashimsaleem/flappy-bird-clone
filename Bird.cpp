#include "Bird.hpp"
#include <iostream>

Bird::Bird() : velocityY(0.0f), posX(Config::BIRD_START_X), posY(Config::BIRD_START_Y) {
    // Create a white placeholder texture (lifetime tied to this Bird instance)
    sf::Image placeholderImg({static_cast<unsigned int>(Config::BIRD_WIDTH), static_cast<unsigned int>(Config::BIRD_HEIGHT)}, sf::Color::White);
    if (!placeholderTexture.loadFromImage(placeholderImg)) {
        std::cerr << "Error: Failed to create placeholder texture." << std::endl;
    }
    sprite = new sf::Sprite(placeholderTexture);
    sprite->setPosition({posX, posY});
}

void Bird::load(const std::string& texturePath) {
    if (birdTexture.loadFromFile(texturePath)) {
        // Delete the placeholder sprite and create a new one with the real texture
        delete sprite;
        sprite = new sf::Sprite(birdTexture);
        sprite->setOrigin({birdTexture.getSize().x / 2.f, birdTexture.getSize().y / 2.f});
        sprite->setPosition({posX, posY});
    } else {
        // Keep the placeholder sprite — it's still valid since placeholderTexture is a member
        sprite = new sf::Sprite(placeholderTexture);
        sprite->setPosition({posX, posY});
    }
}

Bird::~Bird() {
    delete sprite;
}

void Bird::update(float dt) {
    // Apply gravity: velocity increases downwards over time
    velocityY += Config::GRAVITY * dt;

    // Calculate new position delta
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    posY += deltaY;

    // Update the sprite position based on physics calculations
    sprite->setPosition({posX, posY});
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = Config::JUMP_STRENGTH;
}

void Bird::draw(sf::RenderWindow& window) const {
    if (sprite) {
        window.draw(*sprite);
    }
}

sf::FloatRect Bird::getBoundingBox() const {
    if (sprite) {
        return sprite->getGlobalBounds();
    }
    return sf::FloatRect({posX, posY}, {Config::BIRD_WIDTH, Config::BIRD_HEIGHT});
}

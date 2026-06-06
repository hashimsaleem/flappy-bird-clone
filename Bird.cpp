#include "Bird.hpp"
#include <iostream>

Bird::Bird() : velocityY(0.0f), posX(Config::BIRD_START_X), posY(Config::BIRD_START_Y) {
    // Create a temporary white texture as placeholder using sf::Image constructor
    sf::Image placeholderImg({static_cast<unsigned int>(Config::BIRD_WIDTH), static_cast<unsigned int>(Config::BIRD_HEIGHT)}, sf::Color::White);
    sf::Texture placeholder(placeholderImg);
    sprite = new sf::Sprite(placeholder);
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
        std::cerr << "Warning: Could not load bird texture '" << texturePath << "'. Using placeholder." << std::endl;
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

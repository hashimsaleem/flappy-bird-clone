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
        sprite->setOrigin({placeholderTexture.getSize().x / 2.f, placeholderTexture.getSize().y / 2.f});
        sprite->setPosition({posX, posY});
    }
}

Bird::~Bird() {
    delete sprite;
}

void Bird::update(float dt) {
    if (isDying) {
        // Death physics: accelerate downwards and spin rapidly
        velocityY += Config::GRAVITY * 1.5f * dt;
        posY += velocityY * dt;
        tiltAngle += 360.0f * dt; // Spin 360 degrees per second
        sprite->setPosition({posX, posY});
        sprite->setRotation(sf::degrees(tiltAngle));
        return;
    }

    // Apply gravity: velocity increases downwards over time
    velocityY += Config::GRAVITY * dt;

    // Calculate new position delta
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    posY += deltaY;

    // Update the sprite position based on physics calculations
    sprite->setPosition({posX, posY});

    // Animate tilt and wing flap
    animateTilt(dt);
}

void Bird::animateTilt(float dt) {
    // Calculate target tilt angle from velocity
    // Clamp velocity to a reasonable range for tilt mapping
    float vel = velocityY;
    float minVel = Config::JUMP_STRENGTH * 0.8f;  // ~-480
    float maxVel = Config::GRAVITY * 0.6f;          // ~480

    float t = 0.0f;
    if (vel < minVel) {
        t = 0.0f;
    } else if (vel > maxVel) {
        t = 1.0f;
    } else {
        t = (vel - minVel) / (maxVel - minVel);
    }

    float targetAngle = Config::BIRD_MIN_TILT + t * (Config::BIRD_MAX_TILT - Config::BIRD_MIN_TILT);

    // Smooth interpolation toward target
    float lerpSpeed = Config::BIRD_TILT_LERP * dt;
    tiltAngle += (targetAngle - tiltAngle) * std::min(lerpSpeed, 1.0f);

    // Wing flap oscillation (only when velocity is negative — going up)
    if (vel < -50.0f) {
        flapTimer += dt;
        float flap = std::sin(flapTimer * Config::BIRD_FLAP_RATE * 3.14159f)
                     * Config::BIRD_FLAP_DEPTH;
        sprite->setPosition({posX, posY + flap});
    } else {
        flapTimer = 0.0f;
    }

    // Apply tilt rotation (SFML 3 uses sf::degrees() free function)
    if (sprite) {
        sprite->setRotation(sf::degrees(tiltAngle));
    }
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = Config::JUMP_STRENGTH;
}

void Bird::draw(sf::RenderWindow& window) {
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

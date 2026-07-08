#include "Bird.hpp"
#include <iostream>

Bird::Bird(const ConfigValues& cfg)
    : velocityY(0.0f),
      posX(cfg.birdStartX),
      posY(cfg.birdStartY),
      gravity(cfg.gravity),
      jumpStrength(cfg.jumpStrength),
      birdMinTilt(cfg.birdMinTilt),
      birdMaxTilt(cfg.birdMaxTilt),
      birdFlapRate(cfg.birdFlapRate),
      birdFlapDepth(cfg.birdFlapDepth) {
    sf::Image placeholderImg({static_cast<unsigned int>(cfg.birdWidth), static_cast<unsigned int>(cfg.birdHeight)}, sf::Color::White);
    if (!placeholderTexture.loadFromImage(placeholderImg)) {
        std::cerr << "Error: Failed to create placeholder texture." << std::endl;
    }
    sprite = std::make_unique<sf::Sprite>(placeholderTexture);
    sprite->setPosition({posX, posY});
}

void Bird::load(const std::string& texturePath) {
    if (birdTexture.loadFromFile(texturePath)) {
        // Delete the placeholder sprite and create a new one with the real texture
        sprite.reset();
        sprite = std::make_unique<sf::Sprite>(birdTexture);
        sprite->setOrigin({birdTexture.getSize().x / 2.f, birdTexture.getSize().y / 2.f});
        sprite->setPosition({posX, posY});
    } else {
        // Keep the placeholder sprite — it's still valid since placeholderTexture is a member
        if (sprite) sprite.reset();
        sprite = std::make_unique<sf::Sprite>(placeholderTexture);
        sprite->setOrigin({placeholderTexture.getSize().x / 2.f, placeholderTexture.getSize().y / 2.f});
        sprite->setPosition({posX, posY});
    }
}

void Bird::update(float dt) {
    if (isDying) {
        // Death physics: accelerate downwards and spin rapidly
        velocityY += gravity * 1.5f * dt;
        posY += velocityY * dt;
        tiltAngle += 360.0f * dt; // Spin 360 degrees per second
        if (sprite) {
            sprite->setPosition({posX, posY});
            sprite->setRotation(sf::degrees(tiltAngle));
        }
        return;
    }

    // Apply gravity: velocity increases downwards over time
    velocityY += gravity * dt;

    // Calculate new position delta
    float deltaY = velocityY * dt;

    // Update the Y position tracker
    posY += deltaY;

    // Update the sprite position based on physics calculations
    if (sprite) {
        sprite->setPosition({posX, posY});
    }

    // Animate tilt and wing flap
    animateTilt(dt);
}

void Bird::animateTilt(float dt) {
    if (wingFlapActive) {
        wingFlapTimer -= dt;
        float t = std::max(0.0f, wingFlapTimer / 0.15f);
        float sx = 1.0f + 0.25f * t;
        float sy = 1.0f - 0.35f * t;
        if (sprite) sprite->setScale({sx, sy});
        if (wingFlapTimer <= 0.0f) {
            wingFlapActive = false;
            if (sprite) sprite->setScale({1.0f, 1.0f});
        }
    }

    // Calculate target tilt angle from velocity
    // Clamp velocity to a reasonable range for tilt mapping
    float vel = velocityY;
    float minVel = jumpStrength * 0.8f;  // ~-480
    float maxVel = gravity * 0.6f;          // ~480

    float t = 0.0f;
    if (vel < minVel) {
        t = 0.0f;
    } else if (vel > maxVel) {
        t = 1.0f;
    } else {
        t = (vel - minVel) / (maxVel - minVel);
    }

    float targetAngle = birdMinTilt + t * (birdMaxTilt - birdMinTilt);

    // Smooth interpolation toward target
    float lerpSpeed = Config::BIRD_TILT_LERP * dt;
    tiltAngle += (targetAngle - tiltAngle) * std::min(lerpSpeed, 1.0f);

    // Wing flap oscillation (only when velocity is negative — going up)
    if (vel < -50.0f) {
        flapTimer += dt;
        float flap = std::sin(flapTimer * birdFlapRate * 3.14159f)
                     * birdFlapDepth;
        if (sprite) {
            sprite->setPosition({posX, posY + flap});
        }
    } else {
        flapTimer = 0.0f;
    }

    // Apply tilt rotation (SFML 3 uses sf::degrees() free function)
    if (sprite) {
        sprite->setRotation(sf::degrees(tiltAngle));
    }
}

void Bird::setRestartPos(float x, float y) {
    posX = x;
    posY = y;
    if (sprite) sprite->setPosition({x, y});
}

void Bird::setRestartVel(float vel) {
    velocityY = vel;
}

void Bird::flap() {
    velocityY = jumpStrength;
    wingFlapTimer = 0.15f;
    wingFlapActive = true;
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

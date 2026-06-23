#include "Bird.hpp"
#include "ConfigLoader.hpp"
#include <iostream>

Bird::Bird() : velocityY(0.0f), posX(Config::BIRD_START_X), posY(Config::BIRD_START_Y) {
    // Create a white placeholder texture (lifetime tied to this Bird instance)
    sf::Image placeholderImg({static_cast<unsigned int>(Config::BIRD_WIDTH), static_cast<unsigned int>(Config::BIRD_HEIGHT)}, sf::Color::White);
    if (!placeholderTexture.loadFromImage(placeholderImg)) {
        std::cerr << "Error: Failed to create placeholder texture." << std::endl;
    }
    sprite = new sf::Sprite(placeholderTexture);
    sprite->setPosition({posX, posY});

    // Initialize from config
    gravity = ConfigLoader::getFloat("gravity", Config::GRAVITY);
    jumpStrength = ConfigLoader::getFloat("jump_strength", Config::JUMP_STRENGTH);
    birdMinTilt = ConfigLoader::getFloat("bird_min_tilt", Config::BIRD_MIN_TILT);
    birdMaxTilt = ConfigLoader::getFloat("bird_max_tilt", Config::BIRD_MAX_TILT);
    birdFlapRate = ConfigLoader::getFloat("bird_flap_rate", Config::BIRD_FLAP_RATE);
    birdFlapDepth = ConfigLoader::getFloat("bird_flap_depth", Config::BIRD_FLAP_DEPTH);
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
        if (sprite) delete sprite;
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
    // Handle wing flap color animation
    if (wingFlapActive) {
        wingFlapTimer -= dt;
        if (wingFlapTimer <= 0.0f) {
            wingFlapActive = false;
            if (sprite) sprite->setColor(sf::Color::White);
        } else {
            // Pulse the color during flap
            float pulse = std::sin(wingFlapTimer * birdFlapRate) * 0.5f + 0.5f;
            float r = 255.0f;
            float g = 255.0f * pulse;
            float b = 200.0f * (1.0f - pulse);
            if (sprite) sprite->setColor(sf::Color(
                static_cast<unsigned char>(r),
                static_cast<unsigned char>(g),
                static_cast<unsigned char>(b)));
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
    if (sprite) sprite->setPosition(sf::Vector2f(x, y));
}

void Bird::setRestartVel(float vel) {
    velocityY = vel;
}

void Bird::flap() {
    // Set a strong negative (upward) velocity
    velocityY = jumpStrength;
    // Trigger visual wing flap: brighten sprite briefly
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

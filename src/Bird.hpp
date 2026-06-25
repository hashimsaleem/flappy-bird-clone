#ifndef BIRD_HPP
#define BIRD_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include "Config.hpp"

class Bird {
private:
    std::unique_ptr<sf::Sprite> sprite; // Managed by unique_ptr
    sf::Texture birdTexture;
    sf::Texture placeholderTexture; // Lifetime tied to Bird instance
    float velocityY; // Vertical velocity

    // Configurable parameters
    float gravity;
    float jumpStrength;
    float birdMinTilt;
    float birdMaxTilt;
    float birdFlapRate;
    float birdFlapDepth;

public:
    Bird();
    ~Bird() = default; // unique_ptr handles deletion

    // Prevent copying — unique_ptr is non-copyable by default
    Bird(const Bird&) = delete;
    Bird& operator=(const Bird&) = delete;
    Bird(Bird&&) = delete;
    Bird& operator=(Bird&&) = delete;
    // Initializes the sprite and places it at a starting point
    void load(const std::string& texturePath);

    /**
     * @brief Updates the bird's position based on physics and delta time.
     * @param dt Delta time in seconds.
     */
    void update(float dt);

    /**
     * @brief Triggers the death animation state.
     */
    void setDying() { isDying = true; }
    bool isDyingFlag() const { return isDying; }

    /**\n     * @brief Makes the bird jump (resets vertical velocity).
     */
    void flap();

    /**
     * @brief Draws the bird to the window.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Gets the bounding box of the bird.
     * @return sf::FloatRect
     */
    sf::FloatRect getBoundingBox() const;

    float getVelocity() const { return velocityY; }
    float getX() const { return posX; }
    void setRestartPos(float x, float y);
    void setRestartVel(float vel);

    /**
     * @brief Resets the bird to its initial state (called on game restart).
     */
    void reset() {
        velocityY = 0.0f;
        posX = Config::BIRD_START_X;
        posY = Config::BIRD_START_Y;
        tiltAngle = 0.0f;
        flapTimer = 0.0f;
        isDying = false;
        wingFlapActive = false;
        wingFlapTimer = 0.0f;
        if (sprite) {
            sprite->setPosition({posX, posY});
            sprite->setRotation(sf::degrees(0.f));
            sprite->setScale({1.f, 1.f});
        }
    }

private:
    void animateTilt(float dt);

    float wingFlapTimer = 0.0f;
    bool wingFlapActive = false;

    float posX;    // X position tracker
    float posY;    // Y position tracker (needed for collision and physics)
    float tiltAngle = 0.0f;  // Current visual tilt angle (degrees)
    float flapTimer = 0.0f;  // Timer for wing flap oscillation
    bool isDying = false;    // Flag for death animation
};

#endif // BIRD_HPP

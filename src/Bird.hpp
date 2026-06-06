#ifndef BIRD_HPP
#define BIRD_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Config.hpp"

class Bird {
private:
    sf::Sprite* sprite = nullptr; // Initialized in load()
    sf::Texture birdTexture;
    sf::Texture placeholderTexture; // Lifetime tied to Bird instance
    float velocityY; // Vertical velocity

public:
    Bird();
    ~Bird();
    // Initializes the sprite and places it at a starting point
    void load(const std::string& texturePath);

    /**
     * @brief Updates the bird's position based on physics and delta time.
     * @param dt Delta time in seconds.
     */
    void update(float dt);

    /**\n     * @brief Makes the bird jump (resets vertical velocity).
     */
    void flap();

    /**
     * @brief Draws the bird to the window.
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * @brief Gets the bounding box of the bird.
     * @return sf::FloatRect
     */
    sf::FloatRect getBoundingBox() const;

private:
    float posX; // X position tracker
    float posY; // Y position tracker (needed for collision and physics)
};

#endif // BIRD_HPP

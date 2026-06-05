#ifndef BIRD_HPP
#define BIRD_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

class Bird {
private:
    sf::RectangleShape shape;
    float velocityY; // Vertical velocity
    const float GRAVITY = 10.0f; // Gravity strength (pixels/s^2)
    const float JUMP_STRENGTH = -350.0f; // Initial upward velocity (- is up)

public:
    Bird();
    // Initializes the shape and places it at a starting point
    void load(const std::string& texturePath); 

    /**
     * @brief Updates the bird's position based on physics and delta time.
     * @param dt Delta time in seconds.
     */
    void update(float dt);

    /**
     * @brief Makes the bird jump (resets vertical velocity).
     */
    void flap();

    /**
     * @brief Draws the bird to the window.
     */
    void draw(sf::RenderWindow& window) const;

private:
    float posY; // Y position tracker (needed for collision and physics)
};

#endif // BIRD_HPP
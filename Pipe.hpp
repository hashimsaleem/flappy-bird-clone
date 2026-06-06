#ifndef PIPE_HPP
#define PIPE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Config.hpp"

class Pipe {
public:
    Pipe(float x, float y, float gapHeight);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    /**
     * @brief Checks if the bird's bounding box intersects with either the top or bottom pipe.
     * @param birdBounds The bounding box of the bird.
     * @return true if there is a collision, false otherwise.
     */
    bool checkCollision(sf::FloatRect birdBounds) const;

    bool isOffScreen() const;

    /**
     * @brief Gets the x position of the pipe.
     * @return float x position
     */
    float getX() const;

    bool passed = false; // To track if the bird has passed this pipe

private:
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    float velocityX;
};

#endif // PIPE_HPP

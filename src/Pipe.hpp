#ifndef PIPE_HPP
#define PIPE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Config.hpp"

enum class PipeType { STATIC, MOVING };

class Pipe {
public:
    // Constructor now takes speed and type to allow for dynamic difficulty and variety
    Pipe(float x, float y, float gapHeight, float speed, PipeType type = PipeType::STATIC);

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

    /**
     * @brief Resets the pipe's properties for reuse in a pool.
     */
    void reset(float x, float y, float gapHeight, float speed, PipeType type);

private:
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    float velocityX;
    PipeType type;
    float oscillationTimer = 0.0f;
    float baseY;
    float gapHeightValue;
};

#endif // PIPE_HPP

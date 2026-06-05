#ifndef PIPE_HPP
#define PIPE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

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

private:
    sf::Sprite topPipe;
    sf::Sprite bottomPipe;
    sf::Texture pipeTexture;
    float velocityX;
    bool passed = false; // To track if the bird has passed this pipe
    static constexpr float PIPE_WIDTH = 60.0f;

    static constexpr float PIPE_HEIGHT = 400.0f; // Total height of one pipe segment
    static constexpr float GAP_HEIGHT = 150.0f;
};

#endif // PIPE_HPP

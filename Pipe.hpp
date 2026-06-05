#ifndef PIPE_HPP
#define PIPE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Pipe {
public:
    Pipe(float x, float y, float gapHeight);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // Getters for collision detection
    sf::FloatRect getBoundingBox() const;
    bool isOffScreen() const;

private:
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    float velocityX;
    static constexpr float PIPE_WIDTH = 60.0f;

    static constexpr float PIPE_HEIGHT = 400.0f; // Total height of one pipe segment
    static constexpr float GAP_HEIGHT = 150.0f;
};

#endif // PIPE_HPP

#ifndef POWERUP_HPP
#define POWERUP_HPP
 
#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include <memory>
 
enum class PowerUpType { INVINCIBILITY, SLOW_MOTION };
 
class PowerUp {
public:
    PowerUp(float x, float y, PowerUpType type);
    ~PowerUp() = default;
 
    void update(float dt);
    void draw(sf::RenderWindow& window) const;
 
    bool checkCollision(sf::FloatRect birdBounds) const;
    
    PowerUpType getType() const { return type; }
    bool isOffScreen() const;
 
    void reset(float x, float y, PowerUpType type);
 
private:
    std::unique_ptr<sf::Sprite> sprite;
    PowerUpType type;
    float velocityX;
    float duration;
    float remainingTime;
};
 
#endif // POWERUP_HPP


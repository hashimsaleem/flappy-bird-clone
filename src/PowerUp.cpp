#include "PowerUp.hpp"
#include "ResourceManager.hpp"
 
PowerUp::PowerUp(float x, float y, PowerUpType type) {
    reset(x, y, type);
}
 
void PowerUp::reset(float x, float y, PowerUpType type) {
    this->type = type;
    velocityX = -Config::PIPE_SPEED * 0.8f;
    remainingTime = 10.0f;
    
    if (type == PowerUpType::INVINCIBILITY) {
        sprite = std::make_unique<sf::Sprite>(ResourceManager::getInstance().getTexture("assets/invincibility.png"));
    } else {
        sprite = std::make_unique<sf::Sprite>(ResourceManager::getInstance().getTexture("assets/slow_motion.png"));
    }
    sprite->setPosition({x, y});
}

void PowerUp::reset(float x, float y) {
    sprite->setPosition({x, y});
}
 
void PowerUp::update(float dt) {
    if (sprite) {
        sprite->move({velocityX * dt, 0.f});
        remainingTime -= dt;
    }
}
 
void PowerUp::draw(sf::RenderWindow& window) const {
    if (sprite) {
        window.draw(*sprite);
    }
}
 
bool PowerUp::checkCollision(sf::FloatRect birdBounds) const {
    if (!sprite) return false;
    sf::FloatRect bounds = sprite->getGlobalBounds();
    return (bounds.position.x < birdBounds.position.x + birdBounds.size.x &&
            bounds.position.x + bounds.size.x > birdBounds.position.x &&
            bounds.position.y < birdBounds.position.y + birdBounds.size.y &&
            bounds.position.y + bounds.size.y > birdBounds.position.y);
}
 
bool PowerUp::isOffScreen() const {
    if (!sprite) return false;
    return sprite->getPosition().x < -50.f;
}

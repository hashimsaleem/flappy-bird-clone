#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <SFML/Graphics.hpp>

inline bool aabbOverlap(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.position.x < b.position.x + b.size.x &&
           a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y &&
           a.position.y + a.size.y > b.position.y;
}

#endif // MATHUTILS_HPP

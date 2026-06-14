#ifndef BIRDSTATE_H
#define BIRDSTATE_H

#include "Config.hpp"

/// Lightweight snapshot of Bird state for pass-through between states.
struct BirdState {
    float posX = Config::BIRD_START_X;
    float posY = Config::BIRD_START_Y;
    float tiltAngle = 0.0f;
    float flapTimer = 0.0f;
    bool isDying = false;
    float velocityY = 0.0f;
};

#endif // BIRDSTATE_H

#ifndef BIRDSTATE_H
#define BIRDSTATE_H

#include "core/ConfigValues.hpp"

/// Lightweight snapshot of Bird state for pass-through between states.
struct BirdState {
    float posX = 50.0f;
    float posY = 300.0f;
    float tiltAngle = 0.0f;
    float flapTimer = 0.0f;
    bool isDying = false;
    float velocityY = 0.0f;
    int difficulty = 1;
};

#endif // BIRDSTATE_H

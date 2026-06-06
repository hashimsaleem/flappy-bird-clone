# Flappy Bird Clone (SFML)

This project is a 2D side-scrolling game inspired by Flappy Bird, built using C++ and the SFML (Simple and Fast Multimedia Library) framework.

## Project Overview
The goal is to create a game where a bird must navigate through gaps in moving pipes.

### Core Components:
- **`main.cpp`**: The game's entry point. It handles window initialization, the main game loop, and input handling (e.g., Space key for jumping).
- **`Bird.hpp` / `Bird.cpp`**: Manages the bird's behavior, including:
    - Physics (gravity and vertical velocity).
    - Jump mechanics.
    - Position tracking and sprite rendering.
- **`Pipe.hpp` / `Pipe.cpp`**: Defines the structure and implementation for the obstacles (pipes).

## Current Development State
The project is in its early stages and serves as a skeletal foundation:

### Completed Features:
- **Basic Game Loop**: Window setup and 60 FPS framerate limit.
- **Bird Physics**: Gravity and jump mechanics are implemented.
- **Input Handling**: Basic detection of the Space key.
- **Pipe Mechanics**: Randomized spawning of pipes with varying heights and gap widths.
- **Collision Detection**: Detection of collisions between the bird and the pipes.
- **Scoring System**: Automatic score tracking as the bird passes pipes.
- **Object Management**: Automated cleanup of off-screen obstacles.
- **Rendering**: Drawing calls for the bird and pipes are implemented in both PLAYING and GAME_OVER states.
- **Game State Management**: START, PLAYING, and GAME_OVER states are implemented with full state transitions and restart mechanic.

### Pending Tasks:
- **Asset Loading**: Replacing `sf::RectangleShape` placeholders with `sf::Sprite` and loading actual textures/images and fonts.
- **Audio**: Adding sound effects for jumping, hitting pipes, and scoring.
- **UI Polish**: Adding "Press Space to Start" text in the START state.

## Technical Stack
- **Language**: C++
- **Library**: SFML (Simple and Fast Multimedia Library)

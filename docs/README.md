# Flappy Bird Clone (SFML)

A 2D side-scrolling game inspired by Flappy Bird, built using C++ and the SFML (Simple and Fast Multimedia Library) framework.

## Project Structure

```
trading/
├── src/                  # Source code
│   ├── CMakeLists.txt    # Build configuration
│   ├── main.cpp          # Game entry point
│   ├── Bird.cpp/hpp      # Bird class
│   ├── Pipe.cpp/hpp      # Pipe class
│   └── Config.hpp        # Game constants
├── assets/               # Game assets
│   ├── bird.png          # Bird sprite
│   └── font.ttf          # Font file
├── docs/                 # Documentation
│   ├── README.md         # This file
│   ├── DEVELOPMENT_ROADMAP.md  # Consolidated roadmap & review findings
│   ├── FIX_PLAN.md       # Fix plan
│   └── REVIEW_FINDINGS.md # Review findings
├── CMakeLists.txt        # Top-level build config
├── build/                # Build output (gitignored)
└── .gitignore
```

## Project Overview
The goal is to create a game where a bird must navigate through gaps in moving pipes.

### Core Components:
- **`src/main.cpp`**: The game's entry point. It handles window initialization, the main game loop, and input handling (e.g., Space key for jumping).
- **`src/Bird.hpp` / `src/Bird.cpp`**: Manages the bird's behavior, including:
    - Physics (gravity and vertical velocity).
    - Jump mechanics.
    - Position tracking and sprite rendering.
- **`src/Pipe.hpp` / `src/Pipe.cpp`**: Defines the structure and implementation for the obstacles (pipes).
- **`src/Config.hpp`**: Centralized game constants (screen dimensions, physics, colors, paths).

## Current Development State
The project is in an advanced state of development with most core mechanics and visual polish implemented.

### Completed Features:
- **Core Game Loop & Physics**: Window setup, 60 FPS framerate limit, gravity, and jump mechanics.
- **Pipe Mechanics**: Randomized spawning, varying heights/gaps, and movement logic.
- **Collision Detection**: Precise detection between the bird and pipes, including ground collision.
- **Scoring System**: Automatic score tracking with visual feedback (+1 floating text).
- **Game State Management**: Full transitions between START, PLAYING, and GAME_OVER states.
- **Audio System**: Sound effects for jumping, scoring, and death; background music with looping and volume control.
- **High Score Persistence**: File-based high score system (`highscore.dat`).
- **Visual Polish & \"Juice\"**: 
    - Bird wing flap animations and spinning death fall.
    - Screen shake on collision/death.
    - Parallax scrolling background (hills).
    - Time-of-day sky color cycling.
    - Dynamic difficulty scaling (speed/spawn rate increases with score).
- **SFML 3.0 Compatibility**: Full migration to the latest SFML API.

### Pending Tasks:
- **Architecture Refactoring**: Extracting game states into a proper State Pattern (classes for `Menu`, `Play`, `GameOver`).
- **Performance Optimization**: Implementing Object Pooling for pipes and particles.
-   **Content Expansion**: Adding power-ups (e.g., shields, slow-motion) and unique level zones.
-   **Advanced Config**: Schema validation for `gameconfig.json` and multiple config profiles.
-   **Testing**: Expanding to include integration-level tests for complex scenarios.

## Building

```bash
# From project root
mkdir -p build && cd build
cmake ..
make
```

Assets are automatically copied to the build directory during the build process.

## Running

```bash
cd build
./FlappyBirdClone
```

## Technical Stack
- **Language**: C++17
- **Library**: SFML 3.0 (Simple and Fast Multimedia Library)
- **Build System**: CMake 3.10+

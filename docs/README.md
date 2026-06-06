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
│   ├── FIX_PLAN.md       # Fix plan
│   ├── IMPROVEMENTS.md   # Code review & roadmap
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
- **SFML 3.0 Compatibility**: Full migration from SFML 2 to SFML 3.0 API.

### Pending Tasks:
- **Audio**: Adding sound effects for jumping, hitting pipes, and scoring.
- **High Score**: Implementing a file-based high score system.
- **Physics Tuning**: Fine-tuning gravity and jump strength constants.
- **Visual Polish**: Adding bird sprite animation and ground collision.

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

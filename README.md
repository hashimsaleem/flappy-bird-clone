# Flappy Bird Clone

A 2D side-scrolling game inspired by Flappy Bird, built with C++23 and SFML 3.0.

## Features

- **Core gameplay**: Gravity, jumping, pipe obstacles, collision detection, scoring
- **Dynamic difficulty**: Speed and spawn rate scale continuously with score (Easy → Normal → Hard → Insane)
- **State management**: Menu, Play, Game Over, High Score screens with `StateFactory`
- **Visual polish**: Parallax clouds, wing-flap animation, screen shake, score bounce, medals (bronze/silver/gold/platinum)
- **Audio**: BGM with volume control (+/-), SFX for jump/score/death
- **Configurable**: `gameconfig.json` for runtime tuning of speed, gaps, spawn interval, etc.
- **Persistent**: High scores saved to `highscore.dat`

## Project Structure

```
trading/
├── src/                    # Source code (39 files)
│   ├── Bird.cpp/hpp        # Bird physics and rendering
│   ├── Pipe.cpp/hpp        # Pipe obstacles
│   ├── PlayState.cpp/h     # Game loop, scoring, difficulty scaling
│   ├── MenuState.h         # Main menu, difficulty, volume controls
│   ├── GameOverState.cpp/h # Game over screen, medals, high score
│   ├── SoundManager.cpp/h  # BGM + SFX management
│   ├── VisualEffectManager.cpp/h  # Visual effects orchestration
│   ├── EnvironmentRenderer.cpp/h      # Parallax sky, hills, clouds, ground
│   ├── CloudSystem.cpp/h   # Drifting cloud layer
│   ├── ParticleSystem.cpp/h    # Emitter-based particles
│   ├── ScoreManager.cpp/h  # Score, floating text, difficulty
│   ├── ObjectPool.h        # Generic object pool for pipes/particles
│   ├── ResourceManager.cpp/h       # Texture/font/sound/music cache
│   ├── ConfigLoader.cpp/h        # JSON config with fallback defaults
│   ├── HighScore.cpp/h     # High score persistence
│   ├── PowerUp.cpp/h       # Invincibility/slow-motion items
│   ├── StateFactory.cpp/h  # State creation and transitions
│   └── main.cpp            # Entry point, window, game loop
├── tests/                  # Unit tests (189 tests, 20 suites)
│   ├── test_Bird.cpp       # 12 tests
│   ├── test_Collision.cpp  # 17 tests
│   ├── test_ConfigLoader.cpp # 12 tests
│   ├── test_HighScore.cpp  # 11 tests
│   ├── test_Integration.cpp # 16 tests
│   ├── test_ObjectPool.cpp # 3 tests
│   ├── test_Pipe.cpp       # 12 tests
│   ├── test_PowerUp.cpp    # 6 tests
│   ├── test_ResourceManager.cpp # 6 tests
│   ├── test_ScoreFloat.cpp # 4 tests
│   ├── test_DifficultyScaling.cpp # 2 tests
│   ├── test_StateFactory.cpp # 4 tests
│   ├── test_CloudSystem.cpp # 6 tests
│   ├── test_ParticleSystem.cpp # 10 tests
│   ├── test_SoundManager.cpp # 12 tests
│   ├── test_VisualEffectManager.cpp # 15 tests
│   ├── test_EnvironmentRenderer.cpp # 5 tests
│   ├── test_MenuState.cpp  # 15 tests
│   ├── test_GameOverState.cpp # 14 tests
│   └── test_HighScoreScreenState.cpp # 8 tests
├── assets/                 # Sprites and fonts
├── docs/                   # Consolidated project log
├── CMakeLists.txt          # Top-level build config
└── .gitignore
```

## Building

### Requirements
- C++23 compiler (GCC 14+, Clang 18+, or MSVC 19.40+)
- CMake 3.10+
- SFML 3.0+ (graphics, window, system, audio modules)

### Build Commands

```bash
# Build game only
mkdir -p build && cd build
cmake ..
make

# Build game + tests
mkdir -p build && cd build
cmake -DBUILD_TESTS=ON ..
make
```

## Running

```bash
cd build
./FlappyBirdClone
```

Assets are automatically copied to the build directory during build.

## Running Tests

```bash
cd build
ctest --output-on-failure
# or directly:
./tests/flappy_tests
```

**189 tests across 20 suites — all passing.**

## Controls

| Key | Action |
|-----|--------|
| Space / Left Click | Jump (Play), Restart (Game Over), Play (Menu) |
| Escape | Pause/Resume (Play), Return to Menu (Game Over) |
| 1 | Play Game (Menu) |
| 2 | Show High Scores (Menu) |
| 3 / 4 / 5 | Easy / Medium / Hard difficulty (Menu) |
| + / - | Increase / Decrease volume (Menu) |
| F1 | Toggle FPS counter |
| F11 | Toggle fullscreen |

## Difficulty Scaling

Speed and spawn interval scale continuously with score using a global lerp:

```
lerp = score / 9999
targetSpeed = 200 + (400 - 200) * lerp
targetInterval = 1.5 - (1.5 - 0.8) * lerp
```

Score zones: Easy (≤50), Normal (≤150), Hard (≤300), Insane (≥301)

## Config

Edit `gameconfig.json` (relative to executable) to override defaults:

```json
{
  "ground_height": 50,
  "pipe_speed": 200.0,
  "pipe_spawn_interval": 1.5,
  "gap_height": 180.0,
  "max_pipe_speed": 400.0,
  "min_spawn_interval": 0.8
}
```

All keys are optional — missing values fall back to `Config.hpp` constants.

## Architecture

The project uses a **State Pattern** for game flow and a **Manager-based** architecture to keep `PlayState` focused on core loop logic.

- **States**: `MenuState` → `PlayState` → `GameOverState` → (back to Menu or Play)
- **Managers**: `SoundManager`, `VisualEffectManager`, `ScoreManager` delegate responsibilities from `PlayState`
- **Object Pooling**: `ObjectPool<Pipe>` and `ObjectPool<Particle>` pre-allocate objects to avoid runtime allocations
- **Resource Caching**: `ResourceManager` singleton caches textures, fonts, sounds, and music

## License

Private project.

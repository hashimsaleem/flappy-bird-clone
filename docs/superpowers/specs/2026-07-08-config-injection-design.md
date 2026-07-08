# ConfigLoader Dependency Injection Design

## Problem

`ConfigLoader` is a static singleton — all methods are `static`, with global mutable state (`data`, `loaded`). This makes it:
- Not testable (tests share global config state)
- Not thread-safe
- Tightly coupled across 17 source files

## Solution: Inject ConfigValues

### New `ConfigValues` struct

A single data struct holding all runtime config values with defaults sourced from existing `Config::` constants:

```cpp
struct ConfigValues {
    // Screen
    int screenWidth = Config::SCREEN_WIDTH;
    int screenHeight = Config::SCREEN_HEIGHT;
    int targetFPS = Config::TARGET_FPS;

    // Bird
    float gravity = Config::GRAVITY;
    float jumpStrength = Config::JUMP_STRENGTH;
    float birdStartX = Config::BIRD_START_X;
    float birdStartY = Config::BIRD_START_Y;
    float birdWidth = Config::BIRD_WIDTH;
    float birdHeight = Config::BIRD_HEIGHT;
    float birdMinTilt = Config::BIRD_MIN_TILT;
    float birdMaxTilt = Config::BIRD_MAX_TILT;
    float birdFlapRate = Config::BIRD_FLAP_RATE;
    float birdFlapDepth = Config::BIRD_FLAP_DEPTH;

    // Pipe / Gameplay
    float pipeSpeed = Config::PIPE_SPEED;
    float gapHeight = Config::GAP_HEIGHT;
    float pipeSpawnInterval = Config::PIPE_SPAWN_INTERVAL;
    float pipeMinY = Config::PIPE_MIN_Y;
    float pipeMaxY = Config::PIPE_MAX_Y;
    float pipeSpeedMax = Config::PIPE_SPEED_MAX;
    float spawnIntervalMin = Config::SPAWN_INTERVAL_MIN;

    // Assets
    std::string assetsDir = "assets/";
    std::string configPath = "gameconfig.json";
    std::string fontPath = "assets/font.ttf";
    std::string birdPath = "assets/bird.png";
    std::string bgMusic = "assets/bgm.ogg";

    // Ground / Physics
    float groundHeight = Config::GROUND_HEIGHT;
    float backgroundSpeed = Config::BACKGROUND_SPEED;

    // Visual
    float screenShakeDuration = Config::SCREEN_SHAKE_DURATION;
    float screenShakeIntensity = Config::SCREEN_SHAKE_INTENSITY;
};
```

### `loadConfig()` free function

```cpp
ConfigValues loadConfig(const std::string& path);
```

A pure function: takes a path, parses JSON, returns `ConfigValues` with JSON values overriding defaults. No static state, no class needed.

### Injection chain

```
main.cpp
  └── loadConfig(exeDir + assetsDir + configPath)
      └── StateFactory::createMenuState(config, bgmMusic, bgmLoaded, highScore, font)
          └── MenuState(config, ...)
          └── PlayState(config, ...)
              ├── Bird(config, ...)
              ├── ScoreManager(config, ...)
              ├── VisualEffectManager(config, ...)
              │   ├── EnvironmentRenderer(config, ...)
              │   └── CloudSystem(config, ...)
              └── Pipe(config, ...) [via pool factory]
          └── GameOverState(config, ...)
```

### ConfigLoader role after

- Kept for backwards compatibility with existing tests
- Static methods remain but are no longer used by source code
- New `loadConfig()` free function is the public API
- Tests can use either `loadConfig()` for new tests, or keep existing static tests

### Files changed

| File | Change |
|------|--------|
| `src/core/ConfigValues.hpp` | **New** — ConfigValues struct + loadConfig() |
| `src/main.cpp` | Use `loadConfig()`, pass ConfigValues through |
| `src/states/StateFactory.h` | Accept `ConfigValues` in factory methods |
| `src/states/MenuState.h` | Store `ConfigValues`, pass to children |
| `src/states/PlayState.h` | Accept `ConfigValues`, pass to Bird/ScoreManager/etc |
| `src/states/GameOverState.h` | Accept `ConfigValues` |
| `src/entities/Bird.hpp/cpp` | Accept `ConfigValues` (or just needed fields) |
| `src/scoring/ScoreManager.hpp/cpp` | Accept `ConfigValues` |
| `src/visual/EnvironmentRenderer.hpp/cpp` | Accept `ConfigValues` |
| `src/visual/CloudSystem.hpp/cpp` | Accept `ConfigValues` |
| `src/systems/VisualEffectManager.hpp/cpp` | Accept `ConfigValues` |
| `tests/` | Update test file constructors to pass ConfigValues |

### Not changed

- `Config::` constants — kept for compile-time defaults and values that aren't runtime-configurable (sky colors, tile dimensions, audio paths, etc.)
- `ConfigLoader` class — kept for existing test compatibility

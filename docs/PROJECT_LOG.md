# Flappy Bird Clone — Development Log

Consolidated project history, code review fixes, refactoring progress, roadmap, and UI/UX improvements.

---

## Project Overview

A 2D side-scrolling game inspired by Flappy Bird, built with C++23 and SFML 3.0.

### Tech Stack
- **Language**: C++23
- **Library**: SFML 3.0 (graphics, audio, window, system)
- **Build System**: CMake 3.10+
- **Testing**: Google Test (189 tests across 20 suites)
- **Config**: JSON-based runtime config via `gameconfig.json`

### Source Files
- `src/` — 39 source files (headers + implementations)
- `tests/` — 20 test files covering all major classes
- `assets/` — Sprite and font assets
- `docs/` — This consolidated document

---

## Architecture

The project uses a **State Pattern** to manage game flow, with a **Manager-based** architecture to keep `PlayState` from becoming a "god object."

### State Flow
```
MenuState → PlayState → GameOverState → (back to MenuState or PlayState)
                ↓
         HighScoreScreenState
```

### Manager Classes
| Manager | Responsibility |
|---------|---------------|
| `SoundManager` | BGM playback, SFX, volume, fading |
| `VisualEffectManager` | Orchestrates EnvironmentRenderer, CloudSystem, ParticleSystem |
| `EnvironmentRenderer` | Parallax sky, hills, clouds, ground |
| `CloudSystem` | 6 drifting cloud entities |
| `ParticleSystem` | Emitter-based particles (dust, collision sparks) |
| `ScoreManager` | Score value, floating score text, difficulty scaling |
| `ObjectPool<Pipe>` | Pre-allocated pipe objects to avoid allocations |
| `ObjectPool<Particle>` | Pre-allocated particle objects |
| `ResourceManager` | Singleton texture/font/sound/music cache |
| `ConfigLoader` | JSON config parsing with fallback defaults |

### Key Classes
| Class | Responsibility |
|-------|---------------|
| `Bird` | Physics (gravity, flap, velocity), sprite rendering |
| `Pipe` | Top/bottom pipe pair, gap geometry, movement |
| `PowerUp` | Invincibility / slow-motion items |
| `BirdState` | Mutable bird state (position, velocity, tilt, dying flag) |
| `ScoreFloat` | Floating "+1" text animation |
| `HighScore` | File-based high score persistence |
| `GameState` | Abstract base for all game states |
| `StateFactory` | Creates state instances, manages transitions |

---

## Completed Features

### Core Mechanics
- [x] Window setup, 60 FPS framerate limit
- [x] Gravity, jump mechanics, velocity damping
- [x] Pipe spawning, randomized heights/gaps, movement
- [x] AABB collision detection (bird ↔ pipes, bird ↔ ground)
- [x] Scoring system with +1 floating text animation
- [x] Dynamic difficulty scaling (speed/spawn interval increase with score)
  - Formula: `lerp = score / 9999`
  - `targetSpeed = 200 + (400 - 200) * lerp`
  - `targetInterval = 1.5 - (1.5 - 0.8) * lerp`
  - Zones: Easy (≤50), Normal (≤150), Hard (≤300), Insane (≥301)

### Game States
- [x] **MenuState** — Play/High Score selection, difficulty (3=Easy/4=Medium/5=Hard), volume (+/-), F11 fullscreen toggle, F1 FPS counter
- [x] **PlayState** — Full gameplay loop with pause (Escape), countdown (3-2-1-GO!)
- [x] **GameOverState** — Score display, medals (bronze ≥20, silver ≥30, gold ≥50, platinum ≥50), high score persistence
- [x] **HighScoreScreenState** — Displays leaderboard, Escape to return

### Audio
- [x] Jump, score, death sound effects
- [x] Background music with looping and volume control
- [x] Volume control in menu (+/- keys, clamped 0–100)

### Visual Polish
- [x] Bird wing-flap animation (procedural scale squish)
- [x] Spinning death fall
- [x] Screen shake on collision/death
- [x] Parallax scrolling background (hills + cloud layer)
- [x] Time-of-day sky color cycling
- [x] Animated score digits (bounce scale on increment)
- [x] Fade transitions between states
- [x] Medal/badge on game over

### Persistence & Config
- [x] `gameconfig.json` for runtime tuning
- [x] `ConfigLoader` with type-safe getters and fallback defaults
- [x] `HighScore` persistence via `highscore.dat`

---

## Unit Test Coverage (189 tests, 20 suites, all passing)

| Suite | Tests | Coverage |
|-------|-------|----------|
| `BirdTest` | 12 | Physics, flap, bounding box, reset, velocity |
| `CollisionTest` | 17 | Gap/no-gap, top/bottom, edges, overlap, moving pipes |
| `ConfigLoaderTest` | 12 | JSON parsing, types, escapes, fallbacks, dot notation |
| `HighScoreTest` | 11 | Persistence, caching, score comparison, negative rejection |
| `IntegrationTest` | 16 | Valid config, missing keys, high score round-trip, pipe creation, collision triggers |
| `ObjectPoolTest` | 3 | Acquire/release, multiple acquisitions, particle pool |
| `PipeTest` | 12 | Movement, speed, off-screen detection, passed flag, large gap |
| `PowerUpTest` | 6 | Position, type, update movement, collision, off-screen |
| `ResourceManagerTest` | 6 | Font/sound/music retrieval and caching |
| `ScoreFloatTest` | 4 | Lifecycle (alive, update, timeout, value) |
| `DifficultyScalingTest` | 2 | Scaling logic, zone transitions |
| `StateFactoryTest` | 4 | Menu/Play/GameOver/HighScore state creation |
| `CloudSystemTest` | 6 | Construction, update, count stability, zero/large dt, bounds |
| `ParticleSystemTest` | 10 | Spawn, update, expiry, accumulation, multiple spawns |
| `SoundManagerTest` | 12 | Volume, fading, SFX playback, stop, bgm not loaded |
| `VisualEffectManagerTest` | 15 | Construction, particles, scroll offsets, sky timer, expiry |
| `EnvironmentRendererTest` | 5 | Construction, draw with scroll offset, sky timer, combined |
| `MenuStateTest` | 15 | Construction, update, difficulty, next action, volume, draw |
| `GameOverStateTest` | 14 | Construction, high score save, overlay fade, key handling, dying bird, particles, draw |
| `HighScoreScreenStateTest` | 8 | Construction, update, next action, key handling, draw, load |

**Total: 189 tests — all passing 🟢**

---

## Code Review Fixes

The following issues were identified during code review and resolved:

### Build & Configuration
| # | File | Fix |
|---|------|-----|
| 7 | `CMakeLists.txt` | Added `CMAKE_CXX_STANDARD 23` for reproducible builds |
| 3 | `.gitignore` | Removed stray `src/build.log` artifact |
| 10 | `tests/CMakeLists.txt`, `src/CMakeLists.txt` | Use `$<TARGET_FILE_DIR:...>` generator expressions so assets land alongside each binary |

### API & Compatibility
| # | File | Fix |
|---|------|-----|
| 4 | `src/PlayState.cpp` | Removed deprecated SFML 3 `pushGLStates()`/`popGLStates()` no-ops |
| 5 | All state files | Pass `sf::Sound*` as pointers instead of copies to avoid redundant copies |

### Correctness
| # | File | Fix |
|---|------|-----|
| 2 | `src/GameOverState.cpp:36` | Use `ConfigLoader::getFloat("ground_height", ...)` instead of hardcoded `Config::GROUND_HEIGHT` |
| 6 | `src/Bird.hpp`, `src/PlayState.cpp` | Remove redundant sprite destroy/recreate in `Bird::reset()` |
| 8 | `src/Pipe.cpp` | Extract repeated AABB overlap test into named `aabbOverlap` helper |
| 9 | `src/ObjectPool.h` | Add optional `std::function<void(T&)>` cleanup callback for `release()` |
| 1 | `src/HighScore.hpp`, `src/HighScore.cpp`, `src/main.cpp` | Add `HighScore::setPath()` so highscore file uses exe-relative path |
| 11 | `src/PlayState.h`, `src/PlayState.cpp` | Pass `assetDir` to preload bird texture with exe-relative path |

---

## Refactoring Progress: God Object Decoupling

### Goal
Reduce coupling by delegating responsibilities from `PlayState` to dedicated Manager classes.

### Completed
- [x] **Visual Effects** → `EnvironmentRenderer`, `CloudSystem`, `ParticleSystem` orchestrated by `VisualEffectManager`
- [x] **Sound Management** → `SoundManager` with BGM, SFX, volume, fading
- [x] **Score Management** → `ScoreManager` handles score value, floating text, difficulty scaling
- [x] **Build Integration** → All manager files included in CMake

### Result
`PlayState` now delegates all visual, audio, and scoring responsibilities. Core loop handles input, state transitions, and game logic coordination.

---

## Development Roadmap

### ✅ Completed
- Core game loop, physics, pipe mechanics, collision, scoring
- SFML 3.0 migration
- Audio system with volume control
- High score persistence
- Visual polish (wing flap, death spin, screen shake, parallax, sky cycling, score bounce)
- State Pattern with `StateFactory`
- Object Pooling for pipes and particles
- ConfigLoader with JSON runtime tuning
- 189 unit tests across 20 suites
- Dynamic difficulty scaling (continuous lerp by score)
- Pause menu, countdown, difficulty selection, medals

### Phase: Next Steps
| Priority | Item | Notes |
|----------|------|-------|
| Medium | Power-ups | Invincibility, slow-motion bubbles |
| Medium | Level zones | Fast zone, tight gap zone with unique patterns |
| Medium | Advanced config | Nested JSON objects, multiple profiles (`easy.json`, `hard.json`) |
| Low | Smooth state transitions | Fade in/out effects between states |
| Low | Advanced particles | Ground dust, collision sparks, varied effect types |
| Low | Menu polish | Logo/title with bob animation, bird preview background |
| Low | Integration tests | File permission errors, complex config edge cases |
| Low | Contributor guide | Build docs, architecture overview for new contributors |
| Low | Release | Tag final version on a release branch |

---

## UI/UX Checklist

### Visual Polish
- [x] Bird wing-flap animation (scale squish on flap)
- [x] Spinning death fall
- [x] Screen shake on collision/death
- [x] Parallax sky background (drifting cloud layer)
- [x] Animated score digits (bounce on increment)
- [x] Fade transitions between states
- [x] Medal/badge on game over (bronze/silver/gold/platinum)
- [x] Time-of-day sky color cycling

### Gameplay UX
- [x] Pause menu (Escape key, resume/quit)
- [x] Countdown before game starts (3-2-1-GO!)
- [x] Difficulty selection (3=Easy, 4=Medium, 5=Hard)
- [x] Blinking "Press 1 to Play" hint on menu
- [x] F11 fullscreen toggle
- [x] F1 FPS counter (top-right corner)

### Audio
- [x] Volume control (+/- keys in menu, applied to BGM)
- [x] Softer flap sound (60% volume)

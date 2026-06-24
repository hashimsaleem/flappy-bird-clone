# Development Roadmap & Review Findings

This document consolidates all code review findings, improvement suggestions, and next steps for the Flappy Bird Clone project.

---

## 📊 Project Status Summary

The project is currently in a **"Polishing & Refinement"** phase. Most core mechanics (physics, collision, audio, high scores) are complete, and significant visual polish ("Juice") has been implemented.

### ✅ Completed Features
- **Core Mechanics**: Window setup, 60 FPS framerate limit, gravity, jump mechanics, pipe spawning/movement, collision detection, scoring system.
- **SFML 3.0 Migration**: Full compatibility with SFML 3.0 API (Sprite handling, Image creation, Texture loading, etc.).
- **Audio System**: Jump, score, and death sound effects; background music with looping and volume control.
- **High Score Persistence**: File-based high score system (`highscore.dat`).
- **Visual Polish ("Juice")**: 
    - Bird wing flap animations and spinning death fall.
    - Screen shake on collision/death.
    - Parallax scrolling background (hills).
    - Time-of-day sky color cycling.
    - Dynamic difficulty scaling (speed/spawn rate increases with score).
- **Configurability**: `Config.hpp` for constants and `gameconfig.json` for runtime tuning of speed, spawn intervals, and caps.
- **Unit Testing**: 64 tests across 5 suites (Google Test) covering ConfigLoader, HighScore, Bird, Pipe, and Collision logic.

---

## 🔺 Resolved Issues (Code Review)

The following issues were identified during review and have been successfully addressed:

| # | File | Issue | Fix Applied |
|---|------|-------|-------------|
| 1 | `Bird.cpp` | `load()` method was a placeholder. | ✅ Implemented with `sf::Sprite` + `sf::Texture`. |
| 2 | `main.cpp` | Double `gameClock.restart()`. | ✅ Removed redundant call. |
| 3 | `Pipe.hpp` | Dead `GAP_HEIGHT` constant. | ✅ Removed; constructor parameter now used. |
| 4 | `Pipe.cpp` | Hardcoded `velocityX`. | ✅ Moved to `Config::PIPE_SPEED`. |
| 5 | **All** | SFML 3.0 API incompatibility. | ✅ Full migration completed. |
| 6 | `Bird.cpp` | Bird X position hardcoded in `update()`. | ✅ Made configurable via `Config`. |
| 7 | `main.cpp` | No "Press Space to Start" prompt. | ✅ Implemented in START state. |
| 8 | `main.cpp` | Hardcoded asset paths. | ✅ Moved to `Config` constants. |
| 9 | `Pipe.cpp` | Hardcoded pipe colors. | ✅ Moved to `Config` colors. |
| 10 | `main.cpp` | Debug logs in production code. | ✅ Wrapped in `#ifdef DEBUG`. |
| 11 | `Bird.cpp` | Physics variables not `constexpr`. | ✅ Refactored into `Config` namespace. |
| 12 | `main.cpp` | Missing framerate limit check. | ✅ Updated for SFML 3+ compatibility. |
| 13 | `main.cpp` | No visual distinction on Game Over. | ✅ Added semi-transparent overlay and restart prompt. |
| 14 | `main.cpp` | Hardcoded Game Over text color. | ✅ Moved to `Config`. |

---

## 🚀 Future Roadmap & Next Steps

The following items are prioritized for the next development cycles:

### Phase 1: Architecture & Optimization (High Priority)
- **State Pattern Refactor**: Extract game states (`Menu`, `Play`, `GameOver`) into dedicated classes. This will clean up `main.cpp` and make it easier to add new screens (e.g., Options, Pause).
- **Object Pooling**: Implement a pool for pipes and particles to optimize memory usage and prevent fragmentation as the game scales.

### Phase 2: Content & Gameplay Expansion (Medium Priority)
- **Power-ups**: Develop items like "Invincibility" or "Slow-motion" bubbles.
- **Level Zones**: Create specific areas with unique pipe patterns (e.g., a "fast zone" or "tight gap zone").
- **Advanced Config**: Expand `ConfigLoader` to support nested JSON objects or multiple profiles (`easy.json`, `hard.json`).

### Phase 3: Visual & UX Polish (Low Effort, High Payoff)
- **Smooth Transitions**: Add fade-in/fade-out effects between game states.
- **Advanced Particle Systems**: Create varied effects like dust from the ground or sparks on collision.
- **Menu System Polish**: Add a game logo/title with bob animation and a preview of the bird in the background.

### Phase 4: Testing & Deployment
- **Integration Tests**: Write tests for real-world scenarios (e.g., file permission errors, complex config edge cases).
- **Contributor Guide**: Expand documentation to help others build and contribute.
- **Release**: Prepare a release branch or tag for the final version.

---

## 🧪 Unit Test Coverage Summary
- `test_ConfigLoader`: 12 tests (JSON parsing, types, escapes, fallbacks)
- `test_HighScore`: 11 tests (persistence, caching, score comparison)
- `test_Bird`: 12 tests (gravity, flap, bounding box, reset)
- `test_Pipe`: 12 tests (movement, speed, off-screen detection)
- `test_Collision`: 17 tests (gap/no-gap, top/bottom hits, edge overlap)
**Total: 64 Tests - All Passing 🟢**

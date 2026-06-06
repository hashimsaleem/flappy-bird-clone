# Code Review: Flappy Bird Clone (SFML / C++)

**Review Date:** 2026-06-07  
**Project:** Flappy Bird Clone (CMake + C++17 + SFML 3.0)  
**Files Reviewed:** All 13 source files, assets, config, build system

---

## Summary

A fully playable, well-structured Flappy Bird clone with audio, particle effects, dynamic difficulty, parallax scrolling, bird animation, an interactive menu, and runtime JSON config. The project has evolved significantly from its initial skeleton into a polished game.

---

## ✅ What's Done Well

| Aspect | Score | Notes |
|--------|-------|-------|
| **No magic numbers** | 🟢 | `Config.hpp` centralizes every constant |
| **Runtime config** | 🟢 | `ConfigLoader` reads `gameconfig.json` to override defaults |
| **Resource caching** | 🟢 | `ResourceManager` prevents redundant file I/O |
| **Dynamic difficulty** | 🟢 | Speed & spawn rate scale every 5 points (with caps) |
| **Moving pipes** | 🟢 | 20% chance per spawn for sine-wave oscillating pipes |
| **Sound effects** | 🟢 | Jump, score, death sounds + looping BGM |
| **Bird animation** | 🟢 | Velocity-based tilt lerp + wing flap oscillation |
| **Death effects** | 🟢 | Spin animation + particle burst |
| **Parallax background** | 🟢 | Scrolling green hills layer |
| **High score persistence** | 🟢 | Binary file with in-memory caching |
| **Graceful fallbacks** | 🟢 | Silent WAV synthesis for missing audio, default font fallback |
| **UI/Menu** | 🟢 | Interactive menu, START prompt, GAME_OVER overlay, high score screen |

---

## 🔴 Critical Issues

### 1. `Bird` has a raw `sf::Sprite*` with manual `new`/`delete` — violates Rule of Three/Five

**File:** `Bird.hpp`, `Bird.cpp`  
**Risk:** If `Bird` is ever copied (even accidentally), double-free + crash.  
**Current workaround:** `bird.reset()` is called instead of `bird = Bird()` on restart, which avoids the copy. But the type remains copy-unsafe.

```cpp
// Bird.hpp
sf::Sprite* sprite = nullptr; // raw pointer — no copy control
```

### 2. `fontLoaded` flag is dead code — always `true`

**File:** `main.cpp:38-41`  
**Problem:** `ResourceManager::getFont()` never fails (it falls back to a default `sf::Font()`). The `if (!fontLoaded)` branch is unreachable, and all `if (fontLoaded)` guards throughout are always-true noise.

### 3. `rand()` used inconsistently

**File:** `main.cpp:155, 184`  
**Problem:** The rest of the code uses `<random>` (`std::default_random_engine` + distributions), but particle spawning uses legacy C `rand()`. Worse distribution, not thread-safe.

---

## 🟡 Medium Issues

### 4. Duplicate game-over trigger logic (~20 lines repeated)

**File:** `main.cpp` (lines ~161-176 and ~181-197)  
**Impact:** Both the boundary collision and pipe collision trigger nearly identical code. If a new game-over behavior is added (e.g., sound, animation), both blocks need updating.

### 5. `ResourceManager::getFont()` returns by value (copies the entire font)

**File:** `ResourceManager.hpp:12`  
**Impact:** `sf::Font` is a heavyweight object. Every caller copies it from the cache. Should return `const sf::Font&` like `getTexture()` does.

### 6. Scoring threshold hardcoded to `50.0f`

**File:** `main.cpp:191`  
**Impact:** `if (!pipe.passed && pipe.getX() < 50.0f)` assumes the bird is always at X=50. If the bird's X position ever changes, scoring breaks silently. Should compare against the bird's actual bounding box.

### 7. Difficulty caps are hardcoded in main.cpp

**File:** `main.cpp:199-200`  
**Impact:** `currentPipeSpeed > 400.f` and `currentSpawnInterval < 0.8f` are magic numbers buried in the game loop. Should live in `Config.hpp` for tunability.

---

## 🟢 Low Issues

### 8. Debug `std::cout` spam in production code

**File:** `main.cpp` (score changes, difficulty changes, game-over messages)  
**Suggestion:** Wrap in `#ifdef DEBUG` or remove for release builds.

### 9. `menuOption` wraps hardcoded `% 3`

**File:** `main.cpp:97-98`  
**Suggestion:** Use `options.size()` instead of the magic number 3.

### 10. Particle alpha calculation

**File:** `Particle.hpp:25`  
**Note:** Using `alpha = lifetime * 255.f` works but is functionally correct for the 1-second particle lifetime.

---

## 📊 Scorecard

| Category | Rating | Notes |
|----------|--------|-------|
| Correctness | 🟡 7/10 | Works, but raw-pointer Bird is risky; dead fontLoaded check |
| Architecture | 🟢 8/10 | Well-modularized, clear separation, good use of Config |
| Code Style | 🟢 8/10 | Clean, consistent, good comments |
| Build System | 🟢 9/10 | CMake with proper asset copying |
| Robustness | 🟡 7/10 | Good fallbacks, but copy-unsafe Bird type |
| Feature Completeness | 🟢 9/10 | Music, particles, difficulty, animation, menu, config |
| Performance | 🟢 9/10 | Cached resources, no obvious leaks |

---

## Fixes Applied

| # | Issue | Fix |
|---|-------|-----|
| 1 | Bird copy-unsafe raw pointer | Deleted copy constructor & copy assignment operator; bird X getter added |
| 2 | `fontLoaded` dead code | Removed variable, dead branch, and all `if (fontLoaded)` guards |
| 3 | `rand()` inconsistency | Replaced with `std::uniform_int_distribution` |
| 4 | Duplicate game-over logic | Extracted into a local lambda `triggerGameOver` |
| 5 | `getFont()` returns by value | Changed return type to `const sf::Font&` |
| 6 | Hardcoded scoring threshold | Now compares against `bird.getX()` |
| 7 | Hardcoded difficulty caps | Moved to `Config::PIPE_SPEED_MAX` and `Config::SPAWN_INTERVAL_MIN` |
| 8 | Debug cout spam | Wrapped in `#ifdef DEBUG` / `#endif` |
| 9 | `menuOption % 3` | Changed to `options.size()` |

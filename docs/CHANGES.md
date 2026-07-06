# Code Review Fixes & Improvements

This document lists all changes made following the code review on 2026-06-25.

---

## #2 — GameOverState respects runtime config for ground height

**File:** `src/GameOverState.cpp:36`  
**Change:** `Config::GROUND_HEIGHT` → `ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT)`  
**Why:** GameOverState hardcoded the compile-time ground height constant, while PlayState
used the runtime JSON override. If `gameconfig.json` changed `ground_height`, the death
animation would stop the bird at the wrong Y position.

---

## #3 — Remove stray `src/build.log` from tracking

**File:** `.gitignore`  
**Change:** Added `src/build.log` to `.gitignore`  
**Why:** The file `src/build.log` (containing a stray `make: install` error) was tracked
in the repo but is a build artifact with no value.

---

## #4 — Remove deprecated SFML 3 API calls

**File:** `src/PlayState.cpp`  
**Change:** Removed `pushGLStates()` and `popGLStates()` wrappers in `PlayState::draw()`.  
**Why:** These are no-ops in SFML 3 (OpenGL state management is handled internally).
They were retained from the SFML 2 era but serve no purpose now.

---

## #6 — Avoid redundant sprite destroy + recreate in `Bird::reset()`

**File:** `src/Bird.hpp`, `src/PlayState.cpp`  
**Change:** `Bird::reset()` no longer calls `sprite.reset()`. `PlayState::onEnter()`
no longer calls `bird.load()` after `bird.reset()`.  
**Why:** `Bird::reset()` destroyed the sprite only for the subsequent `bird.load()` call
to recreate it. Since the texture doesn't change between state transitions, the
sprite can survive `reset()`. This eliminates one redundant file I/O and texture load
per game restart.

---

## #7 — Set explicit C++17 standard in CMake

**File:** `CMakeLists.txt`  
**Change:** Added `set(CMAKE_CXX_STANDARD 17)` and `set(CMAKE_CXX_STANDARD_REQUIRED ON)`.  
**Why:** The code uses C++17 features (`std::filesystem`, structured bindings, etc.)
but no standard was declared. Different compilers default to different standards,
which could cause build failures on some toolchains.

---

## #8 — Extract AABB collision helper

**File:** `src/Pipe.cpp`  
**Change:** Extracted the repeated AABB overlap test into a static helper function
`aabbOverlap`.  
**Why:** `Pipe::checkCollision()` duplicated the same four-way AABB test for top
and bottom pipes. A named helper improves readability and eliminates repetition.

---

## #1 — HighScore path uses exe-relative path

**Files:** `src/HighScore.hpp`, `src/HighScore.cpp`, `src/main.cpp`  
**Change:** Added `HighScore::setPath()` to override the hardcoded `highscore.dat`.
`main.cpp` now calls `HighScore::setPath(exeDir + "highscore.dat")` before loading.  
**Why:** The highscore file was always saved to CWD, which is inconsistent with config
loading (which uses `exeDir`). Running the binary from outside the project root would
save scores to the wrong location.

---

## #5 — Pass sound objects as pointers, not copies

**Files:** `src/StateFactory.h`, `src/StateFactory.cpp`, `src/MenuState.h`,
`src/PlayState.h`, `src/PlayState.cpp`, `src/main.cpp`, `tests/test_StateFactory.cpp`  
**Change:** State constructors and factory methods now take `sf::Sound*` instead of
`sf::Sound` by value. States store pointers to the ResourceManager-owned originals.  
**Why:** Every state transition was copying 3 `sf::Sound` objects by value. With pointers,
the originals are shared directly — zero copies, and volume/settings changes persist
naturally across states.

---

## #9 — ObjectPool release cleanup callback

**File:** `src/ObjectPool.h`  
**Change:** Added an optional `std::function<void(T&)> cleanup` parameter to the
constructor, invoked in `release()` before marking the slot as free.  
**Why:** Pooled objects might hold resources beyond simple SFML types. The cleanup
callback ensures pooled items can release resources (open files, GPU handles, etc.)
when returned to the pool, preventing leaks in future use cases.

---

## #10 — Copy assets to test binary directory

**File:** `tests/CMakeLists.txt`, `src/CMakeLists.txt`  
**Change:** Both CMake custom commands now use `$<TARGET_FILE_DIR:...>` generator
expressions instead of hardcoded paths, so assets land alongside each binary.  
**Why:** Tests failed when run from the `build/tests/` directory because assets were
copied to `CMAKE_BINARY_DIR` (`build/`) instead of the test binary's own directory.
The game binary also now resolves assets relative to itself rather than a hardcoded
`build/assets/` path.

---

## #11 — Bird texture loaded with exe-relative path

**Files:** `src/PlayState.h`, `src/PlayState.cpp`, `src/StateFactory.h`,
`src/StateFactory.cpp`, `src/main.cpp`, `tests/test_StateFactory.cpp`  
**Change:** `PlayState` now accepts an `assetDir` parameter and prepends it to
`Config::BIRD_PATH` when loading the bird texture.  
**Why:** `bird.load()` was the only asset load using a bare relative path (`"assets/bird.png"`),
while every other resource (fonts, sounds, music, config) used `exeDir` prefix from
`main.cpp`. This caused a crash when the CWD differed from the executable directory
because `std::filesystem::canonical()` (used internally by SFML 3) failed on the
unresolved relative path.

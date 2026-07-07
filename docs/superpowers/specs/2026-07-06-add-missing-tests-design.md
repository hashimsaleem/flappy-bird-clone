# Test Expansion: Untested Classes

## Goal
Add behavioral tests for all 8 untested source classes in the Flappy Bird Clone project.
Current coverage: 95 tests running (PowerUp tests exist but are not registered in CMake and are broken).
Target: ~125-130 tests.

## Current Coverage

### Fully Tested (95 tests)
- Bird (12), Pipe (12), Collision (17), ConfigLoader (12)
- HighScore (11), ObjectPool (3), ScoreFloat (4), PowerUp (4)
- ResourceManager (6), Integration (16), StateFactory (4), DifficultyScaling (2)

### Not Tested (8 classes)
- **PowerUp** — tests exist but call non-existent `getX()`/`getY()` methods
- **CloudSystem** — parallax cloud layer
- **ParticleSystem** — pool-backed particle spawning/lifetime
- **SoundManager** — BGM volume, fade-in/out, SFX
- **VisualEffectManager** — composite wrapper (CloudSystem + ParticleSystem + EnvironmentRenderer)
- **EnvironmentRenderer** — sky gradient, ground tiles, grass
- **MenuState** — main menu (difficulty, volume, selection)
- **GameOverState** — game-over screen (keys, high-score save)
- **HighScoreScreenState** — simple high-score display (ESC to return)

## Approach
- One test file per class (or fix existing test)
- Follow `TEST_F` pattern used throughout the project
- Use dummy SFML objects (sf::Music, sf::Font) — they are handles, no window needed
- Headless-safe: tests check state, not rendered pixels
- No additional mocking frameworks

## Required Source Changes

### 1. PowerUp (fix existing tests)
Add two accessors to `PowerUp.hpp`:
```cpp
float getX() const { return sprite ? sprite->getPosition().x : 0.f; }
float getY() const { return sprite ? sprite->getPosition().y : 0.f; }
```

### 2. SoundManager (add volume getter)
Add to `SoundManager.hpp`:
```cpp
float getCurrentBGMVolume() const { return currentBGMVolume; }
```
Needed to verify fade interpolation in tests.

## Test Plan

### PowerUp (fix existing) — 5 tests
File: `tests/test_PowerUp.cpp` (already exists, broken)
1. `StartsAtCorrectPosition` — verify getX()/getY() return constructor values
2. `HasCorrectType` — verify getType() returns correct enum
3. `UpdateMovesWithVelocity` — call update(dt), verify x changes by velocityX * dt
4. `CheckCollisionWorks` — test overlap and non-overlap cases
5. `IsOffScreenWorks` — test left-off-screen and on-screen positions

### CloudSystem — 3 tests
File: `tests/test_CloudSystem.cpp`
1. `InitializesWithClouds` — verify 6 clouds created after constructor
2. `UpdateAdvancesCloudOffset` — call update(dt) N times, verify offset = BACKGROUND_SPEED * totalDt
3. `UpdateWithZeroDtDoesNotChangeOffset` — dt=0 should not advance offset

### ParticleSystem — 4 tests
File: `tests/test_ParticleSystem.cpp`
1. `InitialParticleCountIsZero` — new system has no particles
2. `SpawnCreatesParticles` — spawn(10), getParticles().size() == 10
3. `UpdateExpiresDeadParticles` — spawn, call update(2.0f), verify count drops to 0
4. `SpawnMultipleTimesAddsAll` — spawn twice, verify total count

### SoundManager — 4 tests
File: `tests/test_SoundManager.cpp`
1. `InitialVolumeIsBGMVolume` — constructor copies from sf::Music
2. `SetBGMVolumeUpdatesState` — setVolume(50), getCurrentBGMVolume() == 50
3. `FadeBGMStartsFade` — fadeBGM(0, 1.0f) starts fade timer
4. `UpdateDuringFadeProgresses` — setVolume(100), fadeBGM(0, 2.0f), call update(0.5f), verify volume decreased

### VisualEffectManager — 4 tests
File: `tests/test_VisualEffectManager.cpp`
1. `InitializesAllSubsystems` — constructor does not crash, all members non-null
2. `SetGroundScrollOffsetChangesState` — setGroundScrollOffset(50), verify
3. `SetCloudOffsetChangesState` — setCloudOffset(50), verify
4. `SetSkyTimerChangesState` — setSkyTimer(10), verify

### EnvironmentRenderer — 2 tests
File: `tests/test_EnvironmentRenderer.cpp`
1. `ConstructorDoesNotCrash` — new EnvironmentRenderer is valid
2. `DrawDoesNotCrash` — draw with dummy window does not crash (minimal window for bounds)

### MenuState — 6 tests
File: `tests/test_MenuState.cpp`
1. `InitialState` — selectedOption=-1, difficulty=1, volume=50
2. `KeyPressNum3SetsDifficulty` — key=3 → difficulty=0 (Easy)
3. `KeyPressNum4SetsDifficulty` — key=4 → difficulty=1 (Medium)
4. `KeyPressNum5SetsDifficulty` — key=5 → difficulty=2 (Hard)
5. `KeyPressPlusIncreasesVolume` — key=+ → volume increases by 10
6. `KeyPressMinusDecreasesVolume` — key=- → volume decreases by 10

### GameOverState — 5 tests
File: `tests/test_GameOverState.cpp`
1. `InitialState` — overlayAlpha=0, nextActionCode=None
2. `KeyPressSpaceTriggersPlayGame` — Space → nextAction=PlayGame
3. `KeyPressEscapeTriggersReturnToMenu` — Escape → nextAction=ReturnToMenu
4. `HighScoreUpdatedWhenExceeded` — score > highScore → highScore updated
5. `HighScoreNotUpdatedWhenNotExceeded` — score < highScore → unchanged

### HighScoreScreenState — 2 tests
File: `tests/test_HighScoreScreenState.cpp`
1. `InitialState` — nextAction=none
2. `KeyPressEscapeReturnsToMenu` — ESC → nextAction=ReturnToMenu

## Test File Registration
Add 8 new test sources to `tests/CMakeLists.txt`:
```cmake
set(TEST_SOURCES
    ...
    test_CloudSystem.cpp
    test_ParticleSystem.cpp
    test_SoundManager.cpp
    test_VisualEffectManager.cpp
    test_EnvironmentRenderer.cpp
    test_MenuState.cpp
    test_GameOverState.cpp
    test_HighScoreScreenState.cpp
)
```

## Success Criteria
- All 125-130 tests pass
- No existing tests break
- Build completes without warnings

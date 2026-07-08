# Phase 1: Audio Foundation Design

## Objective
Implement working sound effects and BGM fade-in/out for the Flappy Bird game. Currently, SFX methods are empty stubs and BGM fade infrastructure exists but is never used.

## Current State
- `SoundManager::playJump()`, `playScore()`, `playDeath()` are empty stubs
- SFX pool exists: 8 `sf::Sound` slots backed by `ResourceManager` sound buffer cache
- Asset files present: `assets/jump.ogg`, `assets/score.ogg`, `assets/death.ogg`
- BGM fade fields exist in `SoundManager` (`fadeTimer`, `targetVolume`, `startVolume`) but `update()` never processes them
- `PlayState` calls `soundManager->playDeath()` on death (does nothing)
- `MenuState` creates `SoundManager` but never calls `update()` on it
- No calls to `fadeBGM()` exist anywhere

## Design

### 1. SoundManager SFX Methods
**File**: `src/audio/SoundManager.cpp`

Each method follows the same pattern:
1. Iterate the `_sfxPool` (8 `sf::Sound` slots)
2. Find the first sound that is not currently playing
3. Set its buffer via `ResourceManager::getSound(path)`
4. Call `play()` on the sound

```cpp
void SoundManager::playJump() { playSfx("assets/jump.ogg"); }
void SoundManager::playScore() { playSfx("assets/score.ogg"); }
void SoundManager::playDeath() { playSfx("assets/death.ogg"); }
```

The helper `playSfx(path)` is a new private method that encapsulates the pool search + play logic.

### 2. BGM Fade Processing
**File**: `src/audio/SoundManager.cpp`

Add fade processing to `SoundManager::update()`:
- If `fadeActive` is true, decrement `fadeTimer` by `dt`
- If timer > 0, lerp current volume from `startVolume` toward `targetVolume` using `fadeTimer / fadeDuration`
- If timer <= 0, set volume to `targetVolume`, set `fadeActive = false`

```cpp
void SoundManager::update(float dt) {
    if (fadeActive) {
        fadeTimer -= dt;
        float progress = std::max(0.f, fadeTimer / fadeDuration);
        float vol = startVolume + (targetVolume - startVolume) * (1.f - progress);
        bgmMusic.setVolume(vol);
        if (fadeTimer <= 0.f) {
            bgmMusic.setVolume(targetVolume);
            fadeActive = false;
        }
    }
}
```

### 3. Fade Calls in PlayState
**File**: `src/states/PlayState.cpp`

- **On game start** (`onEnter`): `soundManager->fadeBGM(40, 0.5f)` — fade in from current to 40% over 0.5s
- **On death** (`triggerGameOver`): `soundManager->fadeBGM(20, 1.0f)` — fade to 20% over 1s (while bird falls)
- **On return to menu** (`quitToMenu`): `soundManager->stopBGM()` — stop music when leaving game

### 4. MenuState SoundManager Update
**File**: `src/states/MenuState.h`

In `MenuState::update()`, add `soundManager->update(dt)` so BGM fades work in the menu (e.g., if returning from game).

### File Changes Summary
| File | Changes |
|------|---------|
| `src/audio/SoundManager.cpp` | Implement SFX methods, add fade processing to `update()` |
| `src/states/PlayState.cpp` | Add fade calls in `onEnter()`, `triggerGameOver()`, `quitToMenu` |
| `src/states/MenuState.h` | Add `soundManager->update(dt)` in `update()` |

## Testing
- SFX plays on jump, score, death
- BGM fades in on game start, fades out on death
- BGM continues playing in menu
- No crashes or double-play issues

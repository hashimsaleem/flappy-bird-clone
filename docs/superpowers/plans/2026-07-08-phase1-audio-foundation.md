# Phase 1: Audio Foundation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Implement working SFX (jump, score, death) and BGM fade-in/fade-out for the Flappy Bird game.

**Architecture:** SoundManager already has an SFX pool (8 sf::Sound slots) and fade infrastructure. We wire up `playSfx()` to use the pool, initialize the pool in the constructor, implement the 3 SFX methods, and add fade calls in PlayState/MenuState.

**Tech Stack:** C++23, SFML 3, Google Test

## Global Constraints
- SFML 3 API: mouse position is `event->position` (Vector2i), not separate x/y
- SoundManager SFX pool is `std::unique_ptr<sf::Sound> sfxPool[MAX_SFX_COUNT]` with MAX_SFX_COUNT = 8
- ResourceManager is a singleton with `getSound(path)` that returns a fallback for missing files
- All SFX paths use relative paths from exe directory (e.g., "assets/jump.ogg")
- BGM fade uses exponential interpolation: `current = current + (target - current) * progress`
- Assets directory: `assets/` contains `jump.ogg`, `score.ogg`, `death.ogg`

---

### Task 1: Initialize SFX Pool + Implement playSfx

**Files:**
- Modify: `src/audio/SoundManager.cpp:5-7`

**Interfaces:**
- Consumes: None
- Produces: `playSfx()` that searches for an available sf::Sound slot, sets its buffer, and plays it

**Steps:**

- [ ] **Step 1: Initialize SFX pool in constructor**

Replace the constructor body in `src/audio/SoundManager.cpp` to create 8 sf::Sound objects:

```cpp
SoundManager::SoundManager(sf::Music& bgmMusic, bool bgmLoaded)
    : bgmMusic(bgmMusic), bgmLoaded(bgmLoaded) {
    for (int i = 0; i < MAX_SFX_COUNT; i++) {
        sfxPool[i] = std::make_unique<sf::Sound>();
    }
}
```

- [ ] **Step 2: Implement playSfx() helper**

Replace the empty `playSfx` method with this:

```cpp
void SoundManager::playSfx(const std::string& assetPath) {
    for (int i = 0; i < MAX_SFX_COUNT; i++) {
        if (!sfxPool[i]->getStatus() == sf::Sound::Status::Playing) {
            auto& buffer = ResourceManager::getInstance().getSound(assetPath);
            sfxPool[i]->setBuffer(buffer.get());
            sfxPool[i]->play();
            return;
        }
    }
}
```

Wait — `ResourceManager::getSound()` returns `sf::Sound&`, not `sf::SoundBuffer*`. Let me check the correct API.

Looking at `ResourceManager::getSound()`, it returns `sf::Sound&` and the buffer is stored in `soundBuffers[path]` as `std::shared_ptr<sf::SoundBuffer>`. We need to set the buffer on `sf::Sound` via `setBuffer()`. The ResourceManager stores `sf::SoundBuffer` pointers in `soundBuffers` map.

The correct approach: use `ResourceManager::soundBuffers` via a getter or directly access the buffer. Actually, `getSound()` returns `sf::Sound&`, and we can call `getBuffer()` on it.

Corrected playSfx:

```cpp
void SoundManager::playSfx(const std::string& assetPath) {
    for (int i = 0; i < MAX_SFX_COUNT; i++) {
        if (sfxPool[i]->getStatus() != sf::Sound::Status::Playing) {
            sf::Sound& ref = ResourceManager::getInstance().getSound(assetPath);
            sfxPool[i]->setBuffer(ref.getBuffer());
            sfxPool[i]->play();
            return;
        }
    }
}
```

- [ ] **Step 3: Implement SFX methods**

Replace the empty SFX methods:

```cpp
void SoundManager::playJump() { playSfx("assets/jump.ogg"); }
void SoundManager::playScore() { playSfx("assets/score.ogg"); }
void SoundManager::playDeath() { playSfx("assets/death.ogg"); }
```

- [ ] **Step 4: Verify compilation**

```bash
cmake --build build
```

Expected: Clean build, only nodiscard warning for bird texture (pre-existing).

- [ ] **Step 5: Commit**

```bash
git add src/audio/SoundManager.cpp
git commit -m "feat: implement SoundManager SFX pool and fade infrastructure"
```

---

### Task 2: Wire BGM Fade Calls in PlayState

**Files:**
- Modify: `src/states/PlayState.cpp:32`, `src/states/PlayState.cpp:86-102`, `src/states/PlayState.cpp:56-78`

**Interfaces:**
- Consumes: `soundManager->fadeBGM(targetVolume, duration)`, `soundManager->stopBGM()`
- Produces: BGM fades in on game start, fades out on death, stops on quit to menu

**Steps:**

- [ ] **Step 1: Add BGM fade-in on game start**

In `PlayState` constructor, after `bird.load(...)`, add:
```cpp
soundManager->fadeBGM(40.f, 0.5f);
```
This sets the fade target to 40% (matching what main.cpp sets) over 0.5s.

- [ ] **Step 2: Add BGM fade-out on death**

In `triggerGameOver()` (around line 100, after `soundManager->playDeath()`), add:
```cpp
soundManager->fadeBGM(20.f, 1.0f);
```

- [ ] **Step 3: Add BGM stop on quit to menu**

In `handleKeyPress()` where `quitToMenu = true` (around line 74-75), add:
```cpp
soundManager->stopBGM();
```

- [ ] **Step 4: Verify compilation**

```bash
cmake --build build
```

- [ ] **Step 5: Commit**

```bash
git add src/states/PlayState.cpp
git commit -m "feat: wire BGM fade-in on game start and fade-out on death"
```

---

### Task 3: Add SoundManager Update to MenuState

**Files:**
- Modify: `src/states/MenuState.h:32-43`

**Interfaces:**
- Consumes: `soundManager->update(dt)`
- Produces: BGM fade processing while in menu (e.g., if returning from game)

**Steps:**

- [ ] **Step 1: Add update call to soundManager**

In `MenuState::update()`, add `soundManager->update(dt)` at the start:

```cpp
void update(float dt) override {
    soundManager->update(dt);
    skyTimer += dt;
    // ... rest unchanged
}
```

- [ ] **Step 2: Verify compilation**

```bash
cmake --build build
```

- [ ] **Step 3: Commit**

```bash
git add src/states/MenuState.h
git commit -m "fix: call SoundManager::update() in MenuState for BGM fade support"
```

---

### Task 4: Verify Build and Runtime

**Files:**
- No file changes

**Steps:**

- [ ] **Step 1: Full rebuild**

```bash
rm -rf build && cmake -B build -S . && cmake --build build
```

Expected: Clean build.

- [ ] **Step 2: Quick smoke test**

```bash
cd /tmp && timeout 3 /home/hashim/trading/build/FlappyBirdClone 2>&1
```

Expected: Exit code 124 (ran for 3s then killed), no crash, no errors.

- [ ] **Step 3: Commit**

```bash
git commit --allow-empty -m "verify: Phase 1 audio foundation passes smoke test"
```

---

**Total tasks:** 4
**Estimated time:** ~15 minutes

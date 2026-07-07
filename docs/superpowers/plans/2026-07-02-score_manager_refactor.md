# ScoreManager Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Delegate scoring and difficulty scaling logic from `PlayState` to a dedicated `ScoreManager` class.

**Architecture:** Create a `ScoreManager` class that maintains the current score, handles score float updates (visuals), and manages difficulty scaling (pipe speed and spawn interval). `PlayState` will hold a unique pointer to `ScoreManager`.

**Tech Stack:** C++, SFML

## Global Constraints

- Follow existing project naming conventions.
- Use `std::unique_ptr` for manager ownership in `PlayState`.
- Ensure no regressions in scoring or difficulty scaling logic.

---

### Task 1: Create ScoreManager Header and Source Files

**Files:**
- Create: `src/ScoreManager.hpp`
- Create: `src/ScoreManager.cpp`

**Interfaces:**
- Consumes: None
- Produces: `ScoreManager::update(float dt)`, `ScoreManager::addScore()`, `ScoreManager::getScore()`, `ScoreManager::getDifficulty()`, `ScoreManager::getCurrentPipeSpeed()`, `ScoreManager::getCurrentSpawnInterval()`

- [ ] **Step 1: Write ScoreManager.hpp**

```cpp
#ifndef SCORE_MANAGER_HPP
#define SCORE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "ScoreFloat.h"

class ScoreManager {
public:
    ScoreManager(const sf::Font& font);

    void update(float dt);
    void addScore();
    
    int getScore() const;
    int getDifficulty() const;
    float getCurrentPipeSpeed() const;
    float getCurrentSpawnInterval() const;

private:
    int score = 0;
    int difficulty = 1;
    float currentPipeSpeed;
    float currentSpawnInterval;
    float scoreBounceTimer = 0.f;
    float scoreScale = 1.f;

    const sf::Font* font;
    std::vector<std::shared_ptr<ScoreFloat>> scoreFloats;
};

#endif
```

- [ ] **Step 2: Write ScoreManager.cpp**

```cpp
#include "ScoreManager.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include <cmath>
#include <algorithm>

ScoreManager::ScoreManager(const sf::Font& font) : font(&font) {
    currentPipeSpeed = ConfigLoader::getFloat("pipe_speed", Config::PIPE_SPEED);
    currentSpawnInterval = ConfigLoader::getFloat("pipe_spawn_interval", Config::PIPE_SPAWN_INTERVAL);

    if (difficulty == 0) { // Note: difficulty is initialized to 1 by default, but let's keep logic from PlayState
        // This part needs to be careful as difficulty might not be set yet.
        // Actually, in PlayState it was checked against the passed difficulty.
    } else if (difficulty == 2) {
        currentPipeSpeed *= 1.3f;
        currentSpawnInterval *= 0.7f;
    }
    // Wait, the logic in PlayState was:
    // if (difficulty == 0) { ... } else if (difficulty == 2) { ... }
    // But difficulty is passed to constructor. ScoreManager needs it too.
}

// I need to update the constructor to take initial difficulty.
```

- [ ] **Step 3: Refine ScoreManager.hpp and .cpp with correct constructor**

Update `ScoreManager` to accept `initialDifficulty`.

- [ ] **Step 4: Commit**

```bash
git add src/ScoreManager.hpp src/ScoreManager.cpp
git commit -m "feat: add ScoreManager class"
```

### Task 2: Refactor PlayState to use ScoreManager

**Files:**
- Modify: `src/PlayState.h`
- Modify: `src/PlayState.cpp`

**Interfaces:**
- Consumes: `ScoreManager`
- Produces: Updated `PlayState` methods using `ScoreManager`.

- [ ] **Step 1: Update PlayState header to include ScoreManager and replace fields**

Replace `score`, `difficulty`, `currentPipeSpeed`, `currentSpawnInterval`, `scoreBounceTimer`, `scoreScale`, `scoreFloats` with `std::unique_ptr<ScoreManager> scoreManager`.

- [ ] **Step 2: Update PlayState constructor to initialize ScoreManager**

- [ ] **Step 3: Refactor update() and draw() methods in PlayState.cpp**

Replace direct access to scoring/difficulty fields with calls to `scoreManager`.

- [ ] **Step 4: Commit**

```bash
git add src/PlayState.h src/PlayState.cpp
git commit -m "refactor: delegate scoring and difficulty to ScoreManager"
```

# Dynamic Difficulty Scaling Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a hybrid difficulty scaling system that smoothly transitions between predefined difficulty zones based on the player's score.

**Architecture:** The game will define "Difficulty Zones" (Easy, Normal, Hard, Insane). Within each zone, pipe speed and spawn intervals will scale linearly. Transitions occur at specific score milestones.

**Tech Stack:** C++, SFML, JSON (for configuration).

## Global Constraints
- Follow existing naming conventions for C++ classes and variables.
- Ensure all new config entries are accessible via the `ConfigLoader`.
- Maintain backward compatibility with existing `gameconfig.json` keys where possible.

---

### Task 1: Configuration Update

**Files:**
- Modify: `src/Config.hpp` (Add zone structures)
- Modify: `gameconfig.json` (Add zone data)

**Interfaces:**
- Produces: `Config::DifficultyZone` objects and updated `ConfigLoader` accessors.

- [ ] **Step 1: Update Config class to include DifficultyZone struct**
```cpp
struct DifficultyZone {
    int minScore;
    int maxScore;
    float basePipeSpeed;
    float baseSpawnInterval;
};
// Add a static vector of zones or similar accessors
```

- [ ] **Step 2: Update gameconfig.json with zone definitions**
(Add Easy, Normal, Hard, Insane zones)

- [ ] **Step 3: Commit changes**

### Task 2: PlayState Logic Implementation

**Files:**
- Modify: `src/PlayState.cpp` (Update update loop)

**Interfaces:**
- Consumes: `ScoreManager::getScore()`
- Produces: Interpolated `currentPipeSpeed` and `currentSpawnInterval`.

- [ ] **Step 1: Implement Zone detection and Lerp calculation in PlayState::update**
(Calculate lerp factor based on score within active zone)

- [ ] **Step 2: Apply interpolated values to pipe pool and spawn timer**

- [ ] **Step 3: Commit changes**

### Task 3: Debug Overlay & Verification

**Files:**
- Modify: `src/PlayState.cpp` (Draw debug info)

**Interfaces:**
- Consumes: Current lerp factor and zone name.
- Produces: Visual text on screen.

- [ ] **Step 1: Add debug text to draw() method showing current Zone and Lerp factor**

- [ ] **Step 2: Verify scaling behavior by running the game and checking values**

- [ ] **Step 3: Commit changes**

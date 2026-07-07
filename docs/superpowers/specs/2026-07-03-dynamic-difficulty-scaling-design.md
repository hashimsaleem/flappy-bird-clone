# Design Doc: Dynamic Difficulty Scaling

## Overview
This feature implements a hybrid difficulty scaling system for the Flappy Bird clone. Instead of simple linear scaling or jarring jumps, it uses "Difficulty Zones" where values scale linearly within a range and transition smoothly between milestones.

## Goals
- Provide a smooth progression of challenge as the player's score increases.
- Allow developers to balance specific "difficulty tiers" (Easy, Normal, Hard, Insane).
- Ensure the game remains playable for longer periods by preventing runaway speed increases.

## Architecture & Components

### 1. Configuration (`gameconfig.json` / `Config` class)
We will introduce a new structure for Difficulty Zones:
- **Zone Definition**: A struct containing `minScore`, `maxScore`, `basePipeSpeed`, and `baseSpawnInterval`.
- **Zones**: At least four zones (Easy, Normal, Hard, Insane).

### 2. Logic Layer (`PlayState::update`)
The core logic will reside in the main game loop:
- **Zone Detection**: Determine the active zone based on `ScoreManager::getScore()`.
- **Interpolation**: Calculate a `lerp` factor (0.0 to 1.0) based on the current score's position within the active zone's range.
- **Value Calculation**:
  - `currentSpeed = baseSpeed + (zoneMaxSpeed - baseSpeed) * lerpFactor`
  - `currentInterval = baseInterval - (baseInterval - zoneMinInterval) * lerpFactor`

### 3. Data Flow
1. `ScoreManager` -> Provides current score.
2. `PlayState` -> Queries active Zone data from Config.
3. `PlayState` -> Calculates interpolated speed/interval.
4. `PlayState` -> Updates `pipePool` and `spawnTimer`.

## Success Criteria
- Difficulty increases smoothly without sudden "teleporting" speeds.
- The game remains playable at high scores (e.g., 100+ points).
- Configuration allows for easy tuning of all thresholds and base values.

## Implementation Plan
1. Update `Config` class and `gameconfig.json`.
2. Modify `PlayState::update` to replace current hardcoded scaling with the hybrid logic.
3. Add a debug overlay (optional) to show current Zone and Lerp factor during development.

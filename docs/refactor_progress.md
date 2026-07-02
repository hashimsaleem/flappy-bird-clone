# Refactoring Progress: Decoupling PlayState (God Object)

## Overview
The goal is to refactor the project's architecture to reduce coupling by delegating responsibilities from the "God Object" `PlayState` to dedicated Manager classes.

## Completed Tasks
- **Visual Effects Refactor**:
    - Implemented `EnvironmentRenderer`, `CloudSystem`, and `ParticleSystem`.
    - Created `VisualEffectManager` to orchestrate visual systems.
    - Refactored `PlayState` to delegate all visual responsibilities to `VisualEffectManager`.
- **Sound Management**:
    - Created `SoundManager` class for BGM and SFX management.
    - Refactored `PlayState` to use `std::unique_ptr<SoundManager>`.
    - Updated `StateFactory` and `main.cpp` to remove raw sound pointers from the state creation pipeline.
- **Score Management**:
    - Created `ScoreManager` class to handle score value, score floats, and difficulty scaling logic.
    - Refactored `PlayState` to delegate scoring responsibilities to `ScoreManager`.
- **Build Fixes**:
    - Corrected build errors in `src/PlayState.h` related to missing members in `PlayStateSnapshot` and constructor variables.
    - Updated CMake files to include all manager source files.

## Current Status
- **Complete**: All primary refactoring tasks for the "God Object" decoupling are finished. Verification passed.

## Key Decisions
- **Manager-based approach**: Delegate responsibilities from `PlayState` to dedicated classes like `SoundManager`, `VisualEffectManager`, and `ScoreManager`.
- **Difficulty Scaling**: The new `ScoreManager` handles difficulty scaling logic (previously in `PlayState`).

## Relevant Files
- `src/PlayState.h` / `.cpp`
- `src/ScoreManager.hpp` / `.cpp`
- `src/VisualEffectManager.hpp` / `.cpp`
- `src/SoundManager.hpp` / `.cpp`
- `src/StateFactory.h` / `.cpp`
- `src/main.cpp`


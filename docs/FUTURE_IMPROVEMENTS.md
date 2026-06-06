# Future Improvement Suggestions

This document outlines potential future enhancements for the Flappy Bird Clone.

---

## 1. Visual Polish & UX
*   **Parallax Scrolling Background:** ✅ Implemented (Scrolling hills)
*   **Particle Effects:** ✅ Implemented (Crash particles)
*   **Animations:** ✅ Implemented (Spinning death fall)
*   **Menu System:** Implement a proper main menu with options to view high scores, toggle music/sound, or change difficulty settings.

## 2. Gameplay Mechanics
*   **Dynamic Difficulty:** ✅ Implemented (Speed and spawn rate scale every 5 points)
*   **Obstacle Variety:** ✅ Implemented (Moving pipes with sine-wave oscillation)
*   **Power-ups:** Add temporary power-ups (e.g., "Invincibility" or "Slow-motion").

## 3. Technical Architecture
*   **State Pattern:** Refactor the game loop into a proper State Pattern to manage different game states (`MenuState`, `PlayState`, `GameOverState`) more cleanly.
*   **Object Pooling:** Implement object pooling for pipes and particles to optimize memory usage.
*   **Unit Testing:** Integrate a testing framework (e.g., Catch2 or GoogleTest) for core logic like collision detection, score persistence, and config loading.

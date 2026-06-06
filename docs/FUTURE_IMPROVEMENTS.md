# Future Improvement Suggestions

This document outlines potential future enhancements for the Flappy Bird Clone.

---

## 1. Visual Polish & UX
*   **Parallax Scrolling Background:** Add a scrolling background layer (trees, hills, buildings) that moves at a fraction of the `PIPE_SPEED` to create a sense of depth.
*   **Particle Effects:** Spawn particle effects (small squares or circles) that scatter when the bird hits a pipe or the ground to simulate a crash.
*   **Animations:** Implement a "death animation" (e.g., the bird spinning rapidly and falling to the bottom of the screen).
*   **Menu System:** Implement a proper main menu with options to view high scores, toggle music/sound, or change difficulty settings.

## 2. Gameplay Mechanics
*   **Dynamic Difficulty:** Gradually increase `PIPE_SPEED` and decrease `PIPE_SPAWN_INTERVAL` as the user's score increases.
*   **Obstacle Variety:** Introduce moving obstacles (e.g., pipes moving up and down) or bonus collectibles.
*   **Power-ups:** Add temporary power-ups (e.g., "Invincibility" or "Slow-motion").

## 3. Technical Architecture
*   **State Pattern:** Refactor the game loop into a proper State Pattern to manage different game states (`MenuState`, `PlayState`, `GameOverState`) more cleanly.
*   **Object Pooling:** Implement object pooling for pipes and particles to optimize memory usage.
*   **Unit Testing:** Integrate a testing framework (e.g., Catch2 or GoogleTest) for core logic like collision detection, score persistence, and config loading.

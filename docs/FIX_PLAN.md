# Flappy Bird Clone Fix Plan

## Completed Tasks
- [x] **Pipe Rendering**: Fixed `Pipe` constructor to set the size for `bottomPipe`.
- [x] **Bird Boundaries**: Prevented the bird from going off-screen (top and bottom boundaries).
- [x] **Collision Logic**: Implemented collision detection between the bird and the pipes.
- [x] **Game State Management**: Implemented `START`, `PLAYING`, `GAME_OVER` states.
- [x] **Scoring System**: Added a score counter that increments when the bird successfully passes a pipe.
- [x] **Pipe Spawning**: Refined the pipe spawning logic to be more consistent.
- [x] **Game Over Screen**: Displayed a message and score when the game is over.
- [x] **Restart Mechanic**: Allowed the user to restart the game (e.g., by pressing Space).
- [x] **Text Rendering**: Added a score display on the screen.

## Remaining Tasks
- [x] **Delta Time**: Replace hardcoded `dt` with `sf::Clock` for true frame-rate independence.
- [ ] **Constants Header**: Move magic numbers (screen dimensions, speeds, gravity) to a dedicated configuration file/header.
- [ ] **Resource Manager**: Implement a manager to handle `sf::Texture` and `sf::Font` loading to avoid redundant loads and ensure persistence.
- [ ] **Visuals Improvement**: Replace `sf::RectangleShape` with `sf::Sprite` and load actual textures for bird and pipes.
- [ ] **High Score System**: Implement a system to track and save/load the high score to a file.
- [ ] **Sound Effects**: Add `sf::Sound` for jump, collision, and scoring events.
- [ ] **Physics Tuning**: Fine-tune the `GRAVITY` and `JUMP_STRENGTH` constants to achieve the classic Flappy Bird feel.
- [ ] **UI Improvement**: Add "Press Space to Start" text in the `START` game state.

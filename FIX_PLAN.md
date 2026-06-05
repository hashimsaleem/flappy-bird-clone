# Flappy Bird Clone Fix Plan

## Completed Tasks
- [x] **Pipe Rendering**: Fixed `Pipe` constructor to set the size for `bottomPipe`.
- [x] **Bird Boundaries**: Prevented the bird from going off-screen (top and bottom boundaries).
- [x] **Collision Logic**: Implemented collision detection between the bird and the pipes.
- [x] **Game State Management**: Implemented `START`, `PLAYING`, and `GAME_OVER` states.
- [x] **Scoring System**: Added a score counter that increments when the bird successfully passes a pipe.
- [x] **Pipe Spawning**: Refined the pipe spawning logic to be more consistent.
- [x] **Game Over Screen**: Displayed a message and score when the game is over.
- [x] **Restart Mechanic**: Allowed the user to restart the game (e.g., by pressing Space).
- [x] **Text Rendering**: Added a score display on the screen.

## Remaining Tasks
- [ ] **Physics Tuning**: Fine-tune the `GRAVITY` and `JUMP_STRENGTH` constants in `Bird.hpp` to achieve the classic Flappy Bird feel.
- [ ] **Visuals Improvement**: Replace the current `RectangleShape` for the bird and the pipes with `sf::Sprite` to use actual textures.
- [ ] **High Score System**: Implement a system to track and display the high score, perhaps saving it to a file.
- [ ] **Sound Effects**: Add sound effects for jumping, collision, and scoring.

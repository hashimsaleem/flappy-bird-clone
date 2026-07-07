# Advanced Particles Design

## Goal
Add three visually distinct particle effects to Flappy Bird: ground dust (trail behind bird), collision sparks (on death), and score sparkles (on scoring).

## Scope
Three effect types added to the existing `Particle` struct and `ParticleSystem`. No new files. No config changes.

## Design

### ParticleType enum
```cpp
enum class ParticleType {
    Fire,     // Existing yellow fire (backward compatible)
    Dust,     // Ground dust trail
    Spark,    // Collision sparks
    Bubble    // Score sparkles
};
```

### Particle struct changes
- Add `ParticleType type` field
- `update()` dispatches per-type physics and color logic
- `draw()` unchanged (still uses shape)

### Particle type behaviors

| Type | Color | Size | Lifetime | Physics |
|------|-------|------|----------|---------|
| Dust | `rgb(200,190,170)` | 2-4px | 0.3-0.5s | Floats down at 20-40px/s |
| Spark | `rgb(255,140,50)` | 3-5px | 0.2-0.4s | Explosive radial burst 200-600px/s |
| Bubble | `rgb(255,215,0)` | 6px | 0.6-0.8s | Rises 100px/s, sinusoidal wobble ±10px |

### ParticleSystem additions
- `spawnDust(sf::Vector2f pos, int count)` — ground dust at position
- `spawnSparks(sf::Vector2f pos, int count)` — collision sparks at position
- `spawnScoreSparkle(sf::Vector2f pos, int count)` — score sparkle at position
- Existing `spawn()` method unchanged (backward compatible)

### Integration points
- **PlayState::update()**: spawn dust periodically when bird is near ground
- **PlayState::triggerGameOver()**: spawn sparks at bird position
- **PlayState::onScore()**: spawn score bubble at score position
- **GameOverState::draw()**: continue rendering remaining particles after death

### Testing
- 6 new tests in `test_ParticleSystem.cpp`:
  - `spawnDustCreatesParticles`
  - `dustFloatsDownward`
  - `spawnSparksCreatesParticles`
  - `sparksExplosiveBurst`
  - `spawnScoreSparkleCreatesParticles`
  - `scoreBubbleRisesUpward`

### What's NOT in scope
- Particle trails on pipes
- Power-up collection particles
- Configurable particle effects
- Sprite-based particles (still rectangles)

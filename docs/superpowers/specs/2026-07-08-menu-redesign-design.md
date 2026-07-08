# Menu Redesign Design

## Objective
Redesign the Flappy Bird menu screen to be visually polished and appealing, replacing the current plain dark background with a rich, animated experience.

## Design Decisions

### Option A — Gradient Sky (APPROVED)
- Animated gradient sky background cycling through sunset colors
- Parallax cloud layers at different scroll speeds
- Large animated title with text glow/shadow effect
- Animated bird (bird.png sprite) flying in arc pattern with wing-flap animation
- Glowing play button with gradient fill
- Pill-shaped difficulty selector (Easy/Medium/Hard)
- Volume control bar with gradient fill

### Layout
```
┌─────────────────────────────────────────┐
│              ☀️ sun glow                 │
│    ☁️  ☁️    parallax clouds layer 1     │
│                                         │
│         FLAPPY BIRD (glowing title)     │
│                                         │
│             🐦 animated bird            │
│                                         │
│       🏆 High Score: 0                  │
│                                         │
│          ▶ PLAY GAME (glowing btn)      │
│                                         │
│     [Easy] [Medium] [Hard] (pills)      │
│                                         │
│     VOL [====----] 50% (slider bar)     │
└─────────────────────────────────────────┘
```

### Visual Details
| Element | Behavior |
|---------|----------|
| **Sky gradient** | Uses existing `Config::SKY_TOP` / `Config::SKY_BOT` arrays, lerps between colors over 120s cycle. Draws as two rectangles (top half, bottom half) |
| **Clouds** | Reuses `CloudSystem` — 6 clouds with different speeds (0.4x-0.8x), rendered as overlapping circles |
| **Sun glow** | `sf::CircleShape` with radial gradient at top-right, slight bob animation |
| **Title** | 60px text, white, with layered glow using `sf::RectangleShape` behind text, bob ±5px |
| **Bird** | `bird.png` sprite, centered below title, arc path (sin Y + slight X), wing-flap via scale oscillation |
| **High Score** | Pill with trophy emoji, gold text, semi-transparent bg |
| **Play button** | Gradient from `#667eea` to `#764ba2`, rounded rect (radius 28px), glow shadow, pulse animation |
| **Difficulty pills** | 3 small pills, selected has gradient fill + glow, unselected has subtle bg |
| **Volume bar** | Horizontal bar with gradient fill, +/- keys adjust 10% steps |

### Interaction
| Input | Action |
|-------|--------|
| `1` or click on Play button | Start game |
| `3` | Easy difficulty |
| `4` | Medium difficulty |
| `5` | Hard difficulty |
| `+` / `-` | Volume up/down |
| `Esc` | Exit |

### File Changes
| File | Change |
|------|--------|
| `src/states/MenuState.h` | Full redesign — sky rendering, cloud system integration, glow effects, new UI elements |
| **No changes to** | `GameState.h`, `StateFactory.cpp`, any other file, any tests |

### Risks & Mitigations
- **Texture loading in onEnter()**: Bird texture must exist at `cfg.birdPath`. Added check: if load fails, falls back to yellow circle sprite
- **CloudSystem integration**: CloudSystem already exists and is used in PlayState. MenuState will create its own instance
- **Performance**: Sky gradient + clouds + bird animation is lightweight (no heavy effects, no particles)

## Implementation Plan

### Phase 1: Sky + Clouds
1. Add `skyTimer`, `cloudSystem`, `cloudOffset` members to MenuState
2. In `update()`: increment `skyTimer`, call `cloudSystem->update(dt)`
3. In `draw()`: render gradient sky using `EnvironmentRenderer` logic, then clouds

### Phase 2: Title + Bird
4. Add glow behind title using layered `sf::RectangleShape`
5. Bird arc animation: `sin(birdBobTimer)` for Y, `sin(birdFlapTimer)` for scale
6. Load `bird.png` in `onEnter()` with fallback

### Phase 3: UI Elements
7. Play button: gradient `sf::RectangleShape` with glow + pulse
8. Difficulty pills: 3 small rounded rects
9. Volume bar: horizontal bar with gradient fill
10. High score: pill-shaped display

### Phase 4: Polish
11. Mouse click support for play button
12. Sun glow decoration at top-right
13. Final review

# Code Review: Flappy Bird Clone (SFML / C++)

**Review Date:** 2026-06-18
**Project:** Flappy Bird Clone (CMake + C++17 + SFML 3.0.2)
**Scope:** All source files, assets, config, build system, tests
**Method:** Three parallel review passes (memory/ownership, logic/bugs, SFML 3 API/rendering), with every finding verified against the actual source. The project builds cleanly against SFML 3.0.2.

> **Note:** This review supersedes the 2026-06-07 review. Several issues here were introduced or left unaddressed by the state-pattern refactor and SFML 3 migration. One agent-flagged "compile error" (copying `sf::SoundBuffer`) was a **false positive** — SFML 3.0.2 exposes a public copy constructor — and is not listed below.

---

## Summary

The game is playable and well-structured, but the state-machine refactor introduced memory leaks on menu/high-score transitions, and the `PlayState → GameOverState` snapshot is broken in several ways that make the game-over screen show no pipes, no particles, and a frozen bird. Two visual systems (sky day-night cycle, ground scrolling) are dead code due to a `dt = 0` call-site bug.

---

## 🔴 Critical Issues (memory safety / crashes / UB)

### 1. Memory leaks on menu & high-score state transitions [FIXED]

**File:** `src/main.cpp`

`createMenuState`, `createPlayState`, and `createHighScoreScreen` assign `state = new ...` without deleting the previous state. The Play→GameOver and GameOver→* transitions correctly `delete state` first, but the menu-related transitions do not.

- Menu → Play: leaks `MenuState`
- Menu → HighScores: leaks `MenuState`
- HighScores → Menu: leaks `HighScoreScreenState`

**Fix:** Replace the raw `GameState* state` with `std::unique_ptr<GameState>` so assignment automatically frees the old state. This also closes the dangling-pointer window between `delete state` and `state = new ...`.

---

### 2. Memory leak in `Bird::load()` else branch [FIXED]

**File:** `src/Bird.cpp:24`

On texture-load failure, a new sprite is allocated without deleting the old one. The `if` branch calls `delete sprite;` first; the `else` branch does not. The comment even says "Keep the placeholder sprite" but the code allocates a new one.

```cpp
} else {
    // Keep the placeholder sprite — it's still valid since placeholderTexture is a member
    sprite = new sf::Sprite(placeholderTexture);  // ← OLD SPRITE LEAKED
    ...
}
```

Since `PlayState::onEnter()` calls `reset()` then `load()` on every restart, this leak repeats each game if the texture is missing.

**Fix:** Add `delete sprite;` before the allocation, or reuse the existing placeholder sprite (just update origin/position).

---

### 3. Null pointer dereference risk in `Bird::update()` / `animateTilt()` [FIXED]

**File:** `src/Bird.cpp:43, 57, 91`

After `Bird::reset()`, `sprite` is `nullptr`. But `update()` and parts of `animateTilt()` dereference `sprite` with no null check, while `draw()`, `getBoundingBox()`, and other `animateTilt()` paths do check. Currently masked because `onEnter()` calls `load()` immediately after `reset()`, but any code path that calls `update()` between the two is undefined behavior.

```cpp
void Bird::update(float dt) {
    if (isDying) {
        sprite->setPosition({posX, posY});        // ← no null check
        sprite->setRotation(sf::degrees(tiltAngle)); // ← no null check
        ...
```

**Fix:** Replace `sf::Sprite*` with `std::unique_ptr<sf::Sprite>` or `std::optional<sf::Sprite>`, or at minimum add consistent null checks.

---

### 4. Undefined behavior in `ScoreFloat::update()` [FIXED]

**File:** `src/ScoreFloat.h:28`

When `lifetime` goes negative (which happens every expiration, since `update()` runs before the erase-remove), `scale` goes negative and the cast to `unsigned int` is undefined behavior:

```cpp
text.setCharacterSize(static_cast<unsigned int>(28 * scale)); // ← UB when scale < 0
```

**Fix:** Clamp `scale` to `>= 0`, or guard the whole update when `lifetime <= 0`.

---

### 5. `ConfigLoader::parse()` crashes on array values [FIXED]

**File:** `src/ConfigLoader.cpp`

Array values (e.g. `"key": [1, 2, 3]`) hit the "skip until comma" branch, producing a raw value like `[1`. `parseValue("[1")` then reaches the integer path and `std::stoi("[1")` throws `std::invalid_argument` — uncaught, terminating the program. Nested objects without float values trigger the same crash.

Latent (current `gameconfig.json` has no arrays) but a real robustness hole.

**Fix:** Detect `[` / `{` in `parseValue` and return `NONE`, or skip arrays/objects in the parser like nested objects are already skipped.

---

### 6. `ConfigLoader::parse()` out-of-bounds read [FIXED]

**File:** `src/ConfigLoader.cpp:84`

If JSON ends with `"key": ` (trailing whitespace, no value), the whitespace-skip loop leaves `i == json.size()` and `json[i]` reads out of bounds:

```cpp
while (i < json.size() && std::isspace(json[i])) ++i;
size_t valStart = i;
if (json[i] == '"') {   // ← OOB if i == json.size()
```

Latent (needs malformed JSON).

**Fix:** Guard with `if (i >= json.size()) break;` before accessing `json[i]`.

---

## 🟠 Logic Bugs (incorrect behavior)

### 7. `getSnapshot()` drops pipes & particles [FIXED]

**File:** `src/PlayState.cpp`

Only `birdState`, `score`, and `scoreFloats` are copied. `pipes` and `particles` remain empty. The game-over screen iterates these vectors to render, so it shows **no pipes and no death explosion** — the 20 particles created in `triggerGameOver()` are lost.

**Fix:** `out.pipes = pipes; out.particles = particles;`

---

### 8. `getSnapshot()` hardcodes `isDying = false` [FIXED]

**File:** `src/PlayState.cpp:196`

The bird *is* dying when the snapshot is taken, but the snapshot forces `isDying = false`. `GameOverState::update()` only runs the death fall/spin when `birdState.isDying` is true, so **the bird freezes in place on the game-over screen** instead of falling and spinning.

**Fix:** `bs.isDying = bird.isDyingFlag();`

---

### 9. `getSnapshot()` uses bounding-box top for `posY` [FIXED]

**File:** `src/PlayState.cpp`

`getBoundingBox().position.y` is the top edge (sprite origin is center), but `GameOverState::draw()` subtracts `BIRD_HEIGHT/2` again, placing the bird **~20px too high** on the game-over screen.

**Fix:** Store the actual center `posY`, or `bs.posY = bird.getBoundingBox().position.y + bird.getBoundingBox().size.y / 2.f;`

---

### 10. Sky day-night cycle never advances [FIXED]

**File:** `src/PlayState.cpp`

`drawSky()` advances `skyTimer` using `dt`, but `draw()` calls it with `dt = 0.f`:

```cpp
drawSky(window, 0.f);   // ← skyTimer never changes
```

The entire day-night cycle system is dead code. The `(void)dt;` line is misleading — it suppresses the unused-parameter warning even though `dt` is actually used.

**Fix:** Move `skyTimer += dt;` into `update()`.

---

### 11. Ground scrolling never advances [FIXED]

**File:** `src/PlayState.cpp`

Same `dt = 0.f` issue — `drawGround()` is called with `0.f`, so `groundScrollOffset` never updates and the ground appears static while pipes move.

**Fix:** Move `groundScrollOffset += currentPipeSpeed * dt;` into `update()`.

---

### 12. Bird invisible during death animation [FIXED]

**File:** `src/PlayState.cpp:179`

```cpp
if (!bird.isDyingFlag()) bird.draw(window);
```

The bird is hidden the moment it dies; only particles render. The death physics (spin, fall) still run in `update()` but are never seen.

**Fix:** Always draw the bird, or draw it during the dying state.

---

### 13. Explosion particles only move right and down [FIXED]

**File:** `src/PlayState.cpp:64`

`typeDist` is `uniform_int_distribution<int>(0, 4)`, so `vx` and `vy` are always non-negative. The "explosion" only goes right and down, never left or up.

```cpp
float vx = static_cast<float>(typeDist(rng)) * 100.0f;  // [0, 400]
float vy = static_cast<float>(typeDist(rng)) * 100.0f;  // [0, 400]
```

**Fix:** Center around zero, e.g. `(typeDist(rng) - 2) * 200.0f` for range `[-400, 400]`.

---

### 14. Menu "Press ENTER" prompt nearly invisible [FIXED]

**File:** `src/MenuState.h:83`

```cpp
auto promptAlpha = static_cast<unsigned char>(0.5f + 0.5f * std::sin(menuGlowTimer * 2.0f));
```

`0.5 + 0.5 * sin(...)` ranges `0.0`–`1.0`; the cast to `unsigned char` yields `0` or `1` (out of 255). The prompt is effectively fully transparent.

**Fix:** `static_cast<unsigned char>(255 * (0.5f + 0.5f * std::sin(menuGlowTimer * 2.0f)));`

---

### 15. No way to return to menu from game over [FIXED]

**File:** `src/GameOverState.cpp`

`handleKeyPress()` only sets `nextActionCode = 1` (Space = restart). `main.cpp` checks for action `2` (return to menu) but it is never set. The player is forced to restart with no path back to the main menu.

**Fix:** Bind a key (e.g. Escape) to `nextActionCode = 2` in `GameOverState::handleKeyPress()`.

---

### 16. Bird falls through ground on game-over screen [FIXED]

**File:** `src/GameOverState.cpp:22`

The death-fall update has no ground collision check, so the bird falls through the ground and off-screen. Currently masked by bug #8 (`isDying = false`), but would manifest if that were fixed.

**Fix:** Clamp `birdState.posY` to the ground line.

---

### 17. ScoreFloat scale discontinuity [FIXED]

**File:** `src/ScoreFloat.h:21`

At `progress = 0.3`, scale jumps from 2.6 to 1.6:

```cpp
if (progress < 0.3f) {
    scale = 1.0f + Config::SCORE_FLOAT_SCALE_MAX * (progress / 0.3f);  // → 2.6 at 0.3
} else {
    scale = Config::SCORE_FLOAT_SCALE_MAX * (1.0f - (progress - 0.3f) / 0.7f);  // → 1.6 at 0.3
}
```

**Fix:** First branch should use `Config::SCORE_FLOAT_SCALE_MAX - 1.0f` to go from 1.0 to 1.6.

---

### 18. Most config values are loaded but never used [FIXED]

**File:** `src/PlayState.cpp`

Only `pipe_speed`, `pipe_spawn_interval`, `pipe_speed_max`, and `spawn_interval_min` are read from `ConfigLoader`. `gravity`, `jump_strength`, `gap_height`, `ground_height`, `bird_min_tilt`, `bird_max_tilt`, `bird_flap_rate`, `bird_flap_depth` are parsed then ignored — editing them in `gameconfig.json` has no effect.

**Fix:** Wire these through to `Bird` and `PlayState`, or remove them from the config schema.

---

## 🟡 Minor / Design Issues

### 19. High score saved inside `draw()` [FIXED]

**File:** `src/GameOverState.cpp`

File I/O (`HighScore::save()`) runs in the render path. The `if (score > highScore)` guard prevents repeated saves after the first frame, but this violates separation of concerns and can cause frame hitches.

**Fix:** Move to `update()` or `onEnter()`.

---

### 20. `bird.load()` called twice on state entry [FIXED]

**File:** `src/PlayState.cpp`

The constructor calls `bird.load()`, then `onEnter()` calls `reset()` + `load()` again, wasting the first load (file I/O + allocation/deallocation) and doubling the leak from issue #2.

**Fix:** Remove the `load()` from the constructor.

---

### 21. Malformed silent-WAV fallback [FIXED]

**File:** `src/ResourceManager.cpp`

The fallback silent WAV is structurally invalid: offset 12 has `0x10` instead of the `"fmt "` chunk tag, and the `"data"` tag is at the wrong offset. `loadFromMemory()` fails, so the code silently falls through to the ultimate empty-buffer fallback. The silent sound "works" by accident.

**Fix:** Correct the WAV header bytes, or drop the synthesized WAV and rely on the empty-buffer fallback directly.

---

### 22. Empty font fallback renders invisible text [FIXED]

**File:** `src/ResourceManager.cpp`

If `font.ttf` fails to load, a default-constructed `sf::Font()` is stored. In SFML 3 this has no glyphs, so all text (score, menu, game over) disappears with no warning.

**Fix:** Log a clear error; consider bundling a known-good fallback font.

---

### 23. Dead code [FIXED]

- `src/ObjectPool.h` — entire template defined but never instantiated.
- `src/MenuState.h:68` — `glowAlpha` computed but never used.
- `src/MenuState.h:70` — green channel `220 * intensity + 220 * (1 - intensity)` simplifies to constant `220`.
- `src/ConfigLoader.cpp:73` — `Value currentArray` declared but never used.
- `src/main.cpp` — `PlayState` action `3` (close window) is checked but never set.

---

### 24. Bird bounding box includes rotation [FIXED]

**File:** `src/Bird.cpp:108`

`getBoundingBox()` returns `sprite->getGlobalBounds()`, the axis-aligned box after rotation. When the bird is tilted, this box is larger than the actual 40×40 sprite, making collisions forgiving/imprecise.

---

### 25. Score text shakes with the game world [FIXED]

**File:** `src/PlayState.cpp`

Score and high-score text are drawn inside the shake view, so they shake along with the world during the screen-shake effect. UI should be drawn after restoring the original view.

---

### 26. Integration test hardcoded absolute path [FIXED]

**File:** `tests/test_Integration.cpp`

```cpp
ConfigLoader::load("/home/hashim/trading/assets/gameconfig.json");
```

Won't run on any other machine. Should use a relative path or test fixture.

---

### 27. `HighScore` binary format is platform-dependent [FIXED]

**File:** `src/HighScore.cpp`

The score is written as raw `int` bytes with no endianness or size validation. A corrupted file yields arbitrary (including negative) values.

---

### 28. `ConfigLoader::validate()` doesn't verify types [FIXED]

**File:** `src/ConfigLoader.cpp`

The type string in the required-keys map is only used in the warning message. Validation only checks that a value exists and is not `NONE` — it doesn't verify that `screen_width` is actually an `int` or `gravity` is a `float`.

---

### 29. Unnecessary `pushGLStates` / `popGLStates` [FIXED]

**File:** `src/PlayState.cpp`

No raw OpenGL calls are made, so saving/restoring the full OpenGL state stack is pure per-frame overhead.

---

### 30. `ScoreFloat` uses `setCharacterSize` instead of `setScale` [FIXED]

**File:** `src/ScoreFloat.h`

Changing character size forces glyph re-rendering every frame. `text.setScale({scale, scale})` would transform already-rendered text far more efficiently.

---

## Priority Recommendations

1. **Switch `GameState* state` to `std::unique_ptr<GameState>`** in `main.cpp` — fixes #1 and the dangling-pointer window in one stroke.
2. **Fix `Bird::load()` else branch** — add `delete sprite;` or reuse the placeholder. Fixes #2.
3. **Fix `getSnapshot()`** — populate `pipes`/`particles`, set `isDying` correctly, use center `posY`. Fixes #7, #8, #9 (the broken game-over screen).
4. **Move `skyTimer` and `groundScrollOffset` updates into `update()`** — fixes #10, #11 (two dead visual systems).
5. **Fix the menu prompt alpha** (`* 255`) — fixes #14.
6. **Add Escape → menu on game over** — fixes #15.

## ✅ Resolution Summary

All 30 issues identified in this review have been addressed and verified:

- **Memory Safety**: Migrated to `std::unique_ptr` for state management, fixed leaks in `Bird::load()`, and added null checks.
- **Robustness**: Improved `ConfigLoader` to handle malformed JSON, arrays, and out-of-bounds reads safely.
- **Game Logic**: Fixed the game-over snapshot (pipes, particles, bird state), corrected explosion physics, and moved time-dependent updates from `draw()` to `update()`.
- **UI/UX**: Corrected menu prompt visibility, added an Escape key return to menu, and fixed score scale discontinuities.
- **Code Quality**: Removed dead code, removed redundant loads, and standardized high score persistence.
- **SFML 3 Migration**: Ensured all rendering and resource loading is compatible with SFML 3.0.2.

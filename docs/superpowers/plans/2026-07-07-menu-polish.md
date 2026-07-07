# Menu Polish Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to implement this plan task-by-task.

**Goal:** Add title bob animation and bird preview sprite to the main menu.

**Architecture:** Extend `MenuState.h` inline — add `sf::Sprite` + `sf::Texture` for bird preview, `float` timers for bob animation. Sine-based motion in `update()`, rendered in `draw()`. No new files.

**Tech Stack:** C++23, SFML 3.0, CMake, Google Test

## Global Constraints

- C++23, SFML 3.0
- All 189 existing tests must continue to pass
- Only modify `MenuState.h` — no new files, no changes to other source/test files
- Follow existing naming: `camelCase` members, `sf::` prefixes
- `assets/bird.png` already exists — reuse it
- YAGNI: no title shadow, no bird shadow, no fade-in — just bob + idle bird

---

### Task 1: Add bird texture, sprite, and bob timers to MenuState

**Files:**
- Modify: `src/states/MenuState.h:118-119` (add private members)

**Interfaces:**
- Consumes: nothing new
- Produces: 4 new members: `sf::Sprite birdSprite`, `sf::Texture birdTexture`, `float bobTimer = 0.f`, `float birdBobTimer = 0.f`, `float birdFlapTimer = 0.f`

- [ ] **Step 1: Add new members to MenuState**

Add these private members after line 119 (`int difficulty = 1;`):

```cpp
    // Menu polish
    float bobTimer = 0.f;
    float birdBobTimer = 0.f;
    float birdFlapTimer = 0.f;
    sf::Texture birdTexture;
    sf::Sprite birdSprite;
```

- [ ] **Step 2: Build to verify syntax**

Run: `mkdir -p build && cd build && cmake .. && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds (no errors). Warnings about unused variables are fine.

- [ ] **Step 3: Run existing tests to verify nothing broke**

Run: `cd build && ctest --output-on-failure 2>&1 | tail -20`
Expected: All tests pass.

- [ ] **Step 4: Commit**

```bash
git add src/states/MenuState.h
git commit -m "wip: add menu polish members to MenuState"
```

---

### Task 2: Load bird texture in onEnter

**Files:**
- Modify: `src/states/MenuState.h:100-102` (onEnter method)

**Interfaces:**
- Consumes: `birdTexture.loadFromFile()` from SFML
- Produces: bird sprite loaded and positioned

- [ ] **Step 1: Load bird texture in onEnter()**

Add these 4 lines to the top of `onEnter()` (after `blinkTimer = 0.f;`):

```cpp
    birdTexture.loadFromFile("assets/bird.png");
    birdSprite.setTexture(birdTexture);
    birdSprite.setPosition(355.f, 250.f);
    birdSprite.setScale(2.2f, 2.2f);
```

- [ ] **Step 2: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds.

- [ ] **Step 3: Commit**

```bash
git add src/states/MenuState.h
git commit -m "wip: load bird texture in onEnter"
```

---

### Task 3: Add bob animation to update() and draw()

**Files:**
- Modify: `src/states/MenuState.h:24-34` (update method)
- Modify: `src/states/MenuState.h:36-78` (draw method)

**Interfaces:**
- Consumes: `std::sin()` from `<cmath>` (already included via `<SFML/Graphics.hpp>`)
- Produces: animated title + bird

- [ ] **Step 1: Add bob timer to update()**

Replace the `update()` method body (lines 24-34) with:

```cpp
    void update(float dt) override {
        blinkTimer += dt;
        bobTimer += dt;
        birdBobTimer += dt;
        birdFlapTimer += dt;
        if (selectedOption >= 0) {
            if (selectedOption == 0) {
                nextActionCode = StateAction::PlayGame;
            } else if (selectedOption == 1) {
                nextActionCode = StateAction::ShowHighScore;
            }
        }
    }
```

- [ ] **Step 2: Add bird draw + title bob to draw()**

In the `draw()` method, after line 41 (`window.draw(bg);`) and before line 43 (title), add the bird:

```cpp
        birdSprite.setOrigin(20.f, 20.f);
        float birdOffset = std::sin(birdBobTimer * 1.5f) * 3.0f;
        birdSprite.setPosition(400.f, 250.f + birdOffset);
        birdSprite.setScale(2.2f + std::sin(birdFlapTimer * 8.0f) * 0.05f,
                            2.2f + std::sin(birdFlapTimer * 8.0f) * 0.05f);
        birdSprite.setColor(sf::Color(255, 255, 255, 200));
        window.draw(birdSprite);
```

Then add bob offset to the title Y position (line 43-45). Change:

```cpp
        auto titleText = makeText(fontRef, "FLAPPY CLONE", 60, Config::TEXT_COLOR,
            {static_cast<float>(Config::SCREEN_WIDTH) / 2.f - 100.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 120.f});
```

To:

```cpp
        float titleBob = std::sin(bobTimer * 2.0f) * 5.0f;
        auto titleText = makeText(fontRef, "FLAPPY CLONE", 60, Config::TEXT_COLOR,
            {static_cast<float>(Config::SCREEN_WIDTH) / 2.f - 100.f, static_cast<float>(Config::SCREEN_HEIGHT) / 2.f - 120.f + titleBob});
```

- [ ] **Step 3: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds.

- [ ] **Step 4: Run all tests**

Run: `cd build && ctest --output-on-failure 2>&1 | tail -20`
Expected: All tests pass.

- [ ] **Step 5: Commit**

```bash
git add src/states/MenuState.h
git commit -m "feat: add title bob and bird preview to menu"
```

---

### Task 4: Final build and test verification

**Files:**
- Build: full project

**Interfaces:**
- Consumes: nothing
- Produces: verified passing build + tests

- [ ] **Step 1: Clean build**

Run: `cd build && cmake --build . --clean-first 2>&1 | tail -15`
Expected: Clean build succeeds.

- [ ] **Step 2: Full test suite**

Run: `cd build && ctest --output-on-failure`
Expected: 189+ tests, all passing.

- [ ] **Step 3: Commit**

```bash
git commit --amend --no-edit
```
(No new commit needed — Task 3 already committed. This step is just to confirm.)

---

**Plan complete. Ready to execute.**

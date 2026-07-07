# Menu Polish Design

## Overview
Add two visual improvements to the main menu: title bob animation and bird preview sprite.

## Changes

### Title Bob Animation
- Add `float bobTimer = 0.f` member to `MenuState`
- In `update()`: `bobTimer += dt`; compute `bob = std::sin(bobTimer * 2.0f) * 5.0f`
- Apply `bob` offset to title Y position in `draw()`
- Period: ~1.57s, vertical range: ±5px

### Bird Preview
- Add `sf::Sprite birdSprite`, `sf::Texture birdTexture`, `float birdBobTimer = 0.f`, `float birdFlapTimer = 0.f` members
- In `onEnter()`: load `assets/bird.png` into `birdTexture`, set sprite texture
- Position: centered horizontally (x = 355), y = 250, scale 2.2x (90px wide), alpha 200/255
- Bob: `std::sin(birdBobTimer * 1.5f) * 3.0f`, period ~2.5s
- Wing flap: scale `1.0 + std::sin(birdFlapTimer * 8.0f) * 0.05`

### File Changes
- **Only `MenuState.h`**: add 4 members, modify `update()` (+2 lines), `draw()` (+12 lines), `onEnter()` (+5 lines)
- **No changes** to StateFactory, GameState, any other file, or any tests

### Risk
- Texture loading in `onEnter()` must not fail silently if asset path is wrong
- Bird rendered behind text — z-order unchanged from existing draw sequence

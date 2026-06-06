# Code Review: Issues & Improvement Suggestions

This document outlines issues found during the code review and prioritized improvement suggestions for the Flappy Bird Clone.

---

## 🔺 Issues Found

### Critical

| # | File | Issue | Impact |
|---|------|-------|--------|
| 1 | `Bird.cpp` | `load()` method is a **placeholder** — does nothing. No texture is loaded despite the method signature suggesting asset loading. | Bird renders as a blank 40×40 rectangle regardless of any "bird.png" path. |
| 2 | `main.cpp` | `gameClock.restart()` is called **twice** — once after declaration and again implicitly before the first `restart()` in the loop. The first call is redundant and could cause the first frame's `dt` to be incorrect. | First frame physics may be inaccurate. |
| 3 | `Pipe.hpp` | `GAP_HEIGHT` is declared as `static constexpr float GAP_HEIGHT = 150.0f` but the constructor takes a `gapHeight` parameter that is **never used**. | The hardcoded `GAP_HEIGHT` is misleading — the actual gap comes from the constructor argument. |
| 4 | `Pipe.cpp` | `velocityX` is hardcoded (`200.0f`) in the constructor with no way to configure it. | No tuning without recompiling. |
| 5 | **All files** | **SFML 3.0 API incompatibility** — code was written for SFML 2 but system has SFML 3.0.2 installed. | Compilation failures: `sf::Sprite` has no default constructor, `sf::Image::create()` removed, `sf::Texture::loadFromFile()` is now an instance method, `sf::FloatRect` constructor changed, `sf::Rectangle` removed. |

### Medium

| # | File | Issue | Impact |
|---|------|-------|--------|
| 5 | `Bird.cpp` | Bird X position is hardcoded to `50.f` inside `Bird::update()`. | Bird cannot move horizontally; not extensible for future features (e.g., horizontal movement). |
| 6 | `main.cpp` | `START` state renders **nothing** — no "Press Space to Start" prompt. | Players may not know how to begin. |
| 7 | `main.cpp` | Font and texture paths are hardcoded strings (`"font.ttf"`, `"bird.png"`). | Fragile — breaks if assets are relocated. No error recovery. |
| 8 | `main.cpp` | `sf::default_random_engine` and distributions are created in `main()` every run. | Minor: no persistent seed means different behavior every run (not necessarily a bug, but worth noting for reproducibility). |
| 9 | `Pipe.cpp` | Pipes use `sf::Color::Green` hardcoded. | No visual customization without code changes. |

### Low

| # | File | Issue | Impact |
|---|------|-------|--------|
| 10 | `main.cpp` | `if (dt > 0.1f) std::cout << "Large dt: " << dt;` is a debug log left in production code. | Unnecessary console spam in release builds. |
| 11 | `Bird.cpp` | `GRAVITY` and `JUMP_STRENGTH` are member variables initialized in the class body. | They are effectively constant but not `constexpr` or `static`. Misleading — looks like they can be changed per-instance. |
| 12 | `main.cpp` | No `sf::RenderWindow::setFramerateLimit()` error check (SFML 3+ renamed this to `setVerticalSyncEnabled` / `setFramerateLimit`). | May compile differently across SFML versions. |
| 13 | `main.cpp` | On game over, the bird and pipes are drawn but there's no visual distinction (e.g., dimming, freeze). | Game over state is not visually clear to the player. |
| 14 | `main.cpp` | `gameOverText` uses `sf::Color::Red` hardcoded. | Inconsistent with any design system. |

---

## 🚀 Improvement Suggestions

### ✅ Phase 1 Complete — Issues Fixed

| # | Issue | Fix Applied |
|---|-------|-------------|
| 1 | `Bird::load()` placeholder | ✅ Implemented with `sf::Sprite` + `sf::Texture` |
| 2 | Double `gameClock.restart()` | ✅ Removed redundant call |
| 3 | Dead `GAP_HEIGHT` in `Pipe.hpp` | ✅ Removed; constructor `gapHeight` parameter now used |
| 4 | Hardcoded `velocityX` in `Pipe` | ✅ Moved to `Config::PIPE_SPEED` |
| 5 | SFML 3.0 incompatibility | ✅ Full migration (see details below) |

### 📝 SFML 3.0 Migration Changes

| SFML 2 | SFML 3.0 | Example |
|--------|----------|---------|
| `sf::Sprite sprite;` (default ctor) | `sf::Sprite* sprite = nullptr;` (no default ctor) | Use raw pointer, init in `load()` |
| `sf::Image img; img.create(size, color);` | `sf::Image img(size, color);` | Constructor takes args directly |
| `sf::Texture::loadFromFile(path)` (static) | `texture.loadFromFile(path)` (instance) | Call on instance |
| `sf::FloatRect(x, y, w, h)` | `sf::FloatRect({x, y}, {w, h})` | Takes Vector2 pairs |
| `sf::Rectangle` | `sf::RectangleShape` | Renamed |
| `sf::RenderTexture(w, h)` | `sf::RenderTexture rt; rt.create({w, h});` | Two-step creation |

### High Priority

#### 1. Implement `Bird::load()` with `sf::Sprite`
```cpp
// Bird.hpp
sf::Sprite sprite;
sf::Texture birdTexture;

// Bird.cpp
void Bird::load(const std::string& texturePath) {
    birdTexture.load(texturePath);
    sprite.setTexture(birdTexture);
    sprite.setOrigin(birdTexture.getSize().x / 2.f, birdTexture.getSize().y / 2.f);
    sprite.setPosition(50.f, posY);
}
```
**Why:** The entire visual system currently uses `RectangleShape` which defeats the purpose of a graphics library. Switching to `sf::Sprite` is the single highest-impact visual improvement.

#### 2. Create a `Config` / Constants Header
```cpp
// Config.hpp
namespace Config {
    constexpr int SCREEN_WIDTH  = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr int TARGET_FPS    = 60;

    // Bird
    constexpr float GRAVITY          = 800.0f;
    constexpr float JUMP_STRENGTH    = -600.0f;
    constexpr float BIRD_START_X     = 50.0f;
    constexpr float BIRD_START_Y     = 300.0f;
    constexpr float BIRD_WIDTH       = 40.0f;
    constexpr float BIRD_HEIGHT      = 40.0f;

    // Pipe
    constexpr float PIPE_WIDTH       = 60.0f;
    constexpr float PIPE_HEIGHT      = 400.0f;
    constexpr float PIPE_SPEED       = 200.0f;
    constexpr float GAP_HEIGHT       = 150.0f;
    constexpr float PIPE_SPAWN_INTERVAL = 1.5f;
    constexpr float PIPE_MIN_Y       = 150.0f;
    constexpr float PIPE_MAX_Y       = 450.0f;
    constexpr float PIPE_GAP_RANGE   = 250.0f; // min to max gap variation

    // Assets
    constexpr const char* FONT_PATH  = "font.ttf";
    constexpr const char* BIRD_PATH  = "bird.png";

    // Colors
    constexpr sf::Color SKY_COLOR    = {135, 206, 235};
    constexpr sf::Color PIPE_COLOR   = {34, 139, 34};  // Forest green
    constexpr sf::Color TEXT_COLOR   = {255, 255, 255};
    constexpr sf::Color GAME_OVER_COLOR = {255, 0, 0};
}
```
**Why:** Eliminates magic numbers everywhere. Makes tuning physics, visuals, and layout trivial without hunting through source files.

#### 3. Implement a `ResourceManager`
```cpp
// ResourceManager.hpp
class ResourceManager {
public:
    static sf::Texture& getTexture(const std::string& path);
    static sf::Font     getFont(const std::string& path, unsigned int size);
    static sf::Sound    getSound(const std::string& path);

    static void reload(const std::string& path);
    static void clear();
private:
    static std::unordered_map<std::string, sf::Texture> textures;
    static std::unordered_map<std::string, sf::Font>    fonts;
    static std::unordered_map<std::string, sf::Sound>   sounds;
};
```
**Why:** Prevents redundant file I/O, ensures assets persist across resets, and centralizes asset management.

#### 4. Add "Press Space to Start" in START State
```cpp
else if (currentState == START) {
    sf::Text prompt(font);
    prompt.setString("Press SPACE to Start");
    prompt.setCharacterSize(36);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition({SCREEN_WIDTH / 2.f - 150.f, SCREEN_HEIGHT / 2.f});
    window.draw(prompt);

    // Also draw the bird centered for visual feedback
    bird.draw(window);
}
```
**Why:** Critical UX improvement — players need a clear affordance to begin.

---

### Medium Priority

#### 5. Make Bird X Position Configurable
```cpp
// Bird.hpp
float posX;

// Bird.cpp
Bird::Bird() : velocityY(0.0f), posX(Config::BIRD_START_X), posY(Config::BIRD_START_Y) {
    sprite.setSize({Config::BIRD_WIDTH, Config::BIRD_HEIGHT});
    sprite.setPosition({posX, posY});
}

void Bird::update(float dt) {
    velocityY += Config::GRAVITY * dt;
    posY += velocityY * dt;
    sprite.setPosition({posX, posY});
}
```

#### 6. Add Sound Effects
```cpp
// In main.cpp:
sf::SoundBuffer jumpBuffer("jump.wav");
sf::Sound jumpSound(jumpBuffer);
sf::SoundBuffer scoreBuffer("score.wav");
sf::Sound scoreSound(scoreBuffer);
sf::SoundBuffer deathBuffer("death.wav");
sf::Sound deathSound(deathBuffer);

// In the flap() branch:
jumpSound.play();

// In scoring:
scoreSound.play();

// On game over:
deathSound.play();
```

#### 7. Implement High Score System
```cpp
// HighScore.hpp
class HighScore {
public:
    static int load();
    static void save(int score);
    static int get();
private:
    static constexpr const char* PATH = "highscore.dat";
    static int cached;
};
```

#### 8. Add Visual Polish to Game Over
```cpp
// Dim the background on game over
else if (currentState == GAME_OVER) {
    sf::Rectangle overlay(sf::Vector2f(0, 0), sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    window.draw(overlay);

    bird.draw(window);
    for (const auto& pipe : pipes) pipe.draw(window);

    if (fontLoaded) {
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        gameOverText.setString("GAME OVER");
        window.draw(gameOverText);

        sf::Text restartText(font);
        restartText.setString("Press SPACE to Restart");
        restartText.setCharacterSize(28);
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition({SCREEN_WIDTH / 2.f - 130.f, SCREEN_HEIGHT / 2.f + 50.f});
        window.draw(restartText);
    }
}
```

---

### Low Priority

#### 9. Remove Debug Log
Delete this line from `main.cpp`:
```cpp
// Remove:
if (dt > 0.1f) std::cout << "Large dt: " << dt << std::endl;
```
Or wrap it in `#ifdef DEBUG`.

#### 10. Make Physics Tunable at Runtime
Add a config file (JSON or INI) to allow tweaking gravity, jump strength, pipe speed, and gap without recompiling:
```cpp
// ConfigLoader.hpp
class ConfigLoader {
public:
    static void load(const std::string& path);
    static float getGravity();
    static float getJumpStrength();
    // ...
};
```

#### 11. Add Sprite Animation for Bird
Replace the static bird texture with an animation frame sequence:
```cpp
class Bird {
    // ...
    sf::SpriteFrameAnimation animation;
    void animate(float dt);
};
```

#### 12. Add Ground/Floor Collision
Add a ground plane that the bird can collide with:
```cpp
constexpr float GROUND_Y = SCREEN_HEIGHT - 50.f;
if (birdBounds.position.y + birdBounds.size.y >= GROUND_Y) {
    currentState = GAME_OVER;
}
```

#### 13. Consider Switching to SFML 3 API
SFML 3 has breaking changes from SFML 2 (e.g., `sf::Vector2f` instead of raw floats, new resource loading API). If targeting SFML 3, check compatibility:
```cpp
// Check CMake: find_package(SFML 3 REQUIRED ...)
```

---

## 📋 Implementation Roadmap

| Phase | Tasks | Estimated Effort |
|-------|-------|-----------------|
| **Phase 1** | (1) Implement `Bird::load()` with `sf::Sprite` <br> (2) Create `Config.hpp` <br> (3) Remove debug log <br> (4) Fix `GAP_HEIGHT` inconsistency in `Pipe.hpp` <br> **(5) Update codebase for SFML 3.0 compatibility** ⭐ | 1-2 hours |

> ⭐ **Phase 1 Bonus:** During implementation, the codebase was updated to be fully compatible with SFML 3.0.2 (found on the system). Key changes:
> - `sf::Sprite` requires a texture at construction (no default constructor)
> - `sf::Image` constructor changed: `Image(Vector2u size, Color)` instead of `create()`
> - `sf::Texture` uses `loadFromFile()` (instance method) instead of `Texture::loadFromFile()` (static)
> - `sf::FloatRect` constructor takes `{position, size}` pairs instead of 4 floats
> - `sf::RenderTexture` uses `create({width, height})` instead of constructor args
> - `sf::Rectangle` removed — use `sf::RectangleShape` instead
| **Phase 2** | (5) Add "Press Space to Start" in START state <br> (6) Add visual polish to Game Over <br> (7) Make Bird X position configurable | 1-2 hours |

> ✅ **Phase 2 Complete:** All items implemented:
> - START state renders bird + "Press SPACE to Start" prompt
> - Game Over screen has semi-transparent overlay, restart prompt, and high score display
> - Bird X position is configurable via `Config::BIRD_START_X` (currently 50.0f)
| **Phase 3** | (8) Implement `ResourceManager` <br> (9) Add sound effects <br> (10) Implement high score system | 2-3 hours |

> ✅ **Phase 3 Complete:** All items implemented:
> - `ResourceManager` with texture, font, and sound caching via `std::shared_ptr<sf::SoundBuffer>`
> - Silent WAV fallback for missing sound files
> - `HighScore` with binary file persistence (`highscore.dat`)
> - Sound effects: jump, score, death sounds
> - High Score displayed in top-right during gameplay and centered on game over screen
| **Phase 4** | (11) Add ground collision <br> (12) Add sprite animation <br> (13) Config file for tuning | 3-4 hours |
| **Phase 5** | (14) Bird copy safety (delete copy ctor) <br> (15) Remove dead `fontLoaded` code <br> (16) Replace `rand()` with `<random>` <br> (17) Extract game-over helper lambda <br> (18) Return `const sf::Font&` from ResourceManager <br> (19) Dynamic scoring threshold from bird X <br> (20) Move difficulty caps to Config <br> (21) Wrap debug logs in `#ifdef DEBUG` <br> (22) Use `menuOptions.size()` instead of hardcoded `% 3` | 1 hour |

> ✅ **Phase 5 Complete:** All items implemented:
> - **Bird copy safety** — Deleted copy/move constructors & assignment operators on `Bird` (raw `sf::Sprite*` is now non-copyable)
> - **Removed dead `fontLoaded` code** — Variable, dead branch, and all `if (fontLoaded)` guards removed from `main.cpp`
> - **Consistent RNG** — Replaced `rand()` with `std::uniform_int_distribution<int>` using the existing RNG engine
> - **Game-over helper lambda** — `triggerGameOver` lambda in `main.cpp` eliminates ~20 lines of duplicated code
> - **`getFont()` returns `const sf::Font&`** — Avoids copying the font from the cache on every call
> - **Dynamic scoring threshold** — Scoring now compares `pipe.getX() < bird.getX()` instead of hardcoded `50.0f`
> - **Difficulty caps in Config** — `Config::PIPE_SPEED_MAX` and `Config::SPAWN_INTERVAL_MIN`, overridable via `gameconfig.json` (`pipe_speed_max`, `spawn_interval_min`)
> - **Debug logs guarded** — All `std::cout` debug output wrapped in `#ifdef DEBUG` (disabled by default)
> - **Menu navigation uses `menuOptions.size()`** — No more hardcoded `% 3` in menu wrapping

---

## 🐛 Bug Fixes Applied

### Critical

| # | Issue | Fix |
|---|-------|-----|
| 1 | **Pipe velocity direction** — pipes moved right (off-screen) instead of left | Removed redundant `velocityX = Config::PIPE_SPEED` assignment in Pipe constructor that overwrote the correct `-200` value |
| 2 | **Game restart double-free** — `bird = Bird()` shallow-copied `sf::Sprite*`, causing double-free on destruction | Added `Bird::reset()` method that properly destroys old sprite, resets all fields, then calls `load()` to create a fresh sprite |
| 3 | **Empty sound files** — WAV files were headers-only (8864 bytes, no audio data) | Generated real OGG Vorbis sounds via ffmpeg: jump (upward chirp), score (bright ding), death (descending buzzer) |
| 4 | **SFML WAV format not supported** — SFML's audio backend lacks WAV codec support | Replaced all `.wav` files with `.ogg` Vorbis; updated paths in main.cpp |

### Medium — ConfigLoader Fixes

| # | Issue | Fix |
|---|-------|-----|
| 5 | Key substring length off-by-one (`json.substr(key_start, key_end - key_start)`) | Added `+1` to include the last character: `json.substr(key_start, key_end - key_start + 1)` |
| 6 | Colon search loop stopped at wrong position (`json[i+1] != ':'`) | Changed to `json[i] != ':'` |
| 7 | String value parser didn't advance past closing quote | Added `i++` after detecting closing quote |
| 8 | `std::stof` locale-dependent decimal parsing | Replaced with `std::strtod` for consistent dot-decimal parsing |

### Phase 5 — Code Quality & Safety Fixes

| # | Issue | Fix |
|---|-------|-----|
| 9 | **Bird copy-unsafe** — `sf::Sprite*` with no copy control | Deleted copy/move constructors & assignment operators on `Bird`
| 10 | **`fontLoaded` dead code** — always-true flag with unreachable branch | Removed variable, dead branch, and all `if (fontLoaded)` guards
| 11 | **`rand()` inconsistency** — mixed C `rand()` with C++ `<random>` | Replaced with `std::uniform_int_distribution<int>` using existing engine
| 12 | **Duplicate game-over logic** — ~20 lines repeated across two collision branches | Extracted into `triggerGameOver` local lambda
| 13 | **`getFont()` returns by value** — copies entire `sf::Font` from cache | Changed return type to `const sf::Font&`
| 14 | **Hardcoded scoring threshold** — `pipe.getX() < 50.0f` assumes bird at X=50 | Changed to `pipe.getX() < bird.getX()`
| 15 | **Hardcoded difficulty caps** — magic numbers in game loop | Moved to `Config::PIPE_SPEED_MAX` / `Config::SPAWN_INTERVAL_MIN`, exposed in `gameconfig.json`
| 16 | **Debug logs in release builds** — `std::cout` spam on every score/difficulty change | Wrapped all debug output in `#ifdef DEBUG` (disabled by default)
| 17 | **Hardcoded `% 3` menu wrap** — breaks if menu options change | Changed to `% menuOptions.size()`
| 18 | **HighScore::reset() corrupts file** — writes 0 to disk every call | Changed to only clear in-memory cache, not touch file |
| 19 | **Bird placeholder missing origin** — else-branch in load() didn't set sprite origin | Added `setOrigin()` to match real-texture branch |

### Unit Tests (Google Test 1.17)

| Suite | Tests | Coverage |
|-------|-------|----------|
| `test_ConfigLoader` | 12 | JSON parsing, int/float/bool/string, escapes, fallbacks, clear/reload |
| `test_HighScore` | 11 | File persistence, caching, score comparison, reset semantics |
| `test_Bird` | 12 | Gravity, flap, bounding box, reset, velocity compounding |
| `test_Pipe` | 12 | Movement, speed, off-screen detection, moving pipes |
| `test_Collision` | 17 | Gap/no-gap, top/bottom pipe hits, edge overlap, moved/moving pipes |
| **Total** | **64** | **All passing** 🟢 |

Enable with: `cmake .. -DBUILD_TESTS=ON && make flappy_tests && ./tests/flappy_tests`

## 📊 Current State Summary

| Category | Status |
|----------|--------|
| Core Game Loop | ✅ Complete |
| Bird Physics | ✅ Complete |
| Pipe Spawning | ✅ Complete |
| Collision Detection | ✅ Complete |
| Scoring System | ✅ Complete |
| Game States (START/PLAYING/GAME_OVER) | ✅ Complete |
| Rendering (shapes) | ✅ Complete |
| Input Handling | ✅ Complete |
| **Texture/Sprite Rendering** | ✅ Implemented (with `sf::Sprite`) |
| **Resource Management** | ✅ Complete (`ResourceManager` with texture/font/sound caching) |
| **Audio** | ✅ Complete (jump, score, death sounds with silent fallback) |
| **High Score** | ✅ Complete (binary persistence via `highscore.dat`) |
| **Configurability** | ✅ Implemented (`Config.hpp` with all constants) |
| **UI Polish** | ✅ Complete (START prompt, GAME_OVER overlay, ground rendering) |
| **Ground Collision** | ✅ Complete (ground plane + collision check) |
| **Bird Animation** | ✅ Complete (velocity-based tilt + wing flap oscillation) |
| **Runtime Config** | ✅ Complete (JSON config loader, `gameconfig.json`) |
| **Background Music** | ✅ Complete (Plays at launch, persists, loop enabled, 40% vol) |
| **Bird Copy Safety** | ✅ Complete (copy/move deleted, `reset()` for restart) |
| **Scoring Correctness** | ✅ Complete (uses bird X, not hardcoded 50.0f) |
| **Debug Hygiene** | ✅ Complete (debug logs guarded by `#ifdef DEBUG`) |
| **Code Duplication** | ✅ Complete (game-over trigger extracted to lambda) |
| **ResourceManager Efficiency** | ✅ Complete (`getFont()` returns `const sf::Font&`) |
| **Difficulty Caps Configurable** | ✅ Complete (`gameconfig.json` overrides caps) |
| **Unit Tests** | ✅ Complete (64 tests, 5 suites, GTest, `-DBUILD_TESTS=ON`) |

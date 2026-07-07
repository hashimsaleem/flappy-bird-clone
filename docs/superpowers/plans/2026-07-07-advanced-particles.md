# Advanced Particles Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add ground dust, collision sparks, and score sparkle particle effects to Flappy Bird.

**Architecture:** Extend the existing `Particle` struct with a `ParticleType` enum to control color, size, lifetime, and physics. Add typed spawn methods to `ParticleSystem`. Integrate with `PlayState` at the right moments (bird near ground, on death, on scoring).

**Tech Stack:** C++23, SFML 3.0, Google Test, std::random

## Global Constraints

- All 197 existing tests must continue to pass (total: 203 after new tests)
- Only existing files modified — no new files created
- Follow existing test patterns: `TEST` macros, `EXPECT_EQ` assertions
- DRY, YAGNI, TDD, frequent commits
- `Particle::update()` must handle all 4 types including existing `Fire` for backward compatibility

---

### Task 1: Extend Particle struct with ParticleType enum

**Files:**
- Modify: `src/visual/Particle.hpp`

**Interfaces:**
- Consumes: nothing
- Produces: `enum class ParticleType { Fire, Dust, Spark, Bubble }`, typed constructor, `spawn()` that sets type

**Step 1: Rewrite Particle struct**

Replace `Particle.hpp` with:

```cpp
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>

enum class ParticleType {
    Fire,
    Dust,
    Spark,
    Bubble
};

struct Particle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    ParticleType type = ParticleType::Fire;
    float wobblePhase = 0.0f; // For sinusoidal effects
    std::mt19937 rng; // Per-particle RNG

    Particle(sf::Vector2f position, sf::Vector2f vel, float life, ParticleType t = ParticleType::Fire)
        : velocity(vel), lifetime(life), type(t), rng(std::random_device{}()) {
        shape.setSize({5.f, 5.f});
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(position);
    }

    void update(float dt) {
        shape.move(velocity * dt);
        lifetime -= dt;

        // Update wobble for bubble type
        if (type == ParticleType::Bubble) {
            wobblePhase += dt * 5.0f;
        }

        switch (type) {
            case ParticleType::Fire: {
                float alpha = std::max(0.f, lifetime * 255.f);
                shape.setFillColor(sf::Color(255, 255, 0, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Dust: {
                float alpha = std::max(0.f, lifetime * 400.f);
                shape.setFillColor(sf::Color(200, 190, 170, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Spark: {
                float alpha = std::max(0.f, lifetime * 800.f);
                shape.setFillColor(sf::Color(255, 140, 50, static_cast<unsigned char>(alpha)));
                break;
            }
            case ParticleType::Bubble: {
                float alpha = std::max(0.f, (lifetime - 0.1f) * 350.f);
                if (alpha < 0) alpha = 0;
                shape.setFillColor(sf::Color(255, 215, 0, static_cast<unsigned char>(alpha)));
                // Sinusoidal wobble
                float wobble = std::sin(wobblePhase) * 10.f;
                shape.move(sf::Vector2f(wobble * dt, 0.f));
                break;
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (lifetime > 0) {
            window.draw(shape);
        }
    }
};

#endif // PARTICLE_HPP
```

- [ ] **Step 2: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds (may show warnings about unused variables).

- [ ] **Step 3: Run existing tests**

Run: `cd build && ctest -R Particle --output-on-failure`
Expected: All 10 existing ParticleSystem tests pass.

- [ ] **Step 4: Commit**

```bash
git add src/visual/Particle.hpp
git commit -m "feat: add ParticleType enum to Particle struct"
```

---

### Task 2: Add typed spawn methods to ParticleSystem

**Files:**
- Modify: `src/visual/ParticleSystem.hpp`
- Modify: `src/visual/ParticleSystem.cpp`

**Interfaces:**
- Consumes: `Particle` struct with `ParticleType` enum
- Produces: `spawnDust(sf::Vector2f pos, int count)`, `spawnSparks(sf::Vector2f pos, int count)`, `spawnScoreSparkle(sf::Vector2f pos, int count)`

**Step 1: Update ParticleSystem.hpp**

Add these public method declarations after the existing `spawn()` method:

```cpp
    void spawnDust(sf::Vector2f pos, int count);
    void spawnSparks(sf::Vector2f pos, int count);
    void spawnScoreSparkle(sf::Vector2f pos, int count);
```

Full updated file:

```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "core/ObjectPool.h"
#include "Particle.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void spawnDust(sf::Vector2f pos, int count);
    void spawnSparks(sf::Vector2f pos, int count);
    void spawnScoreSparkle(sf::Vector2f pos, int count);
    std::vector<Particle> getParticles() const;
    size_t getActiveCount() const { return activeParticles.size(); }
private:
    std::unique_ptr<ObjectPool<Particle>> particlePool;
    std::vector<int> activeParticles;
};
```

**Step 2: Update ParticleSystem.cpp**

Replace the entire file with:

```cpp
#include "ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <random>

ParticleSystem::ParticleSystem() {
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void ParticleSystem::spawn(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    for (int i = 0; i < count; ++i) {
        float vx = (dist(rng) - 1.0f) * 200.0f;
        float vy = (dist(rng) - 1.0f) * 200.0f;
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setPosition(pos);
        p.velocity = velocity + sf::Vector2f(vx, vy);
        p.lifetime = 1.0f;
        p.type = ParticleType::Fire;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnDust(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> velDist(0.5f, 1.5f);
    std::uniform_real_distribution<float> angleDist(-0.2f, 0.2f); // Slight horizontal drift
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float angle = angleDist(rng);
        float speed = velDist(rng) * 30.0f;
        sf::Vector2f vel(std::cos(angle) * speed, 20.0f + std::abs(std::sin(angle)) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.3f + std::uniform_real_distribution<float>(0.0f, 0.2f)(std::random_device{}());
        p.type = ParticleType::Dust;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnSparks(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(3.0f, 5.0f);
    std::uniform_real_distribution<float> speedDist(200.0f, 600.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 6.283185f); // Full circle
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float speed = speedDist(rng);
        float angle = angleDist(rng);
        sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.2f + std::uniform_real_distribution<float>(0.0f, 0.2f)(std::random_device{}());
        p.type = ParticleType::Spark;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnScoreSparkle(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(4.0f, 6.0f);
    std::uniform_real_distribution<float> wobbleDist(-0.1f, 0.1f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = sf::Vector2f(wobbleDist(rng) * 100.f, -100.f); // Rises upward
        p.lifetime = 0.6f + std::uniform_real_distribution<float>(0.0f, 0.2f)(std::random_device{}());
        p.type = ParticleType::Bubble;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::update(float dt) {
    for (auto it = activeParticles.begin(); it != activeParticles.end(); ) {
        int idx = *it;
        Particle& p = (*particlePool)[idx];
        p.update(dt);
        if (p.lifetime <= 0) {
            particlePool->release(idx);
            it = activeParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (int idx : activeParticles) {
        (*particlePool)[idx].draw(window);
    }
}

std::vector<Particle> ParticleSystem::getParticles() const {
    std::vector<Particle> particles;
    for (int idx : activeParticles) {
        particles.push_back((*particlePool)[idx]);
    }
    return particles;
}
```

Wait — I need to add a member RNG to ParticleSystem so the spawn methods don't create a new RNG each call. Let me fix that.

**Revised ParticleSystem.hpp** (with RNG member):

```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include "core/ObjectPool.h"
#include "Particle.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void spawnDust(sf::Vector2f pos, int count);
    void spawnSparks(sf::Vector2f pos, int count);
    void spawnScoreSparkle(sf::Vector2f pos, int count);
    std::vector<Particle> getParticles() const;
    size_t getActiveCount() const { return activeParticles.size(); }
private:
    std::unique_ptr<ObjectPool<Particle>> particlePool;
    std::vector<int> activeParticles;
    std::random_device rd;
    std::mt19937 rng;
};
```

**Revised ParticleSystem.cpp** (using member RNG):

```cpp
#include "ParticleSystem.hpp"
#include "core/Config.hpp"
#include "core/ConfigLoader.hpp"
#include <random>

ParticleSystem::ParticleSystem() : rng(rd()) {
    particlePool = std::make_unique<ObjectPool<Particle>>([]() {
        return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f);
    });
}

void ParticleSystem::spawn(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    for (int i = 0; i < count; ++i) {
        float vx = (dist(rng) - 1.0f) * 200.0f;
        float vy = (dist(rng) - 1.0f) * 200.0f;
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setPosition(pos);
        p.velocity = velocity + sf::Vector2f(vx, vy);
        p.lifetime = 1.0f;
        p.type = ParticleType::Fire;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnDust(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> velDist(0.5f, 1.5f);
    std::uniform_real_distribution<float> angleDist(-0.2f, 0.2f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float angle = angleDist(rng);
        float speed = velDist(rng) * 30.0f;
        sf::Vector2f vel(std::cos(angle) * speed, 20.0f + std::abs(std::sin(angle)) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.3f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
        p.type = ParticleType::Dust;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnSparks(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(3.0f, 5.0f);
    std::uniform_real_distribution<float> speedDist(200.0f, 600.0f);
    std::uniform_real_distribution<float> angleDist(0.0f, 6.283185f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        float speed = speedDist(rng);
        float angle = angleDist(rng);
        sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
        
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = vel;
        p.lifetime = 0.2f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
        p.type = ParticleType::Spark;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::spawnScoreSparkle(sf::Vector2f pos, int count) {
    std::uniform_real_distribution<float> sizeDist(4.0f, 6.0f);
    std::uniform_real_distribution<float> wobbleDist(-0.1f, 0.1f);
    
    for (int i = 0; i < count; ++i) {
        float size = sizeDist(rng);
        int idx = particlePool->acquire();
        Particle& p = (*particlePool)[idx];
        p.shape.setSize({size, size});
        p.shape.setPosition(pos);
        p.velocity = sf::Vector2f(wobbleDist(rng) * 100.f, -100.f);
        p.lifetime = 0.6f + std::uniform_real_distribution<float>(0.0f, 0.2f)(rng);
        p.type = ParticleType::Bubble;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::update(float dt) {
    for (auto it = activeParticles.begin(); it != activeParticles.end(); ) {
        int idx = *it;
        Particle& p = (*particlePool)[idx];
        p.update(dt);
        if (p.lifetime <= 0) {
            particlePool->release(idx);
            it = activeParticles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (int idx : activeParticles) {
        (*particlePool)[idx].draw(window);
    }
}

std::vector<Particle> ParticleSystem::getParticles() const {
    std::vector<Particle> particles;
    for (int idx : activeParticles) {
        particles.push_back((*particlePool)[idx]);
    }
    return particles;
}
```

- [ ] **Step 1: Write the updated files**

Replace `src/visual/ParticleSystem.hpp` and `src/visual/ParticleSystem.cpp` with the content shown above.

- [ ] **Step 2: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds.

- [ ] **Step 3: Run existing ParticleSystem tests**

Run: `cd build && ctest -R ParticleSystem --output-on-failure`
Expected: All 10 existing tests pass.

- [ ] **Step 4: Commit**

```bash
git add src/visual/ParticleSystem.hpp src/visual/ParticleSystem.cpp
git commit -m "feat: add typed particle spawn methods (dust, sparks, score sparkle)"
```

---

### Task 3: Add particle effect tests

**Files:**
- Modify: `tests/test_ParticleSystem.cpp` (append 6 new tests)

**Interfaces:**
- Consumes: `spawnDust()`, `spawnSparks()`, `spawnScoreSparkle()`, `update()`, `getParticles()`, `getActiveCount()` from Task 2

**Step 1: Append 6 tests to test_ParticleSystem.cpp**

Add these at the end of the file:

```cpp
// --- spawnDust creates dust particles ---

TEST(ParticleSystemTest, SpawnDustCreatesParticles) {
    ParticleSystem system;
    system.spawnDust(sf::Vector2f(100.f, 500.f), 4);
    EXPECT_EQ(system.getActiveCount(), 4u);
}

// --- Dust particles float downward ---

TEST(ParticleSystemTest, DustFloatsDownward) {
    ParticleSystem system;
    system.spawnDust(sf::Vector2f(100.f, 100.f), 1);
    auto particles = system.getParticles();
    // Dust velocity has positive Y component (downward)
    EXPECT_GT(particles[0].velocity.y, 0.f);
}

// --- spawnSparks creates spark particles ---

TEST(ParticleSystemTest, SpawnSparksCreatesParticles) {
    ParticleSystem system;
    system.spawnSparks(sf::Vector2f(200.f, 200.f), 6);
    EXPECT_EQ(system.getActiveCount(), 6u);
}

// --- Sparks have explosive burst velocities ---

TEST(ParticleSystemTest, SparksExplosiveBurst) {
    ParticleSystem system;
    system.spawnSparks(sf::Vector2f(300.f, 300.f), 10);
    auto particles = system.getParticles();
    // All sparks should have velocity magnitude between 200 and 600
    for (const auto& p : particles) {
        float speed = std::sqrt(p.velocity.x * p.velocity.x + p.velocity.y * p.velocity.y);
        EXPECT_GE(speed, 200.f);
        EXPECT_LE(speed, 600.f);
    }
}

// --- spawnScoreSparkle creates bubble particles ---

TEST(ParticleSystemTest, SpawnScoreSparkleCreatesParticles) {
    ParticleSystem system;
    system.spawnScoreSparkle(sf::Vector2f(400.f, 100.f), 3);
    EXPECT_EQ(system.getActiveCount(), 3u);
}

// --- Score bubbles rise upward ---

TEST(ParticleSystemTest, ScoreBubbleRisesUpward) {
    ParticleSystem system;
    system.spawnScoreSparkle(sf::Vector2f(500.f, 300.f), 1);
    auto particles = system.getParticles();
    // Bubble velocity has negative Y component (upward)
    EXPECT_LT(particles[0].velocity.y, 0.f);
}
```

- [ ] **Step 2: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds.

- [ ] **Step 3: Run all ParticleSystem tests**

Run: `cd build && ctest -R ParticleSystem --output-on-failure`
Expected: 16/16 tests pass (10 existing + 6 new).

- [ ] **Step 4: Commit**

```bash
git add tests/test_ParticleSystem.cpp
git commit -m "test: add 6 particle effect tests (dust, sparks, score sparkle)"
```

---

### Task 4: Integrate particle effects into PlayState

**Files:**
- Modify: `src/states/PlayState.h` (add dust spawn timer)
- Modify: `src/states/PlayState.cpp` (add dust, sparks, score sparkles)

**Interfaces:**
- Consumes: `VisualEffectManager::spawnDust()`, `spawnSparks()`, `spawnScoreSparkle()`
- Produces: dust trail behind bird, sparks on death, sparkles on score

**Step 1: Add dust spawn timer to PlayState.h**

Add this private member to the `PlayState` class (after `float currentPipeSpeed;`):

```cpp
    float dustSpawnTimer = 0.f;
```

**Step 2: Update PlayState.cpp integration points**

We need to find and modify the right places in PlayState.cpp. Let me read it first.

**Step 2a: Read PlayState.cpp**

Read `src/states/PlayState.cpp` to find:
- Where the bird updates (for dust spawning)
- Where `triggerGameOver()` is (for sparks)
- Where scoring happens (for score sparkle)

**Step 2b: Add dust spawning**

In `PlayState::update()`, after the bird position check and before pipe spawning, add:

```cpp
// Dust trail when bird is near ground
float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
if (bird.getPosition().y + Config::BIRD_HEIGHT / 2.f > groundY - 20.f) {
    dustSpawnTimer += dt;
    if (dustSpawnTimer > 0.03f) {  // ~30ms between dust bursts
        dustSpawnTimer = 0.f;
        visualEffects->spawnDust({bird.getPosition().x, groundY - 5.f}, 1);
    }
}
```

**Step 2c: Add sparks on death**

In `PlayState::triggerGameOver()`, add before the existing code:

```cpp
visualEffects->spawnSparks({bird.getPosition().x, bird.getPosition().y}, 15);
```

**Step 2d: Add score sparkle**

In `PlayState::update()`, where the score is incremented (search for "score++" or "onScore"), add:

```cpp
visualEffects->spawnScoreSparkle({scoreManager->getScorePosition().x, scoreManager->getScorePosition().y}, 3);
```

If `getScorePosition()` doesn't exist on ScoreManager, use `{Config::SCREEN_WIDTH / 2.f, Config::BIRD_START_Y - 30.f}`.

- [ ] **Step 1: Read PlayState.cpp**

Read `src/states/PlayState.cpp` to identify the exact lines for integration.

- [ ] **Step 2: Apply integration changes**

Apply the changes described in Steps 2a, 2b, 2c, 2d above.

- [ ] **Step 3: Build to verify**

Run: `cd build && cmake --build . 2>&1 | tail -10`
Expected: Build succeeds.

- [ ] **Step 4: Run all tests**

Run: `cd build && ctest --output-on-failure`
Expected: 203/203 tests passing (197 existing + 6 new).

- [ ] **Step 5: Commit**

```bash
git add src/states/PlayState.h src/states/PlayState.cpp
git commit -m "feat: integrate ground dust, collision sparks, and score sparkles into gameplay"
```

---

**Plan complete and saved to `docs/superpowers/plans/2026-07-07-advanced-particles.md`.**

Two execution options:

1. **Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

2. **Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

Which approach?

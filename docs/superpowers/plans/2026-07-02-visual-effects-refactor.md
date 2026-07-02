# Visual Effects Refactoring Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Decouple visual effects (clouds, particles, environment) from `PlayState` into a dedicated `VisualEffectManager`.

**Architecture:** Create a `VisualEffectManager` that owns three sub-systems: `CloudSystem`, `ParticleSystem`, and `EnvironmentRenderer`. `PlayState` will interact only with the manager.

**Tech Stack:** C++, SFML, Object Pools.

## Global Constraints

- Follow existing naming conventions (PascalCase for classes).
- Use `std::unique_ptr` for ownership of sub-systems within the manager.
- Ensure no regressions in visual behavior (speed, colors, alpha).

---

### Task 1: Create EnvironmentRenderer

**Files:**
- Create: `src/EnvironmentRenderer.hpp`
- Create: `src/EnvironmentRenderer.cpp`

**Interfaces:**
- Consumes: None
- Produces: `void draw(sf::RenderWindow& window, float scrollOffset)`

- [x] **Step 1: Write implementation for EnvironmentRenderer**

```cpp
// src/EnvironmentRenderer.hpp
#pragma once
#include <SFML/Graphics.hpp>

class EnvironmentRenderer {
public:
    void draw(sf::RenderWindow& window, float scrollOffset);
};

// src/EnvironmentRenderer.cpp
#include "EnvironmentRenderer.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include <cmath>

void EnvironmentRenderer::draw(sf::RenderWindow& window, float scrollOffset) {
    float totalW = static_cast<float>(Config::SCREEN_WIDTH);
    float groundY = static_cast<float>(Config::SCREEN_HEIGHT - ConfigLoader::getFloat("ground_height", Config::GROUND_HEIGHT));
    float scroll = std::fmod(scrollOffset, Config::GROUND_TILE_W);

    sf::RectangleShape base;
    base.setSize(sf::Vector2f(totalW, static_cast<float>(Config::GROUND_HEIGHT)));
    base.setPosition(sf::Vector2f(0.f, groundY));
    base.setFillColor(Config::GROUND_COLOR);
    window.draw(base);

    for (float x = -scroll - Config::GROUND_TILE_W; x < totalW + Config::GROUND_TILE_W; x += Config::GROUND_TILE_W) {
        int idx = static_cast<int>(std::floor((x + scroll) / Config::GROUND_TILE_W)) % Config::GROUND_TILE_COUNT;
        if (idx < 0) idx += Config::GROUND_TILE_COUNT;
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(static_cast<float>(Config::GROUND_TILE_W), 8.f));
        tile.setPosition(sf::Vector2f(x, groundY));
        tile.setFillColor(Config::GROUND_TOP_COLORS[idx]);
        window.draw(tile);
    }

    float grassY = groundY - 2.f;
    for (float x = -scroll; x < totalW; x += 12.f) {
        float h = std::sin(x * 0.5f) * 2.f + 3.f;
        sf::RectangleShape blade;
        blade.setSize(sf::Vector2f(2.f, h));
        blade.setPosition(sf::Vector2f(x, grassY - h));
        blade.setFillColor(sf::Color(80, 140, 60));
        window.draw(blade);
    }

    // Background layers
    float bgOffset = 0.0f; // To be updated by PlayState or passed in
    sf::RectangleShape bgLayer;
    bgLayer.setSize(sf::Vector2f(static_cast<float>(Config::SCREEN_WIDTH), 150.f));
    bgLayer.setPosition(sf::Vector2f(bgOffset, groundY - 150.f));
    bgLayer.setFillColor(sf::Color(100, 150, 100));
    window.draw(bgLayer);

    bgLayer.setPosition(sf::Vector2f(bgOffset + static_cast<float>(Config::SCREEN_WIDTH), groundY - 150.f));
    window.draw(bgLayer);
}
```

- [x] **Step 2: Commit**

### Task 2: Create CloudSystem

**Files:**
- Create: `src/CloudSystem.hpp`
- Create: `src/CloudSystem.cpp`

**Interfaces:**
- Consumes: None
- Produces: `void update(float dt)`, `void draw(sf::RenderWindow& window)`

- [ ] **Step 1: Write implementation for CloudSystem**

```cpp
// src/CloudSystem.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Cloud {
    float x, y, speed, radius, alpha;
};

class CloudSystem {
public:
    CloudSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
private:
    std::vector<Cloud> clouds;
    float cloudOffset = 0.f;
};

// src/CloudSystem.cpp
#include "CloudSystem.hpp"
#include "Config.hpp"
#include <cstdlib>

CloudSystem::CloudSystem() {
    for (int i = 0; i < 6; i++) {
        Cloud c;
        c.x = (static_cast<float>(i) / 6.f) * Config::SCREEN_WIDTH + static_cast<float>(rand() % 100);
        c.y = 30.f + static_cast<float>(rand() % 120);
        c.speed = 15.f + static_cast<float>(rand() % 25);
        c.radius = 20.f + static_cast<float>(rand() % 40);
        c.alpha = static_cast<unsigned char>(60 + rand() % 80);
        clouds.push_back(c);
    }
}

void CloudSystem::update(float dt) {
    cloudOffset += Config::BACKGROUND_SPEED * dt;
}

void CloudSystem::draw(sf::RenderWindow& window) {
    for (const auto& c : clouds) {
        float cx = std::fmod(c.x + cloudOffset * (c.speed / 40.f), Config::SCREEN_WIDTH + c.radius * 2.f) - c.radius;
        sf::CircleShape shape(c.radius);
        shape.setFillColor(sf::Color(255, 255, 255, c.alpha));
        shape.setPosition({cx, c.y});
        window.draw(shape);
        sf::CircleShape shape2(c.radius * 0.7f);
        shape2.setFillColor(sf::Color(255, 255, 255, c.alpha - 20));
        shape2.setPosition({cx + c.radius * 0.5f, c.y - c.radius * 0.3f});
        window.draw(shape2);
    }
}
```

- [ ] **Step 2: Commit**

### Task 3: Create ParticleSystem

**Files:**
- Create: `src/ParticleSystem.hpp`
- Create: `src/ParticleSystem.cpp`

**Interfaces:**
- Consumes: None (Uses existing `ObjectPool<Particle>`)
- Produces: `void update(float dt)`, `void draw(sf::RenderWindow& window)`, `void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity)`

- [ ] **Step 1: Write implementation for ParticleSystem**

```cpp
// src/ParticleSystem.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ObjectPool.hpp" // Assuming existing path

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    sf::RectangleShape shape;
};

class ParticleSystem {
public:
    ParticleSystem();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawn(sf::Vector2f pos, int count, sf::Vector2f velocity);
private:
    std::unique_ptr<ObjectPool<Particle>> particlePool;
    std::vector<int> activeParticles;
};

// src/ParticleSystem.cpp
#include "ParticleSystem.hpp"
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
        float vx = (dist(rng) - 1.0f) * 200.0f; // Adjusted for example
        float vy = (dist(rng) - 1.0f) * 200.0f;
        int idx = particlePool->acquire();
        (*particlePool)[idx].shape.setPosition(pos);
        (*particlePool)[idx].velocity = {vx, vy};
        (*particlePool)[idx].lifetime = 1.0f;
        activeParticles.push_back(idx);
    }
}

void ParticleSystem::update(float dt) {
    for (auto it = activeParticles.begin(); it != activeParticles.end(); ) {
        int idx = *it;
        Particle& p = (*particlePool)[idx];
        p.position += p.velocity * dt;
        p.lifetime -= dt;
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
        (*particlePool)[idx].shape.draw(window);
    }
}
```

- [ ] **Step 2: Commit**

### Task 4: Create VisualEffectManager

**Files:**
- Create: `src/VisualEffectManager.hpp`
- Create: `src/VisualEffectManager.cpp`

**Interfaces:**
- Consumes: `CloudSystem`, `ParticleSystem`, `EnvironmentRenderer`
- Produces: `update(float dt)`, `draw(sf::RenderWindow& window)`, `spawnParticles(...)`

- [ ] **Step 1: Write implementation for VisualEffectManager**

```cpp
// src/VisualEffectManager.hpp
#pragma once
#include <memory>
#include "CloudSystem.hpp"
#include "ParticleSystem.hpp"
#include "EnvironmentRenderer.hpp"

class VisualEffectManager {
public:
    VisualEffectManager();
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity);
    void setGroundScrollOffset(float offset) { groundScrollOffset = offset; }

private:
    std::unique_ptr<CloudSystem> clouds;
    std::unique_ptr<ParticleSystem> particles;
    std::unique_ptr<EnvironmentRenderer> environment;
    float groundScrollOffset = 0.f;
};

// src/VisualEffectManager.cpp
#include "VisualEffectManager.hpp"

VisualEffectManager::VisualEffectManager() {
    clouds = std::make_unique<CloudSystem>();
    particles = std::make_unique<ParticleSystem>();
    environment = std::make_unique<EnvironmentRenderer>();
}

void VisualEffectManager::update(float dt) {
    clouds->update(dt);
    particles->update(dt);
}

void VisualEffectManager::draw(sf::RenderWindow& window) {
    clouds->draw(window);
    particles->draw(window);
    environment->draw(window, groundScrollOffset);
}

void VisualEffectManager::spawnParticles(sf::Vector2f pos, int count, sf::Vector2f velocity) {
    particles->spawn(pos, count, velocity);
}
```

- [ ] **Step 2: Commit**

### Task 5: Refactor PlayState

**Files:**
- Modify: `src/PlayState.h` (Remove cloud/particle members, add `VisualEffectManager`)
- Modify: `src/PlayState.cpp` (Replace logic with manager calls)

- [ ] **Step 1: Update PlayState header and constructor**

- [ ] **Step 2: Replace Cloud rendering in draw()**

- [ ] **Step 3: Replace Particle logic in triggerGameOver() and update()**

- [ ] **Step 4: Replace Ground rendering in draw()**

- [ ] **Step 5: Commit**

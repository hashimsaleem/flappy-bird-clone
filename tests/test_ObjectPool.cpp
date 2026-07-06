#include <gtest/gtest.h>
#include "core/ObjectPool.h"
#include "entities/Pipe.hpp"
#include "visual/Particle.hpp"

// --- Object Pool Tests ---

TEST(ObjectPoolTest, PoolAcquiresAndReleases) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int firstIdx = pool.acquire();
    EXPECT_GE(firstIdx, 0);
    EXPECT_LT(firstIdx, 20); // Default size is 20 in our logic? No, ObjectPool doesn't have a fixed size in header but we pre-allocate.

    // Check if it's actually "new" (we can't easily check uniqueness without more state)
    // But we can check that releasing and re-acquiring works.
    pool.release(firstIdx);
    int secondIdx = pool.acquire();
    EXPECT_EQ(firstIdx, secondIdx); // Should reuse the same slot if it was the only one released
}

TEST(ObjectPoolTest, PoolHandlesMultipleAcquisitions) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int idx1 = pool.acquire();
    int idx2 = pool.acquire();
    EXPECT_NE(idx1, idx2);

    pool.release(idx1);
    int idx3 = pool.acquire();
    EXPECT_EQ(idx1, idx3); // Should reuse idx1
}

TEST(ObjectPoolTest, ParticlePoolWorks) {
    auto factory = []() { return Particle({0.f, 0.f}, {0.f, 0.f}, 1.0f); };
    ObjectPool<Particle> pool(factory);

    int idx = pool.acquire();
    Particle& p = pool[idx];
    EXPECT_FLOAT_EQ(p.lifetime, 1.0f);
    
    p.update(0.5f);
    EXPECT_FLOAT_EQ(p.lifetime, 0.5f);

    pool.release(idx);
}

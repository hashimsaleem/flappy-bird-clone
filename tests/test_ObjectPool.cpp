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

// --- ObjectPool free-list O(1) acquire tests ---

TEST(ObjectPoolTest, FreeListLIFOOrder) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int idx0 = pool.acquire();
    int idx1 = pool.acquire();
    int idx2 = pool.acquire();

    pool.release(idx0);
    pool.release(idx1);
    pool.release(idx2);

    int r0 = pool.acquire();
    int r1 = pool.acquire();
    int r2 = pool.acquire();

    EXPECT_EQ(r0, idx2);
    EXPECT_EQ(r1, idx1);
    EXPECT_EQ(r2, idx0);
}

TEST(ObjectPoolTest, FreeListInterleavedAcquireRelease) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int a0 = pool.acquire();
    int a1 = pool.acquire();
    int a2 = pool.acquire();

    pool.release(a0);
    int b0 = pool.acquire();
    pool.release(a1);
    int b1 = pool.acquire();
    pool.release(a2);

    int c0 = pool.acquire();
    int c1 = pool.acquire();
    int c2 = pool.acquire();

    EXPECT_EQ(b0, a0);
    EXPECT_EQ(b1, a1);
    EXPECT_EQ(c0, a2);
    EXPECT_NE(c1, a0);
    EXPECT_NE(c1, a1);
    EXPECT_NE(c1, a2);
    EXPECT_NE(c2, a0);
    EXPECT_NE(c2, a1);
    EXPECT_NE(c2, a2);
}

TEST(ObjectPoolTest, FreeListExhaustedGrowsPool) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int idx0 = pool.acquire();
    int idx1 = pool.acquire();
    pool.release(idx0);
    pool.release(idx1);

    int r0 = pool.acquire();
    int r1 = pool.acquire();
    int r2 = pool.acquire();

    EXPECT_EQ(r0, idx1);
    EXPECT_EQ(r1, idx0);
    EXPECT_NE(r2, idx0);
    EXPECT_NE(r2, idx1);
}

TEST(ObjectPoolTest, FreeListActiveIndicesCorrect) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int a0 = pool.acquire();
    int a1 = pool.acquire();
    int a2 = pool.acquire();

    pool.release(a0);
    pool.release(a2);

    auto active = pool.getActiveIndices();
    EXPECT_EQ(active.size(), 1);
    EXPECT_EQ(active[0], a1);
}

TEST(ObjectPoolTest, FreeListNoDoubleRelease) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    int idx = pool.acquire();
    pool.release(idx);
    pool.release(idx);

    auto active = pool.getActiveIndices();
    EXPECT_EQ(active.size(), 0);
}

TEST(ObjectPoolTest, FreeListPoolSizeTrack) {
    auto factory = []() { return Pipe(0, 0, 150.f, 200.f, PipeType::STATIC); };
    ObjectPool<Pipe> pool(factory);

    EXPECT_EQ(pool.size(), 0u);
    int idx0 = pool.acquire();
    EXPECT_EQ(pool.size(), 1u);
    int idx1 = pool.acquire();
    EXPECT_EQ(pool.size(), 2u);
    pool.release(idx0);
    pool.release(idx1);
    pool.acquire();
    pool.acquire();
    EXPECT_EQ(pool.size(), 2u);
}

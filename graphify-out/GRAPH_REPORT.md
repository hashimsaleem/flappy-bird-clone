# Graph Report - .  (2026-07-02)

## Corpus Check
- 60 files · ~97,569 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 833 nodes · 1798 edges · 38 communities
- Extraction: 97% EXTRACTED · 3% INFERRED · 0% AMBIGUOUS · INFERRED: 48 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Community 0|Community 0]]
- [[_COMMUNITY_Community 1|Community 1]]
- [[_COMMUNITY_Community 2|Community 2]]
- [[_COMMUNITY_Community 3|Community 3]]
- [[_COMMUNITY_Community 4|Community 4]]
- [[_COMMUNITY_Community 5|Community 5]]
- [[_COMMUNITY_Community 6|Community 6]]
- [[_COMMUNITY_Community 7|Community 7]]
- [[_COMMUNITY_Community 8|Community 8]]
- [[_COMMUNITY_Community 9|Community 9]]
- [[_COMMUNITY_Community 10|Community 10]]
- [[_COMMUNITY_Community 11|Community 11]]
- [[_COMMUNITY_Community 12|Community 12]]
- [[_COMMUNITY_Community 13|Community 13]]
- [[_COMMUNITY_Community 14|Community 14]]
- [[_COMMUNITY_Community 15|Community 15]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 18|Community 18]]
- [[_COMMUNITY_Community 19|Community 19]]
- [[_COMMUNITY_Community 20|Community 20]]
- [[_COMMUNITY_Community 21|Community 21]]
- [[_COMMUNITY_Community 22|Community 22]]
- [[_COMMUNITY_Community 23|Community 23]]
- [[_COMMUNITY_Community 24|Community 24]]
- [[_COMMUNITY_Community 25|Community 25]]
- [[_COMMUNITY_Community 26|Community 26]]
- [[_COMMUNITY_Community 27|Community 27]]
- [[_COMMUNITY_Community 28|Community 28]]
- [[_COMMUNITY_Community 29|Community 29]]
- [[_COMMUNITY_Community 30|Community 30]]
- [[_COMMUNITY_Community 31|Community 31]]
- [[_COMMUNITY_Community 32|Community 32]]
- [[_COMMUNITY_Community 33|Community 33]]
- [[_COMMUNITY_Community 34|Community 34]]
- [[_COMMUNITY_Community 35|Community 35]]
- [[_COMMUNITY_Community 36|Community 36]]

## God Nodes (most connected - your core abstractions)
1. `PlayState` - 73 edges
2. `Bird` - 43 edges
3. `basic_json()` - 41 edges
4. `TEST_F()` - 37 edges
5. `operator<()` - 36 edges
6. `create()` - 31 edges
7. `handle_value()` - 31 edges
8. `Pipe` - 29 edges
9. `push_back()` - 29 edges
10. `json_sax_dom_callback_parser` - 28 edges

## Surprising Connections (you probably didn't know these)
- `TEST_F()` --calls--> `load`  [INFERRED]
  tests/test_Integration.cpp → src/Bird.hpp
- `TEST_F()` --calls--> `flap`  [INFERRED]
  tests/test_Integration.cpp → src/Bird.hpp
- `TEST_F()` --calls--> `getBoundingBox`  [INFERRED]
  tests/test_Integration.cpp → src/Bird.hpp
- `TEST_F()` --calls--> `update`  [INFERRED]
  tests/test_Integration.cpp → src/Pipe.hpp
- `TEST_F()` --calls--> `update`  [INFERRED]
  tests/test_Pipe.cpp → src/Pipe.hpp

## Import Cycles
- None detected.

## Communities (38 total, 0 thin omitted)

### Community 0 - "Community 0"
Cohesion: 0.06
Nodes (43): binary_t, lexer_t, number_float_t, number_integer_t, number_unsigned_t, NumberType, parser_callback_t, back() (+35 more)

### Community 1 - "Community 1"
Cohesion: 0.05
Nodes (53): BirdFlapUpdatesVelocity, BirdInitialState, ClearResetsState, CollisionTriggersDeath, ConfigHandlesLargeNumbers, ConfigHasAllRequiredKeys, DotNotationAccessesNestedKeys, EmptyJsonReturnsEmpty (+45 more)

### Community 2 - "Community 2"
Cohesion: 0.07
Nodes (31): BirdState, difficulty, flapTimer, isDying, posX, posY, tiltAngle, velocityY (+23 more)

### Community 3 - "Community 3"
Cohesion: 0.04
Nodes (50): default_random_engine, Font, Music, Sound, StateAction, unique_ptr, Vector2f, PlayState (+42 more)

### Community 4 - "Community 4"
Cohesion: 0.07
Nodes (31): BasicJsonContext, exception, json_value, NLOHMANN_BASIC_JSON_TPL, NLOHMANN_BASIC_JSON_TPL_DECLARATION, NLOHMANN_JSON_NAMESPACE_BEGIN, NLOHMANN_JSON_NAMESPACE_END, position_t (+23 more)

### Community 5 - "Community 5"
Cohesion: 0.08
Nodes (30): DefaultScoreIsZero, GetReturnsCached, IncreasingScoreUpdates, LargeScore, LoadReReadsAfterReset, NegativeDoesNotOverwritePositive, NegativeScoreRejected, OnlyHighestScoreIsKept (+22 more)

### Community 6 - "Community 6"
Cohesion: 0.08
Nodes (30): GetFontReturnsCachedInstance, GetFontReturnsNonNull, GetMusicReturnsCachedInstance, GetMusicReturnsNonNull, GetSoundReturnsCachedInstance, GetSoundReturnsNonNull, SoundBuffer, Music (+22 more)

### Community 7 - "Community 7"
Cohesion: 0.09
Nodes (29): Allocator, ExceptionType, false_type, initializer_list, It, adl_serializer, array(), clear() (+21 more)

### Community 8 - "Community 8"
Cohesion: 0.08
Nodes (26): CheckCollisionWorks, HasCorrectType, IsOffScreenWorks, FloatRect, PowerUpType, RenderWindow, PowerUpType, Sprite (+18 more)

### Community 9 - "Community 9"
Cohesion: 0.07
Nodes (23): Bird, birdFlapDepth, birdFlapRate, birdMaxTilt, birdMinTilt, birdTexture, flapTimer, gravity (+15 more)

### Community 10 - "Community 10"
Cohesion: 0.11
Nodes (26): aabbOverlap(), FloatRect, PipeType, RenderWindow, PipeType, RectangleShape, Pipe, baseY (+18 more)

### Community 11 - "Community 11"
Cohesion: 0.15
Nodes (24): BasicJsonType, byte_container_with_subtype, const_reference, initializer_list_t, JSON_HEDLEY_RETURNS_NON_NULL, partial_ordering, reference, RefStringTypeLhs (+16 more)

### Community 12 - "Community 12"
Cohesion: 0.09
Nodes (21): BirdAboveTopPipeNoCollision, BirdBelowBottomPipeNoCollision, BirdFarLeftNoCollision, BirdFarRightNoCollision, BirdHittingBottomPipe, BirdHittingTopPipe, BirdInGapNoCollision, BirdJustAboveBottomPipeNoCollision (+13 more)

### Community 13 - "Community 13"
Cohesion: 0.21
Nodes (24): char_int_type, InputType, IteratorType, JSON_HEDLEY_WARN_UNUSED_RESULT, SAX, size_t, span_input_adapter, accept() (+16 more)

### Community 14 - "Community 14"
Cohesion: 0.11
Nodes (16): BecomesDeadAfterTimeout, CorrectValue, InitialState, Font, RenderWindow, Text, Vector2f, ScoreFloat (+8 more)

### Community 15 - "Community 15"
Cohesion: 0.11
Nodes (15): Key, Music, Sound, StateAction, MenuState, bgmLoaded, blinkTimer, deathSound (+7 more)

### Community 16 - "Community 16"
Cohesion: 0.26
Nodes (19): key_type, KeyType, private, public, ReturnType, size_type, cend(), contains() (+11 more)

### Community 17 - "Community 17"
Cohesion: 0.12
Nodes (15): BoundingBoxCenteredOnPosition, BoundingBoxHasCorrectSize, FlapMovesBirdUpward, FlapResetsVelocity, FlapSetsJumpVelocity, GetVelocityReflectsGravity, GravityIncreasesVelocityDownward, GravityMovesBirdDownward (+7 more)

### Community 18 - "Community 18"
Cohesion: 0.13
Nodes (15): CustomSpeed, IsNotOffScreenInitially, IsOffScreenAfterMovingPastLeftEdge, IsOffScreenBoundary, LargeGapPipe, MovingPipeStillMovesLeft, PassedFlagStartsFalse, StartsAtGivenX (+7 more)

### Community 19 - "Community 19"
Cohesion: 0.15
Nodes (12): ObjectPoolTest, ParticlePoolWorks, PoolAcquiresAndReleases, PoolHandlesMultipleAcquisitions, RectangleShape, RenderWindow, Vector2f, Particle (+4 more)

### Community 20 - "Community 20"
Cohesion: 0.19
Nodes (15): array_t, boolean_t, friend, object_t, get_impl_ptr(), get_ptr(), is_binary(), is_boolean() (+7 more)

### Community 21 - "Community 21"
Cohesion: 0.29
Nodes (15): const_iterator, iteration_proxy, iterator, ptrdiff_t, begin(), cbegin(), emplace(), front() (+7 more)

### Community 22 - "Community 22"
Cohesion: 0.24
Nodes (10): function, Index, T, vector, ObjectPool, cleanup, factory, INVALID (+2 more)

### Community 23 - "Community 23"
Cohesion: 0.18
Nodes (14): Font, Key, Music, RenderWindow, Sound, string, draw, drawGround (+6 more)

### Community 24 - "Community 24"
Cohesion: 0.27
Nodes (13): string, getExeDir(), main(), Font, Music, Sound, string, unique_ptr (+5 more)

### Community 25 - "Community 25"
Cohesion: 0.17
Nodes (11): animateTilt, draw, flap, getBoundingBox, load, setRestartPos, setRestartVel, update (+3 more)

### Community 26 - "Community 26"
Cohesion: 0.18
Nodes (12): CompatibleType, InputIt, JsonRef, priority_tag, basic_json(), flatten(), get_impl(), noexcept() (+4 more)

### Community 27 - "Community 27"
Cohesion: 0.22
Nodes (7): Color, Font, RenderWindow, Text, Vector2f, Font, RenderWindow

### Community 28 - "Community 28"
Cohesion: 0.31
Nodes (11): output_adapter, decode(), hex_bytes(), vector, to_bjdata(), to_bson(), to_cbor(), to_msgpack() (+3 more)

### Community 29 - "Community 29"
Cohesion: 0.20
Nodes (6): Font, Key, RenderWindow, StateAction, HighScoreScreenState, returnToMenu

### Community 30 - "Community 30"
Cohesion: 0.25
Nodes (7): CreateGameOverStateReturnsNonNull, CreateHighScoreScreenStateReturnsNonNull, CreateMenuStateReturnsNonNull, CreatePlayStateReturnsNonNull, testing::Test, StateFactoryTest, TEST_F()

### Community 31 - "Community 31"
Cohesion: 0.22
Nodes (5): GameState, draw, handleKeyPress, update, StateAction

### Community 32 - "Community 32"
Cohesion: 0.22
Nodes (9): shared_ptr, vector, PlayStateSnapshot, birdState, difficulty, particles, pipes, score (+1 more)

### Community 33 - "Community 33"
Cohesion: 0.53
Nodes (6): const_reverse_iterator, reverse_iterator, crbegin(), crend(), rbegin(), rend()

### Community 34 - "Community 34"
Cohesion: 0.33
Nodes (6): Cloud, alpha, radius, speed, x, y

### Community 35 - "Community 35"
Cohesion: 0.50
Nodes (4): ENUM_TYPE, from_json(), to_json(), TargetType

### Community 36 - "Community 36"
Cohesion: 0.67
Nodes (3): compares_unordered, else, if()

## Knowledge Gaps
- **160 isolated node(s):** `sprite`, `birdTexture`, `placeholderTexture`, `velocityY`, `gravity` (+155 more)
  These have ≤1 connection - possible missing edges or undocumented components.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `PlayState` connect `Community 3` to `Community 32`, `Community 2`, `Community 34`, `Community 8`, `Community 9`, `Community 10`, `Community 14`, `Community 19`, `Community 22`, `Community 23`, `Community 31`?**
  _High betweenness centrality (0.206) - this node is a cross-community bridge._
- **Why does `GameOverState` connect `Community 2` to `Community 10`, `Community 19`, `Community 14`, `Community 31`?**
  _High betweenness centrality (0.101) - this node is a cross-community bridge._
- **Why does `Pipe` connect `Community 10` to `Community 32`, `Community 1`, `Community 2`, `Community 3`, `Community 5`, `Community 12`, `Community 18`, `Community 19`, `Community 23`?**
  _High betweenness centrality (0.098) - this node is a cross-community bridge._
- **Are the 10 inferred relationships involving `TEST_F()` (e.g. with `flap` and `getBoundingBox`) actually correct?**
  _`TEST_F()` has 10 INFERRED edges - model-reasoned connections that need verification._
- **What connects `sprite`, `birdTexture`, `placeholderTexture` to the rest of the system?**
  _160 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Community 0` be split into smaller, more focused modules?**
  _Cohesion score 0.059154929577464786 - nodes in this community are weakly interconnected._
- **Should `Community 1` be split into smaller, more focused modules?**
  _Cohesion score 0.05384615384615385 - nodes in this community are weakly interconnected._
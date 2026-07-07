# Integration Tests Design

## Overview
Add 8 new integration tests for complex config edge cases and file permission errors.

## Test File: `tests/test_ConfigEdgeCases.cpp`
Single test fixture `ConfigEdgeCaseTest` with 8 tests.

### File Permission Tests
1. **ConfigFileNoPermission** — chmod 000 on temp config, verify load returns false
2. **ConfigFilePathIsDirectory** — pass a directory as config path, verify load fails
3. **HighScoreSaveToReadOnlyDir** — save high score in read-only directory, verify graceful fallback

### Config Edge Cases
4. **ConfigDeeplyNested** — 3-level dot notation (`a.b.c`) from nested JSON
5. **ConfigUnicodeStrings** — emoji and non-ASCII characters in string config values
6. **ConfigExtraUnknownKeys** — config with keys not in schema, verify no crash/warnings
7. **ConfigMultipleConsecutiveLoads** — load config A, then B, verify clean state transition
8. **ConfigEmptyStringValues** — string keys with empty values, verify no crash

## File Changes
- **Create:** `tests/test_ConfigEdgeCases.cpp` (~200 lines)
- **Modify:** `tests/CMakeLists.txt:25` (add one line to TEST_SOURCES)

## Constraints
- No changes to source files (only test files)
- All 189 existing tests must continue to pass
- Temp files cleaned up in TearDown
- Uses only GTest + std::fstream + std::filesystem — no SFML dependency

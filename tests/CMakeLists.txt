# Build tests for Flappy Bird Clone
find_package(GTest REQUIRED)

# Include the source directory for access to headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Test source files (including project .cpp files for linking)
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/ConfigLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/HighScore.cpp
    ${CMAKE_SOURCE_DIR}/src/Bird.cpp
    ${CMAKE_SOURCE_DIR}/src/Pipe.cpp
    test_ConfigLoader.cpp
    test_HighScore.cpp
    test_Bird.cpp
    test_Pipe.cpp
    test_Collision.cpp
    test_Integration.cpp
)

# Executable
add_executable(flappy_tests ${TEST_SOURCES})

# Link GTest and project libraries
target_link_libraries(flappy_tests
    GTest::gtest
    GTest::gtest_main
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copy assets to build directory (same as the main game target)
add_custom_command(TARGET flappy_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_SOURCE_DIR}/build/assets
    COMMENT "Copying assets to build directory for tests"
)

# Discover tests automatically (CTest support)
include(GoogleTest)
gtest_discover_tests(flappy_tests)

# Find SFML
find_package(SFML REQUIRED COMPONENTS Graphics Window System)

# Include directories (all source files are in this directory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
set(SOURCES
    main.cpp
    Bird.cpp
    Pipe.cpp
    ResourceManager.cpp
    HighScore.cpp
    ConfigLoader.cpp
    PlayState.cpp
    GameOverState.cpp
    StateFactory.cpp
)

# Executable
add_executable(FlappyBirdClone ${SOURCES})

# Link libraries
target_link_libraries(FlappyBirdClone sfml-graphics sfml-window sfml-system sfml-audio)

# Set executable output to project root build directory
set_target_properties(FlappyBirdClone PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

# Copy assets to build directory after build
add_custom_command(TARGET FlappyBirdClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_SOURCE_DIR}/build/assets
    COMMENT "Copying assets to build directory"
)

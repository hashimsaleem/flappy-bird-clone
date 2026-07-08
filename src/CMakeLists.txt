find_package(SFML REQUIRED COMPONENTS Graphics Window System)

set(SOURCES
    entities/Bird.cpp
    entities/Pipe.cpp
    entities/PowerUp.cpp
    core/ConfigLoader.cpp
    core/ConfigValues.cpp
    core/HighScore.cpp
    states/PlayState.cpp
    states/GameOverState.cpp
    states/StateFactory.cpp
    systems/ResourceManager.cpp
    systems/VisualEffectManager.cpp
    visual/ParticleSystem.cpp
    visual/CloudSystem.cpp
    visual/EnvironmentRenderer.cpp
    scoring/ScoreManager.cpp
    audio/SoundManager.cpp
    main.cpp
)

add_library(FlappyBirdLib STATIC ${SOURCES})
target_link_libraries(FlappyBirdLib PUBLIC sfml-graphics sfml-window sfml-system sfml-audio)
target_include_directories(FlappyBirdLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(FlappyBirdClone main.cpp)
target_link_libraries(FlappyBirdClone PRIVATE FlappyBirdLib)

set_target_properties(FlappyBirdClone PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

add_custom_command(TARGET FlappyBirdClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:FlappyBirdClone>/assets
    COMMENT "Copying assets to build directory"
)

find_package(SFML REQUIRED COMPONENTS Graphics Window System)

set(SOURCES
    Bird.cpp
    Pipe.cpp
    ResourceManager.cpp
    HighScore.cpp
    ConfigLoader.cpp
    PlayState.cpp
    GameOverState.cpp
    StateFactory.cpp
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
        ${CMAKE_SOURCE_DIR}/build/assets
    COMMENT "Copying assets to build directory"
)

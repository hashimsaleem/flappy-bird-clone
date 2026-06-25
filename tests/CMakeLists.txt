find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Bird.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Collision.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ConfigLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_HighScore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Integration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ObjectPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_Pipe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ScoreFloat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_StateFactory.cpp
)

add_executable(flappy_tests ${TEST_SOURCES})
target_link_libraries(flappy_tests PRIVATE FlappyBirdLib GTest::gtest GTest::gtest_main)

add_custom_command(TARGET flappy_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}"
    COMMENT "Copying assets")

include(CTest)
gtest_discover_tests(flappy_tests)

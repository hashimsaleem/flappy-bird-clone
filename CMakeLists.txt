cmake_minimum_required(VERSION 3.10)
project(FlappyBirdClone VERSION 1.0 LANGUAGES CXX)

# Add source subdirectory
add_subdirectory(src)

# Unit tests (off by default; enable with -DBUILD_TESTS=ON)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

cmake_minimum_required(VERSION 3.10)
project(FlappyBirdClone VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML REQUIRED COMPONENTS Graphics Window System)

# Include directories
# (Optional for modern CMake but good for now)
include_directories(${SFML_INCLUDE_DIRS})

# Source files
set(SOURCES 
    main.cpp 
    Bird.cpp 
    Pipe.cpp
)

# Executable
add_executable(FlappyBirdClone ${SOURCES})

# Link libraries
target_link_libraries(FlappyBirdClone sfml-graphics sfml-window sfml-system)

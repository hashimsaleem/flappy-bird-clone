#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Bird.hpp"
#include "Pipe.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    // 1. Initialize the window/view
    sf::RenderWindow window(sf::VideoMode({(unsigned int)SCREEN_WIDTH, (unsigned int)SCREEN_HEIGHT}), "Flappy Clone SFML");
    window.setFramerateLimit(60);

    std::cout << "Starting Flappy Bird Game Loop..." << std::endl;

    // 2. Initialize game objects
    Bird bird;
    bird.load("bird.png"); // Assuming this exists or will be handled
    std::vector<Pipe> pipes;

    // --- Main Game Loop ---
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    bird.flap();
                }
            }
        }

        // 3. Update Logic
        float dt = 1.0f / 60.0f;
        // Update the bird's position based on gravity and input
        bird.update(dt);

        // Move pipes and spawn new ones
        for (auto& pipe : pipes) {
            pipe.update(dt);
        }

        // Remove pipes that are off-screen
        pipes.erase(std::remove_if(pipes.begin(), pipes.end(), 
            [](const Pipe& p) { return p.isOffScreen(); }), pipes.end());

        // Spawn new pipes logic (simplified)
        static float spawnTimer = 0.0f;
        spawnTimer += dt;
        if (spawnTimer > 1.5f) {
            pipes.push_back(Pipe(SCREEN_WIDTH, 300.0f, 150.0f));
            spawnTimer = 0.0f;
        }

        // Check collisions (logic needed here)
        // for (const auto& pipe : pipes) {
        //     // Collision logic
        // }

        // 4. Rendering
        window.clear(sf::Color(135, 206, 235)); // Sky blue background color

        // Draw the bird
        bird.draw(window);

        // Draw all pipes
        for (const auto& pipe : pipes) {
            pipe.draw(window);
        }

        window.display();
    }

    return 0;
}

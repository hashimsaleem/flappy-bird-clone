#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

// Forward declarations for classes we will create
class Bird;
class Pipe;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    // 1. Initialize the window/view
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Flappy Clone SFML");
    window.setFramerateLimit(60);

    std::cout << "Starting Flappy Bird Game Loop..." << std::endl;

    // 2. Initialize game objects (placeholders for now)
    Bird bird; // We will define this class later
    std::vector<Pipe> pipes; // Container for all active pipes

    // --- Main Game Loop ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                // Handle jumping later
                std::cout << "Jump input received!" << std::endl;
            }
        }

        // 3. Update Logic
        // Update the bird's position based on gravity and input
        bird.update(1.0f / 60.0f); // delta time (assuming 60 FPS)

        // Move pipes and spawn new ones (logic needed here)
        for (auto& pipe : pipes) {
            pipe.move(); // Logic to move the pipe
        }
        // Add logic to remove old/offscreen pipes

        // Check collisions (logic needed here)
        // if (check_collision(bird, pipes)) { break; }


        // 4. Rendering
        window.clear(sf::Color(135, 206, 235)); // Sky blue background color

        // Draw the bird
        // window.draw(bird); // Assuming 'bird' implements sf::Drawable

        // Draw all pipes
        // for (const auto& pipe : pipes) {
        //     window.draw(pipe);
        // }

        window.display();

        // Sleep/Wait until next frame (SFML handles this usually, but good practice conceptualizing it)
    }

    return 0;
}
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Game - SFML Test");
    window.setFramerateLimit(60);

    // Create a simple circle (test shape)
    sf::CircleShape pacman(50.f);
    pacman.setFillColor(sf::Color::Yellow);
    pacman.setPosition(375.f, 275.f);

    // Velocity for movement
    sf::Vector2f velocity(0.f, 0.f);
    float speed = 200.f; // pixels per second

    // Clock for deltaTime
    sf::Clock clock;

    std::cout << "SFML working! Use arrow keys to move the circle.\n";
    std::cout << "Press ESC to close.\n";

    // Main game loop
    while (window.isOpen()) {
        // Delta time
        float dt = clock.restart().asSeconds();

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        // Input handling (continuous)
        velocity = sf::Vector2f(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            velocity.x = -speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            velocity.x = speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            velocity.y = -speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            velocity.y = speed;
        }

        // Update position
        pacman.move(velocity * dt);

        // Keep within bounds
        sf::Vector2f pos = pacman.getPosition();
        if (pos.x < 0) pacman.setPosition(0, pos.y);
        if (pos.x > 700) pacman.setPosition(700, pos.y);
        if (pos.y < 0) pacman.setPosition(pos.x, 0);
        if (pos.y > 500) pacman.setPosition(pos.x, 500);

        // Render
        window.clear(sf::Color::Black);
        window.draw(pacman);
        window.display();
    }

    std::cout << "Window closed. Goodbye!\n";
    return 0;
}

// test
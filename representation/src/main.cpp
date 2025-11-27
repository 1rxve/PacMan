#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"


int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Game - SFML Test");
    window.setFramerateLimit(60);

    // Maak Camera instantie
    representation::Camera camera(800.0f, 600.0f);

    // Maak PacManModel (in NORMALIZED coordinates)
    logic::PacManModel pacman(0.0f, 0.0f, 0.1f, 0.1f, 0.5f);

    // Create a simple circle (test shape)
    sf::CircleShape circle(25.f);
        circle.setFillColor(sf::Color::Yellow);

    // Clock for deltaTime
    sf::Clock clock;


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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Input handling - zet nextDirection op PacManModel
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            pacman.setNextDirection(logic::Direction::LEFT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            pacman.setNextDirection(logic::Direction::RIGHT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            pacman.setNextDirection(logic::Direction::UP);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            pacman.setNextDirection(logic::Direction::DOWN);
        }

        // Update PacManModel (dit beweegt hem in normalized coords)
        pacman.update(dt);

        // Convert normalized → pixel voor visualisatie
        float pixelX = camera.normalizedToPixelX(pacman.getX());
        float pixelY = camera.normalizedToPixelY(pacman.getY());

        // Positioneer circle op basis van PacMan's positie
        circle.setPosition(pixelX - 25.f, pixelY - 25.f);  // -radius voor centrum

        // Render
        window.clear(sf::Color::Black);
        window.draw(circle);
        window.display();
    }

    return 0;
}

//
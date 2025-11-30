#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/world/World.h"
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"
#include "representation/ConcreteFactory.h"

int main() {
    // Setup window
    sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Game");
    window.setFramerateLimit(60);

    // Setup camera
    representation::Camera camera(800.0f, 600.0f);

    // Setup factory
    representation::ConcreteFactory factory(&window, &camera);

    // Setup world
    logic::World world;
    world.setFactory(&factory);
    world.loadMap("resources/maps/test_map.txt");

    // Get PacMan reference for input
    logic::PacManModel* pacman = world.getPacMan();
    if (!pacman) {
        std::cerr << "ERROR: PacMan not found in map!" << std::endl;
        return -1;
    }

    // Game loop
    sf::Clock clock;

    while (window.isOpen()) {
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

        // Input handling
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            pacman->setNextDirection(logic::Direction::LEFT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            pacman->setNextDirection(logic::Direction::RIGHT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            pacman->setNextDirection(logic::Direction::UP);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            pacman->setNextDirection(logic::Direction::DOWN);
        }

        // Update world (triggers notify() -> Views draw themselves)
        window.clear(sf::Color::Black);
        world.update(dt);
        window.display();
    }

    return 0;
}
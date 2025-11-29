#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"
#include "logic/world/World.h"
#include "representation/views/PacManView.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Game - SFML Test");
    window.setFramerateLimit(60);

    representation::Camera camera(800.0f, 600.0f);

    logic::World world;

    world.loadMap("resources/maps/test_map.txt");

    auto pacmanPtr = new logic::PacManModel(0.0f, 0.0f, 0.1f, 0.1f, 0.5f);
    logic::PacManModel* pacman = pacmanPtr;

    world.addEntity(std::unique_ptr<logic::EntityModel>(pacmanPtr));

    // Maak PacManView
    representation::PacManView pacmanView(pacman);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

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

        world.update(dt);
        pacmanView.update(dt);  // Update animatie

        window.clear(sf::Color::Black);
        pacmanView.draw(window, camera);  // Teken PacMan
        window.display();
    }

    return 0;
}
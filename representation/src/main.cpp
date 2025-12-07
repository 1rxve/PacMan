#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/world/World.h"
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"
#include "representation/ConcreteFactory.h"

int main() {
    const std::string MAP_FILE = "resources/maps/test_map.txt";

    // STAP 1: Lees map dimensies
    auto [mapWidth, mapHeight] = logic::World::getMapDimensions(MAP_FILE);

    if (mapWidth == 0 || mapHeight == 0) {
        std::cerr << "ERROR: Invalid map dimensions!" << std::endl;
        return -1;
    }

    // STAP 2: Bereken window size (vierkante cells)
    const int CELL_SIZE_PIXELS = 50;  // Pas aan naar wens (20, 25, 30, 40, etc.)
    int windowWidth = mapWidth * CELL_SIZE_PIXELS;
    int windowHeight = mapHeight * CELL_SIZE_PIXELS;

    std::cout << "=== Map & Window Info ===" << std::endl;
    std::cout << "Map dimensions: " << mapWidth << " x " << mapHeight << " cells" << std::endl;
    std::cout << "Cell size: " << CELL_SIZE_PIXELS << " pixels" << std::endl;
    std::cout << "Window size: " << windowWidth << " x " << windowHeight << " pixels" << std::endl;
    std::cout << "=========================" << std::endl;

    // STAP 3: Setup window met dynamische grootte
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "PacMan Game");
    window.setFramerateLimit(60);

    // STAP 4: Setup camera met correcte dimensies
    representation::Camera camera(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    // STAP 5: Setup factory en world
    representation::ConcreteFactory factory(&window, &camera);
    logic::World world;
    world.setFactory(&factory);
    world.loadMap(MAP_FILE);

    // STAP 6: Get PacMan reference for input
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
#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/world/World.h"
#include "logic/entities/PacManModel.h"
#include "logic/utils/Stopwatch.h"      // ← TOEVOEGEN
#include "representation/Camera.h"
#include "representation/ConcreteFactory.h"

int main() {
    const std::string MAP_FILE = "resources/maps/map";

    auto [mapWidth, mapHeight] = logic::World::getMapDimensions(MAP_FILE);

    if (mapWidth == 0 || mapHeight == 0) {
        std::cerr << "ERROR: Invalid map dimensions!" << std::endl;
        return -1;
    }

    const int CELL_SIZE_PIXELS = 50;
    int windowWidth = mapWidth * CELL_SIZE_PIXELS;
    int windowHeight = mapHeight * CELL_SIZE_PIXELS;

    std::cout << "=== Map & Window Info ===" << std::endl;
    std::cout << "Map dimensions: " << mapWidth << " x " << mapHeight << " cells" << std::endl;
    std::cout << "Cell size: " << CELL_SIZE_PIXELS << " pixels" << std::endl;
    std::cout << "Window size: " << windowWidth << " x " << windowHeight << " pixels" << std::endl;
    std::cout << "=========================" << std::endl;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "PacMan Game");
    window.setFramerateLimit(60);

    representation::Camera camera(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    representation::ConcreteFactory factory(&window, &camera);
    logic::World world;
    world.setFactory(&factory);
    world.loadMap(MAP_FILE);

    logic::PacManModel* pacman = world.getPacMan();
    if (!pacman) {
        std::cerr << "ERROR: PacMan not found in map!" << std::endl;
        return -1;
    }

    // ← NIEUW: Stopwatch singleton ipv sf::Clock
    logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
    stopwatch.restart();

    while (window.isOpen()) {
        // ← NIEUW: Update stopwatch en get deltaTime
        stopwatch.update();
        float dt = stopwatch.getDeltaTime();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Input handling - jouw versie is perfect, blijft EXACT zo
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if (pacman->getCurrentDirection() != logic::Direction::UP) {
                pacman->setNextDirection(logic::Direction::UP);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if (pacman->getCurrentDirection() != logic::Direction::DOWN) {
                pacman->setNextDirection(logic::Direction::DOWN);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (pacman->getCurrentDirection() != logic::Direction::LEFT) {
                pacman->setNextDirection(logic::Direction::LEFT);
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (pacman->getCurrentDirection() != logic::Direction::RIGHT) {
                pacman->setNextDirection(logic::Direction::RIGHT);
            }
        }

        window.clear(sf::Color::Black);
        world.update(dt);
        window.display();
    }

    return 0;
}
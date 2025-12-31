#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/world/World.h"
#include "representation/Game.h"

int main() {
    const std::string MAP_FILE = "resources/maps/map";

    // Get map1 dimensions voor window size
    auto [mapWidth, mapHeight] = logic::World::getMapDimensions(MAP_FILE);

    if (mapWidth == 0 || mapHeight == 0) {
        std::cerr << "ERROR: Invalid map dimensions!" << std::endl;
        return -1;
    }

    // Calculate window size with LEFT + RIGHT sidebars
    const int CELL_SIZE_PIXELS = 50;
    const int SIDEBAR_WIDTH = 250;
    int gameAreaWidth = mapWidth * CELL_SIZE_PIXELS;
    int windowHeight = mapHeight * CELL_SIZE_PIXELS;
    int windowWidth = gameAreaWidth + (2 * SIDEBAR_WIDTH);  // ← Both sides

    std::cout << "=== Map & Window Info ===" << std::endl;
    std::cout << "Map dimensions: " << mapWidth << " x " << mapHeight << " cells" << std::endl;
    std::cout << "Window size: " << windowWidth << " x " << windowHeight << " pixels" << std::endl;
    std::cout << "=========================" << std::endl;

    // Create window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "PacMan Game");
    window.setFramerateLimit(60);

    // Create and run game
    representation::Game game(&window, MAP_FILE);
    game.run();

    return 0;
}
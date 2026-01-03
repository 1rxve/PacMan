#include "logic/world/World.h"
#include "representation/Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    const std::string MAP_FILE = "resources/maps/map";

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float windowScale = 0.8f;

    // Target aspect ratio based on original design (1450x1050)
    const float ASPECT_RATIO = 1450.0f / 1050.0f;
    unsigned int windowHeight = static_cast<unsigned int>(desktop.height * windowScale);
    unsigned int windowWidth = static_cast<unsigned int>(windowHeight * ASPECT_RATIO);

    if (windowWidth > desktop.width * windowScale) {
        windowWidth = static_cast<unsigned int>(desktop.width * windowScale);
        windowHeight = static_cast<unsigned int>(windowWidth / ASPECT_RATIO);
    }

    // Non-resizable to prevent Camera coordinate conversion issues
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "PacMan Game",
                            sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60); // Cap at 60 FPS (matches typical display refresh rate)

    representation::Game game(&window, MAP_FILE);
    game.run();

    return 0;
}
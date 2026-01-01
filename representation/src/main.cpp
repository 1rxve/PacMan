#include <SFML/Graphics.hpp>
#include <iostream>
#include "logic/world/World.h"
#include "representation/Game.h"

int main() {
    const std::string MAP_FILE = "resources/maps/map";

    // Get desktop size for dynamic initial sizing
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float windowScale = 0.8f;  // 80% of screen

    // Calculate window size (maintains 1450x1050 aspect ratio)
    const float ASPECT_RATIO = 1450.0f / 1050.0f;
    unsigned int windowHeight = static_cast<unsigned int>(desktop.height * windowScale);
    unsigned int windowWidth = static_cast<unsigned int>(windowHeight * ASPECT_RATIO);

    // Cap at desktop width if too wide
    if (windowWidth > desktop.width * windowScale) {
        windowWidth = static_cast<unsigned int>(desktop.width * windowScale);
        windowHeight = static_cast<unsigned int>(windowWidth / ASPECT_RATIO);
    }

    // Create NON-RESIZABLE window
    sf::RenderWindow window(
            sf::VideoMode(windowWidth, windowHeight),
            "PacMan Game",
            sf::Style::Close | sf::Style::Titlebar  // No Resize flag
    );
    window.setFramerateLimit(60);

    // Create and run game
    representation::Game game(&window, MAP_FILE);
    game.run();

    return 0;
}
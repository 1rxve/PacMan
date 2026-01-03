#include "representation/Game.h"
#include "logic/utils/Stopwatch.h"
#include "representation/states/MenuState.h"
#include "representation/states/State.h"
#include <iostream>

namespace representation {
Game::Game(sf::RenderWindow* win, const std::string& mapFile) : window(win), mapFile(mapFile) {

    const float SIDEBAR_WIDTH = 250.0f; // UI overlay width (score, lives, level)
    camera = std::make_unique<Camera>(static_cast<float>(window->getSize().x),
                                      static_cast<float>(window->getSize().y),
                                      SIDEBAR_WIDTH);

    factory = std::make_unique<ConcreteFactory>(window, camera.get());
    stateManager = std::make_unique<StateManager>();

    // Initialize with MenuState (entry point)
    stateManager->pushState(
        std::make_unique<MenuState>(window, factory.get(), camera.get(), stateManager.get(), mapFile));
}

Game::~Game() {}

void Game::run() {
    try {
        logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
        stopwatch.restart();

        // Main game loop: event polling → update → render → display
        while (window->isOpen()) {
            stopwatch.update();
            float dt = stopwatch.getDeltaTime();

            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                }

                stateManager->handleEvent(event);
            }

            window->clear(sf::Color::Black);
            stateManager->update(dt);
            stateManager->render();
            window->display();
        }
    } catch (const std::exception& e) {
        // Graceful shutdown on runtime errors (texture loading, map parsing, etc.)
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        window->close();
    }
}
} // namespace representation
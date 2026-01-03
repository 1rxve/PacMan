#include "representation/Game.h"
#include "logic/utils/Stopwatch.h"
#include "representation/states/MenuState.h"
#include "representation/states/State.h"
#include <iostream>

namespace representation {
Game::Game(sf::RenderWindow* win, const std::string& mapFile) : window(win), mapFile(mapFile) {

    // Create Camera with sidebar
    const float SIDEBAR_WIDTH = 250.0f;
    camera = std::make_unique<Camera>(static_cast<float>(window->getSize().x),
                                      static_cast<float>(window->getSize().y),
                                      SIDEBAR_WIDTH);

    // Create Factory
    factory = std::make_unique<ConcreteFactory>(window, camera.get());

    // Create StateManager
    stateManager = std::make_unique<StateManager>();

    // Start with MenuState
    stateManager->pushState(
        std::make_unique<MenuState>(window, factory.get(), camera.get(), stateManager.get(), mapFile));
}

Game::~Game() {
    // Destructor body
}

void Game::run() {
    try {
        logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
        stopwatch.restart();

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
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        window->close();
    }
}
} // namespace representation
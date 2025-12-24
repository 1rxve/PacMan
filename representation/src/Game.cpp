#include "representation/Game.h"
#include "representation/states/MenuState.h"  // ← CHANGE: MenuState ipv LevelState
#include "representation/states/State.h"
#include "logic/utils/Stopwatch.h"
#include <iostream>

namespace representation {
    Game::Game(sf::RenderWindow* win, const std::string& mapFile)
            : window(win), mapFile(mapFile) {


        // Create Camera
        camera = std::make_unique<Camera>(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        );

        // Create Factory
        factory = std::make_unique<ConcreteFactory>(window, camera.get());

        // Create StateManager
        stateManager = std::make_unique<StateManager>();

        // ← CHANGE: Start with MenuState
        stateManager->pushState(std::make_unique<MenuState>(
                window, factory.get(), camera.get(), stateManager.get(), mapFile
        ));
    }

    Game::~Game() {
        // Destructor body
    }

    void Game::run() {
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
                // ← REMOVE ESC handling - states handle dit nu

                stateManager->handleEvent(event);
            }

            window->clear(sf::Color::Black);
            stateManager->update(dt);
            stateManager->render();
            window->display();
        }

    }
}
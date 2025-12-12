#include "representation/Game.h"
#include "representation/states/LevelState.h"
#include "representation/states/State.h"  // ← ADD: Include State.h voor complete type
#include "logic/utils/Stopwatch.h"
#include <iostream>

namespace representation {
    Game::Game(sf::RenderWindow* win, const std::string& mapFile)
            : window(win), mapFile(mapFile) {

        std::cout << "Game: Initializing..." << std::endl;

        // Create Camera
        camera = std::make_unique<Camera>(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        );
        std::cout << "Game: Camera created" << std::endl;

        // Create Factory
        factory = std::make_unique<ConcreteFactory>(window, camera.get());
        std::cout << "Game: ConcreteFactory created" << std::endl;

        // Create StateManager
        stateManager = std::make_unique<StateManager>();

        // Start with LevelState (later MenuState)
        stateManager->pushState(std::make_unique<LevelState>(
                window, factory.get(), camera.get(), stateManager.get(), mapFile
        ));
        std::cout << "Game: LevelState pushed to StateManager" << std::endl;
    }

    // ← ADD EXPLICIT DESTRUCTOR DEFINITION
    Game::~Game() {
        // Destructor moet gedefinieerd worden in .cpp waar State.h included is
        // unique_ptr kan nu correct State destructor aanroepen
    }

    void Game::run() {
        logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
        stopwatch.restart();

        std::cout << "Game: Starting main loop" << std::endl;

        while (window->isOpen()) {
            stopwatch.update();
            float dt = stopwatch.getDeltaTime();

            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                }
                if (event.type == sf::Event::KeyPressed &&
                    event.key.code == sf::Keyboard::Escape) {
                    window->close();
                }

                stateManager->handleEvent(event);
            }

            window->clear(sf::Color::Black);
            stateManager->update(dt);
            stateManager->render();
            window->display();
        }

        std::cout << "Game: Main loop ended" << std::endl;
    }
}
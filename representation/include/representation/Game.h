#ifndef PACMANGAME_GAME_H
#define PACMANGAME_GAME_H

#include "Camera.h"
#include "ConcreteFactory.h"
#include "StateManager.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

namespace representation {
/**
 * Main game controller responsible for SFML setup and game loop execution.
 *
 * Responsibilities:
 * - Owns Camera (coordinate conversion)
 * - Owns ConcreteFactory (entity creation with SFML coupling)
 * - Owns StateManager (state machine orchestration)
 * - Initializes with MenuState
 * - Runs main game loop (event polling, update, render)
 *
 * Does NOT own window - passed in from main() for flexibility.
 * Uses Stopwatch singleton for frame-independent delta time.
 */
class Game {
private:
    sf::RenderWindow* window;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<ConcreteFactory> factory;
    std::unique_ptr<StateManager> stateManager;
    std::string mapFile;

public:
    Game(sf::RenderWindow* window, const std::string& mapFile);
    ~Game();

    /**
     * Executes main game loop until window closed:
     * 1. Update Stopwatch (deltaTime calculation)
     * 2. Poll SFML events (window close, keyboard input)
     * 3. Delegate event handling to active State
     * 4. Update active State (game logic)
     * 5. Render active State (SFML drawing)
     * 6. Display to window
     *
     * Exception handling catches runtime errors and closes window gracefully.
     */
    void run();
};
} // namespace representation

#endif // PACMANGAME_GAME_H
#ifndef PACMANGAME_STATE_H
#define PACMANGAME_STATE_H

#include "logic/patterns/AbstractFactory.h"
#include <SFML/Graphics.hpp>

namespace representation {
class Camera;
class StateManager;

/**
 * Abstract base class for State Pattern implementation (finite state machine).
 *
 * Concrete states (MenuState, LevelState, PausedState, VictoryState, NameEntryState)
 * manage different phases of gameplay with isolated update/render/input logic.
 *
 * State Stack managed by StateManager:
 * - States can push new states (e.g., LevelState → PausedState)
 * - States can pop themselves (e.g., PausedState resume → pop to LevelState)
 * - Only top state receives update/render/handleEvent calls
 *
 * Protected members allow subclasses direct access to core resources
 * (window for rendering, factory for entity creation, camera for coordinate
 * conversion, stateManager for state transitions).
 */
class State {
protected:
    sf::RenderWindow* window;
    logic::AbstractFactory* factory;
    const Camera* camera;
    StateManager* stateManager;

public:
    State(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm)
        : window(win), factory(fac), camera(cam), stateManager(sm) {}

    virtual ~State() = default;

    virtual void update(float deltaTime) = 0;

    virtual void render() = 0;

    virtual void handleEvent(const sf::Event& event) = 0;
};
} // namespace representation

#endif // PACMANGAME_STATE_H
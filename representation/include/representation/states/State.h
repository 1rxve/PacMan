#ifndef PACMANGAME_STATE_H
#define PACMANGAME_STATE_H

#include "logic/patterns/AbstractFactory.h" // ← CORRECT (same as EntityView doet)
#include <SFML/Graphics.hpp>

namespace representation {
class Camera;       // ← Forward declare (eigen namespace)
class StateManager; // ← Forward declare (eigen namespace)

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

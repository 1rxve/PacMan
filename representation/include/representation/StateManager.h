#ifndef PACMANGAME_STATEMANAGER_H
#define PACMANGAME_STATEMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace representation {
class State;

/**
 * Manages State Pattern finite state machine using stack-based transitions.
 *
 * State Stack mechanic:
 * - Only top state receives update/render/handleEvent calls
 * - States can push new states (e.g., LevelState → PausedState)
 * - States can pop themselves (e.g., PausedState resume → back to LevelState)
 * - Enables pause-and-resume without destroying underlying state
 *
 * Typical stack progression:
 * [MenuState] → push LevelState → [MenuState, LevelState] → push PausedState
 * → [MenuState, LevelState, PausedState] → pop → [MenuState, LevelState] (resume)
 *
 * CRITICAL: States must copy needed data to locals BEFORE calling popState()
 * because popState() destroys the state object immediately.
 */
class StateManager {
private:
    std::vector<std::unique_ptr<State>> stateStack;

public:
    /**
     * Pushes new state onto stack, making it active.
     * Ownership transferred via unique_ptr (StateManager now owns state).
     */
    void pushState(std::unique_ptr<State> state);

    /**
     * Pops top state from stack, destroying it immediately.
     * Previous state (if any) becomes active.
     *
     * CRITICAL: Caller must copy needed data to locals BEFORE calling popState()
     * because 'this' is destroyed during pop if caller is the top state.
     */
    void popState();

    void update(float deltaTime);

    void render();

    void handleEvent(const sf::Event& event);

    bool isEmpty() const;
};
} // namespace representation

#endif // PACMANGAME_STATEMANAGER_H
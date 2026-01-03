#ifndef PACMANGAME_PAUSEDSTATE_H
#define PACMANGAME_PAUSEDSTATE_H

#include "State.h"

namespace representation {
/**
 * Overlay state for pausing gameplay without destroying LevelState.
 *
 * State Stack mechanic:
 * - Renders frozen LevelState underneath (via levelStateBelow raw pointer)
 * - Semi-transparent dark overlay for dimming effect
 * - Pause menu text on top
 *
 * State transitions:
 * - P → pop (resume LevelState)
 * - R → pop twice + push new LevelState (restart level)
 * - ESC → pop twice (return to MenuState)
 *
 * CRITICAL: R key copies all needed data to locals BEFORE popState()
 * to prevent use-after-free (this object destroyed during pop).
 */
class PausedState : public State {
private:
    sf::Font font;
    sf::Text pausedText;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text quitText;
    bool fontLoaded;

    State* levelStateBelow; // Not owned - raw pointer to render frozen game underneath
    std::string mapFile;

public:
    PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                State* levelState, const std::string& mapFile);

    void update(float deltaTime) override;

    void render() override;

    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_PAUSEDSTATE_H
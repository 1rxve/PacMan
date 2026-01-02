#ifndef PACMANGAME_PAUSEDSTATE_H
#define PACMANGAME_PAUSEDSTATE_H

#include "State.h"

namespace representation {
class PausedState : public State {
private:
    sf::Font font;
    sf::Text pausedText;
    sf::Text resumeText;
    sf::Text restartText;
    sf::Text quitText;
    bool fontLoaded;

    State* levelStateBelow;
    std::string mapFile;

public:
public:
    PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                State* levelState, const std::string& mapFile);

    void update(float deltaTime) override;
    void render() override;
    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_PAUSEDSTATE_H

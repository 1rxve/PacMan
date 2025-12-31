#ifndef PACMANGAME_PAUSEDSTATE_H
#define PACMANGAME_PAUSEDSTATE_H


#include "State.h"

namespace representation {
    class PausedState : public State {
    private:
        sf::Font font;
        sf::Text pausedText;
        sf::Text instructionText;
        bool fontLoaded;

        State* levelStateBelow;

    public:
        PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                    const Camera* cam, StateManager* sm, State* levelState);

        void update(float deltaTime) override;
        void render() override;
        void handleEvent(const sf::Event& event) override;
    };
}



#endif //PACMANGAME_PAUSEDSTATE_H

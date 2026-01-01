#ifndef PACMANGAME_STATEMANAGER_H
#define PACMANGAME_STATEMANAGER_H


#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

namespace representation {
    class State;

    class StateManager {
    private:
        std::vector<std::unique_ptr<State>> stateStack;

    public:
        void pushState(std::unique_ptr<State> state);
        void popState();
        void update(float deltaTime);
        void render();
        void handleEvent(const sf::Event& event);
        bool isEmpty() const;
    };
}


#endif //PACMANGAME_STATEMANAGER_H

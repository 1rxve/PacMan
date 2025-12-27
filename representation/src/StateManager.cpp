#include "representation/StateManager.h"
#include "representation/states/State.h"

namespace representation {
    void StateManager::pushState(std::unique_ptr<State> state) {
        stateStack.push_back(std::move(state));
    }

    void StateManager::popState() {
        if (!stateStack.empty()) {
            stateStack.pop_back();
        }
    }

    void StateManager::update(float deltaTime) {
        if (!stateStack.empty()) {
            stateStack.back()->update(deltaTime);
        }
    }

    void StateManager::render() {
        if (!stateStack.empty()) {
            stateStack.back()->render();
        }
    }

    void StateManager::handleEvent(const sf::Event& event) {
        if (!stateStack.empty()) {
            stateStack.back()->handleEvent(event);
        }
    }

    bool StateManager::isEmpty() const {
        return stateStack.empty();
    }
}

#ifndef PACMANGAME_LEVELSTATE_H
#define PACMANGAME_LEVELSTATE_H


#include "State.h"
#include "logic/world/World.h"
#include <memory>
#include <string>

namespace representation {
    class LevelState : public State {
    private:
        std::unique_ptr<logic::World> world;
        std::string mapFile;

    public:
        LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                   const Camera* cam, StateManager* sm,
                   const std::string& mapFile);

        ~LevelState();

        void update(float deltaTime) override;
        void render() override;
        void handleEvent(const sf::Event& event) override;
    };
}


#endif //PACMANGAME_LEVELSTATE_H

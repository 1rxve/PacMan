#ifndef PACMANGAME_GAME_H
#define PACMANGAME_GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "StateManager.h"
#include "Camera.h"
#include "ConcreteFactory.h"

namespace representation {
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
        void run();
    };
}


#endif //PACMANGAME_GAME_H

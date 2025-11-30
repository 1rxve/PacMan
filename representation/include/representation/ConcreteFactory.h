#ifndef PACMANGAME_CONCRETEFACTORY_H
#define PACMANGAME_CONCRETEFACTORY_H

#include <SFML/Graphics.hpp>
#include "logic/patterns/AbstractFactory.h"
#include "representation/Camera.h"

namespace representation {
    class ConcreteFactory : public logic::AbstractFactory {
    private:
        sf::RenderWindow* window;
        const Camera* camera;

    public:
        ConcreteFactory(sf::RenderWindow* window, const Camera* camera);

        logic::EntityCreationResult createWall(float x, float y, float w, float h) override;
        logic::EntityCreationResult createPacMan(float x, float y, float w, float h, float speed) override;
    };
}


#endif //PACMANGAME_CONCRETEFACTORY_H

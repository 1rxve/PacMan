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
        std::shared_ptr<sf::Texture> sharedTexture;
        std::shared_ptr<sf::Texture> doorTexture;

    public:
        ConcreteFactory(sf::RenderWindow* window, const Camera* camera);

        logic::EntityCreationResult createWall(float x, float y, float w, float h) override;
        logic::EntityCreationResult createPacMan(float x, float y, float w, float h, float speed) override;
        logic::EntityCreationResult createCoin(float x, float y, float w, float h) override;
        logic::EntityCreationResult createGhost(float x, float y, float w, float h, logic::GhostType type, float spawnDelay) override;
        logic::EntityCreationResult createDoor(float x, float y, float w, float h) override;
        logic::EntityCreationResult createNoEntry(float x, float y, float w, float h) override;
        logic::EntityCreationResult createFruit(float x, float y, float w, float h) override;
    };
}


#endif //PACMANGAME_CONCRETEFACTORY_H

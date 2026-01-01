#ifndef PACMANGAME_ABSTRACTFACTORY_H
#define PACMANGAME_ABSTRACTFACTORY_H


#include <memory>
#include "logic/entities/EntityModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/patterns/Observer.h"

namespace logic {
    struct EntityCreationResult {
        std::unique_ptr<EntityModel> model;
        std::unique_ptr<Observer> view;
    };

    class AbstractFactory {
    public:
        virtual ~AbstractFactory() = default;

        virtual EntityCreationResult createWall(float x, float y, float w, float h) = 0;
        virtual EntityCreationResult createPacMan(float x, float y, float w, float h, float speed) = 0;
        virtual EntityCreationResult createCoin(float x, float y, float w, float h) = 0;
        virtual EntityCreationResult createGhost(float x, float y, float w, float h, GhostType type, float spawnDelay) = 0;
        virtual EntityCreationResult createDoor(float x, float y, float w, float h) = 0;
        virtual EntityCreationResult createNoEntry(float x, float y, float w, float h) = 0;
        virtual EntityCreationResult createFruit(float x, float y, float w, float h) = 0;
    };
}


#endif //PACMANGAME_ABSTRACTFACTORY_H

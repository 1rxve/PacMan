#ifndef PACMANGAME_WORLD_H
#define PACMANGAME_WORLD_H

#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <iostream>
#include "logic/entities/EntityModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/patterns/AbstractFactory.h"
#include "logic/patterns/Observer.h"

namespace logic {
    class World {
    private:
        std::vector<std::unique_ptr<EntityModel> > entities;
        std::vector<std::unique_ptr<Observer>> views;
        AbstractFactory* factory;

    public:
        World() = default;

        void update(float deltaTime);

        void setFactory(AbstractFactory* factory);

        void addEntity(std::unique_ptr<EntityModel> entity);

        void checkCollisions();

        void loadMap(const std::string& filename);

        PacManModel* getPacMan();
    };
}


#endif //PACMANGAME_WORLD_H

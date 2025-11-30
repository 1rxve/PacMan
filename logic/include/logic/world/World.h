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
    class PacManModel;
    class WallModel;

    class World {
    private:
        std::vector<std::unique_ptr<EntityModel> > entities;
        std::vector<std::unique_ptr<Observer>> views;
        AbstractFactory* factory;
        PacManModel* pacman;
        std::vector<WallModel*> walls;

    public:
        World() : factory(nullptr), pacman(nullptr) {}

        void update(float deltaTime);

        void setFactory(AbstractFactory* factory);

        void addEntity(std::unique_ptr<EntityModel> entity);

        void checkCollisions();

        void loadMap(const std::string& filename);

        PacManModel* getPacMan();
    };
}


#endif //PACMANGAME_WORLD_H

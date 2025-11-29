#ifndef PACMANGAME_WORLD_H
#define PACMANGAME_WORLD_H

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "logic/entities/EntityModel.h"

namespace logic {
    class World {
    private:
        std::vector<std::unique_ptr<EntityModel> > entities;

    public:
        World() = default;

        void update(float deltaTime);

        void addEntity(std::unique_ptr<EntityModel> entity);

        void checkCollisions();

        void loadMap(const std::string& filename);
    };
}


#endif //PACMANGAME_WORLD_H

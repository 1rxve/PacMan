#ifndef PACMANGAME_WORLD_H
#define PACMANGAME_WORLD_H

#include <vector>
#include <memory>
#include "logic/entities/EntityModel.h"

namespace logic {

class World {
private:
    std::vector<std::unique_ptr<EntityModel>> entities;

public:
    World() = default;

    void addEntity(std::unique_ptr<EntityModel> entity);
    void update(float deltaTime);
    void checkCollisions();
};

}



#endif //PACMANGAME_WORLD_H

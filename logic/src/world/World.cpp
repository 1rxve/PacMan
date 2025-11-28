#include "logic/world/World.h"

namespace logic {

void World::addEntity(std::unique_ptr<logic::EntityModel> entity) {
    entities.push_back(std::move(entity));
}

void World::update(float deltaTime) {
    for (const auto& entity : entities) {
        entity->update(deltaTime);
    }
}

    void World::checkCollisions() {
        // TODO: Implement collision detection
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                if (entities[i]->intersects(*entities[j])) {
                    // Handle collision
                }
            }
        }
    }

}
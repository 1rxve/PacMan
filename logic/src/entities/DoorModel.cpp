#include "logic/entities/DoorModel.h"

namespace logic {
    DoorModel::DoorModel(float x, float y, float width, float height)
            : EntityModel(x, y, width, height) {
    }

    DoorModel::~DoorModel() {
        exitedGhosts.clear();
    }

    void DoorModel::update(float /*deltaTime*/) {
        notify();
    }

    bool DoorModel::canPass(const EntityModel* entity) const {
        if (!entity) return false;

        // PacMan can NEVER pass through door
        if (entity->isPacMan()) {
            return false;
        }

        // Ghosts inside spawn can pass through
        // Ghosts that have exited cannot re-enter
        if (entity->isGhost()) {
            return exitedGhosts.find(entity) == exitedGhosts.end();
        }

        // All other entities blocked (coins, fruits, etc.)
        return false;
    }

    void DoorModel::registerExit(const EntityModel* ghost) {
        if (ghost && ghost->isGhost()) {
            exitedGhosts.insert(ghost);
        }
    }
}
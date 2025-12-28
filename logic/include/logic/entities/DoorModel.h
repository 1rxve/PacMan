#ifndef PACMANGAME_DOORMODEL_H
#define PACMANGAME_DOORMODEL_H

#include "EntityModel.h"
#include <unordered_set>

namespace logic {
    class DoorModel : public EntityModel {
    private:
        // Track which ghosts have exited spawn (can't return)
        std::unordered_set<const EntityModel*> exitedGhosts;

    public:
        DoorModel(float x, float y, float width, float height);

        ~DoorModel();

        void update(float deltaTime) override;

        // Check if entity can pass through door
        bool canPass(const EntityModel* entity) const;

        // Register that a ghost has exited spawn
        void registerExit(const EntityModel* ghost);

        bool isDoor() const override { return true; }
    };
}

#endif // PACMANGAME_DOORMODEL_H
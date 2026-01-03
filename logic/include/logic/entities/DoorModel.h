#ifndef PACMANGAME_DOORMODEL_H
#define PACMANGAME_DOORMODEL_H

#include "EntityModel.h"

namespace logic {
/**
 * Represents the ghost house door in the maze center.
 *
 * Blocks PacMan from entering the ghost spawn area.
 * Ghosts can pass through when exiting spawn or returning after being eaten.
 * Collision handling is managed by World based on ghost state.
 */
class DoorModel : public EntityModel {
public:
    DoorModel(float x, float y, float width, float height);

    ~DoorModel();

    void update(float deltaTime) override;

    bool isDoor() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_DOORMODEL_H
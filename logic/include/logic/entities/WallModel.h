#ifndef PACMANGAME_WALLMODEL_H
#define PACMANGAME_WALLMODEL_H

#include "EntityModel.h"

namespace logic {
/**
 * Represents a static wall obstacle in the maze.
 * Blocks movement for both PacMan and ghosts.
 */
class WallModel : public EntityModel {
public:
    WallModel(float x, float y, float width, float height);

    void update(float deltaTime) override;

    bool isWall() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_WALLMODEL_H

#ifndef PACMANGAME_NOENTRYMODEL_H
#define PACMANGAME_NOENTRYMODEL_H

#include "EntityModel.h"
#include "GhostModel.h"
#include <unordered_set>

namespace logic {
/**
 * Invisible barrier that blocks specific ghost types.
 *
 * Used in spawn area to prevent ghosts from entering each other's
 * starting positions (BLUE can't enter ORANGE spot and vice versa).
 * Does not block PacMan or walls - only affects ghost pathfinding.
 */
class NoEntryModel : public EntityModel {
private:
    std::unordered_set<GhostType> blockedGhostTypes;

public:
    NoEntryModel(float x, float y, float width, float height);

    void update(float deltaTime) override;

    void addBlockedGhostType(GhostType type);
    bool blocksGhostType(GhostType type) const;

    void clearBlockedGhostTypes();

    bool isNoEntry() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_NOENTRYMODEL_H
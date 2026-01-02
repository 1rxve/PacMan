#ifndef PACMANGAME_NOENTRYMODEL_H
#define PACMANGAME_NOENTRYMODEL_H

#include "EntityModel.h"
#include "GhostModel.h"
#include <unordered_set>

namespace logic {
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
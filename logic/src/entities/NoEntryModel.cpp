#include "logic/entities/NoEntryModel.h"

namespace logic {
    NoEntryModel::NoEntryModel(float x, float y, float width, float height)
            : EntityModel(x, y, width, height) {
        // Default: block all ghost types
        blockedGhostTypes.insert(GhostType::RED);
        blockedGhostTypes.insert(GhostType::PINK);
        blockedGhostTypes.insert(GhostType::BLUE);
        blockedGhostTypes.insert(GhostType::ORANGE);
    }

    void NoEntryModel::update(float /*deltaTime*/) {
        notify();
    }

    void NoEntryModel::addBlockedGhostType(GhostType type) {
        blockedGhostTypes.insert(type);
    }

    bool NoEntryModel::blocksGhostType(GhostType type) const {
        return blockedGhostTypes.find(type) != blockedGhostTypes.end();
    }

    void NoEntryModel::clearBlockedGhostTypes() {
        blockedGhostTypes.clear();
    }
}
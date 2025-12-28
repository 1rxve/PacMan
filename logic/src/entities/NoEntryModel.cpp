#include "logic/entities/NoEntryModel.h"

namespace logic {
    NoEntryModel::NoEntryModel(float x, float y, float width, float height)
            : EntityModel(x, y, width, height) {
    }

    void NoEntryModel::update(float /*deltaTime*/) {
        notify();
    }
}
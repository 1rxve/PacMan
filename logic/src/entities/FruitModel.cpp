#include "logic/entities/FruitModel.h"

namespace logic {
    FruitModel::FruitModel(float x, float y, float width, float height)
            : EntityModel(x, y, width, height),
              collected(false) {
    }

    void FruitModel::update(float /*deltaTime*/) {
        // Fruits don't move, only notify for rendering

    }

    bool FruitModel::isCollected() const {
        return collected;
    }

    void FruitModel::collect() {
        collected = true;
    }

    void FruitModel::uncollect() {
        collected = false;
    }
}
#include "logic/entities/FruitModel.h"

namespace logic {
FruitModel::FruitModel(float x, float y, float width, float height)
    : EntityModel(x, y, width, height), collected(false) {}

void FruitModel::update(float /*deltaTime*/) {
}

bool FruitModel::isCollected() const { return collected; }

void FruitModel::collect() { collected = true; }

void FruitModel::uncollect() { collected = false; }
} // namespace logic
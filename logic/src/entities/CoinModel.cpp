#include "logic/entities/CoinModel.h"

namespace logic {
CoinModel::CoinModel(float x, float y, float width, float height)
    : EntityModel(x, y, width, height), collected(false) {}

void CoinModel::update(float /*deltaTime*/) {
}

bool CoinModel::isCollected() const { return collected; }

void CoinModel::collect() {
    collected = true;
}

void CoinModel::uncollect() { collected = false; }
} // namespace logic
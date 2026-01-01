#include "logic/entities/CoinModel.h"

namespace logic {
    CoinModel::CoinModel(float x, float y, float width, float height)
            : EntityModel(x, y, width, height),
              collected(false) {
    }

    void CoinModel::update(float /*deltaTime*/) {
        // Coins don't move, alleen notify voor rendering
    }

    bool CoinModel::isCollected() const {
        return collected;
    }

    void CoinModel::collect() {
        collected = true;
        // Coin verdwijnt - view zal dit detecteren
    }

    void CoinModel::uncollect() {
        collected = false;
    }
}
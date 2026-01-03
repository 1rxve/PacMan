#ifndef PACMANGAME_COINMODEL_H
#define PACMANGAME_COINMODEL_H

#include "EntityModel.h"

namespace logic {
/**
 * Represents a collectible coin in the game world.
 *
 * Coins are static entities that can be collected by PacMan.
 * Tracking collection state enables respawning coins for next level
 * without recreating entities.
 */
class CoinModel : public EntityModel {
private:
    bool collected;

public:
    CoinModel(float x, float y, float width, float height);

    void update(float deltaTime) override;

    bool isCollected() const;

    void collect();

    void uncollect();

    bool isCoin() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_COINMODEL_H
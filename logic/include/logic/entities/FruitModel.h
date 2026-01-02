#ifndef PACMANGAME_FRUITMODEL_H
#define PACMANGAME_FRUITMODEL_H

#include "EntityModel.h"

namespace logic {
class FruitModel : public EntityModel {
private:
    bool collected;

public:
    FruitModel(float x, float y, float width, float height);

    void update(float deltaTime) override;

    bool isCollected() const;
    void collect();

    void uncollect();

    bool isFruit() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_FRUITMODEL_H
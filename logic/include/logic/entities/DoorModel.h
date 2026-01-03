#ifndef PACMANGAME_DOORMODEL_H
#define PACMANGAME_DOORMODEL_H

#include "EntityModel.h"
#include <unordered_set>

namespace logic {
class DoorModel : public EntityModel {
public:
    DoorModel(float x, float y, float width, float height);

    ~DoorModel();

    void update(float deltaTime) override;

    bool isDoor() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_DOORMODEL_H
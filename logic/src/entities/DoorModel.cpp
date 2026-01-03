#include "logic/entities/DoorModel.h"

namespace logic {
DoorModel::DoorModel(float x, float y, float width, float height) : EntityModel(x, y, width, height) {}

DoorModel::~DoorModel() { exitedGhosts.clear(); }

void DoorModel::update(float /*deltaTime*/) {}
} // namespace logic
#include "logic/entities/WallModel.h"

namespace logic {
WallModel::WallModel(float x, float y, float width, float height) : EntityModel(x, y, width, height) {}

void WallModel::update(float /*deltaTime*/) {}
} // namespace logic
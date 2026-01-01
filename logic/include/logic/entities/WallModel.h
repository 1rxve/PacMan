#ifndef PACMANGAME_WALLMODEL_H
#define PACMANGAME_WALLMODEL_H

#include "EntityModel.h"

namespace logic {
    class WallModel : public EntityModel{
        public:
            WallModel(float x, float y, float width, float height);

            void update(float deltaTime) override;

        bool isWall() const override { return true; }

    };
}


#endif //PACMANGAME_WALLMODEL_H

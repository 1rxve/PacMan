#ifndef PACMANGAME_COINMODEL_H
#define PACMANGAME_COINMODEL_H

#include "EntityModel.h"

namespace logic {
    class CoinModel : public EntityModel {
    private:
        bool collected;

    public:
        CoinModel(float x, float y, float width, float height);

        void update(float deltaTime) override;

        bool isCollected() const;
        void collect();
    };
}

#endif // PACMANGAME_COINMODEL_H
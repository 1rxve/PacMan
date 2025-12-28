#ifndef PACMANGAME_GHOSTMODEL_H
#define PACMANGAME_GHOSTMODEL_H

#include "EntityModel.h"

namespace logic {
    enum class GhostType {
        RED,
        PINK,
        BLUE,
        ORANGE
    };

    enum class GhostState {
        SPAWNING,
        CHASING,
        FEAR,
        EATEN
    };

    class GhostModel : public EntityModel {
    private:
        GhostType type;
        GhostState state;
        float spawnDelay;
        float spawnTimer;
        float speed;

    public:
        GhostModel(float x, float y, float width, float height,
                   GhostType type, float spawnDelay);

        void update(float deltaTime) override;

        GhostType getType() const { return type; }
        GhostState getState() const { return state; }

        bool isGhost() const override { return true; }
    };
}


#endif //PACMANGAME_GHOSTMODEL_H

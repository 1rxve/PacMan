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

        Direction currentDirection;  // ← ADD

        // Grid info voor intersection detection
        float cellWidth;   // ← ADD
        float cellHeight;  // ← ADD

    public:
        GhostModel(float x, float y, float width, float height,
                   GhostType type, float spawnDelay);

        void update(float deltaTime) override;

        GhostType getType() const { return type; }
        GhostState getState() const { return state; }
        Direction getCurrentDirection() const { return currentDirection; }  // ← ADD

        void setCellDimensions(float cellW, float cellH);
        void setDirection(Direction dir);
        void stopMovement();

        bool isGhost() const override { return true; }
    };
}


#endif //PACMANGAME_GHOSTMODEL_H

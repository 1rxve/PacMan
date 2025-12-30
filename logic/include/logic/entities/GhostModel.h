#ifndef PACMANGAME_GHOSTMODEL_H
#define PACMANGAME_GHOSTMODEL_H

#include "EntityModel.h"
#include <vector>

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
        EATEN,
        RESPAWNING,
        EXITING_SPAWN
    };

    class GhostModel : public EntityModel {
    private:
        GhostType type;
        GhostState state;
        float spawnDelay;
        float spawnTimer;
        float speed;

        Direction currentDirection;

        // Grid info voor intersection detection
        float cellWidth;
        float cellHeight;

        bool hasExitedSpawn;

        float eatenRespawnX;  // ← ADD
        float eatenRespawnY;

        int exitStepCounter;

        float fearTimer;

        float respawnFlickerTimer;
        int respawnFlickerCount;

        Direction getReverseDirection(Direction dir) const;

    public:
        GhostModel(float x, float y, float width, float height,
                   GhostType type, float spawnDelay);

        void update(float deltaTime) override;

        GhostType getType() const { return type; }
        GhostState getState() const { return state; }
        Direction getCurrentDirection() const { return currentDirection; }

        bool hasExited() const { return hasExitedSpawn; }
        void markAsExited() { hasExitedSpawn = true; }

        void setCellDimensions(float cellW, float cellH);
        void setDirection(Direction dir);
        void stopMovement();

        bool needsDirectionDecision(const std::vector<Direction>& viableDirections) const;
        void makeDirectionDecision(const std::vector<Direction>& viableDirections,float targetX, float targetY);

        void enterFearMode();
        void exitFearMode();

        void setFearTimer(float timer) { fearTimer = timer; }  // ← ADD
        float getFearTimer() const { return fearTimer; }
        int getRespawnFlickerCount() const { return respawnFlickerCount; }

        bool isFeared() const { return state == GhostState::FEAR; }

        void getEaten();
        bool isEaten() const { return state == GhostState::EATEN; }

        void setEatenRespawnPosition(float x, float y);

        float getEatenRespawnX() const { return eatenRespawnX; }  // ← ADD
        float getEatenRespawnY() const { return eatenRespawnY; }

        void startExitingSpawn();

        void resetToSpawn(float delay);

        bool isGhost() const override { return true; }
    };
}


#endif //PACMANGAME_GHOSTMODEL_H

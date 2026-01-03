#ifndef PACMANGAME_GHOSTMODEL_H
#define PACMANGAME_GHOSTMODEL_H

#include "EntityModel.h"
#include <vector>

namespace logic {
/**
 * Ghost AI behavior types with different chase strategies.
 */
enum class GhostType {
    RED,    // Random-locked movement (50% keep current direction)
    PINK,   // Predictive chase (targets 4 tiles ahead of PacMan)
    BLUE,   // Predictive chase (same as PINK)
    ORANGE  // Direct chase (minimize Manhattan distance to PacMan)
};

/**
 * Ghost state machine states.
 *
 * Flow: SPAWNING → EXITING_SPAWN → CHASING ⇄ FEAR → EATEN → RESPAWNING → cycle
 */
enum class GhostState { SPAWNING, CHASING, FEAR, EATEN, RESPAWNING, EXITING_SPAWN };

/**
 * Represents a ghost enemy with AI-driven behavior.
 *
 * Each ghost type has distinct chase behavior (see GhostType).
 * State machine controls spawn delays, fear mode, and respawning after being eaten.
 *
 * Key mechanics:
 * - Collision-based pathfinding (no backwards movement at intersections)
 * - Fear mode: flees from PacMan, reduced speed
 * - Eaten state: eyes return to spawn, then respawn with flicker effect
 * - Hardcoded exit routes from spawn area per ghost type
 */
class GhostModel : public EntityModel {
private:
    GhostType type;
    GhostState state;
    float spawnDelay;
    float spawnTimer;
    float speed;

    float targetSpeed;

    Direction currentDirection;

    float cellWidth;
    float cellHeight;

    bool hasExitedSpawn;

    float eatenRespawnX;
    float eatenRespawnY;

    int exitStepCounter;

    float fearTimer;

    float respawnFlickerTimer;
    int respawnFlickerCount;

    Direction getReverseDirection(Direction dir) const;

public:
    GhostModel(float x, float y, float width, float height, GhostType type, float spawnDelay);

    void update(float deltaTime) override;

    GhostType getType() const { return type; }

    GhostState getState() const { return state; }

    Direction getCurrentDirection() const { return currentDirection; }

    bool hasExited() const { return hasExitedSpawn; }

    void markAsExited() { hasExitedSpawn = true; }

    void setCellDimensions(float cellW, float cellH);

    void setDirection(Direction dir);

    void stopMovement();

    /**
     * Checks if ghost should reconsider direction at current position.
     *
     * Returns true if:
     * - Multiple viable directions available (intersection)
     * - Current direction no longer viable (forced turn)
     *
     * @param viableDirections Directions that don't collide with walls
     * @return true if new direction decision needed
     */
    bool needsDirectionDecision(const std::vector<Direction>& viableDirections) const;

    /**
     * Determines next direction based on ghost type AI.
     *
     * Filters out backwards direction (no 180° turns).
     * AI behavior per type:
     * - RED: 50% keep direction, 50% random viable
     * - PINK/BLUE: Minimize distance to 4 tiles ahead of PacMan
     * - ORANGE: Minimize distance to PacMan's current position
     * - FEAR mode: Maximize distance from PacMan (flee behavior)
     *
     * @param viableDirections Directions that don't collide
     * @param targetX PacMan's X position
     * @param targetY PacMan's Y position
     * @param pacmanDirection Direction PacMan is facing (for PINK/BLUE prediction)
     */
    void makeDirectionDecision(const std::vector<Direction>& viableDirections, float targetX, float targetY,
                               Direction pacmanDirection);

    void enterFearMode();

    void exitFearMode();

    void setFearTimer(float timer) { fearTimer = timer; }

    float getFearTimer() const { return fearTimer; }

    int getRespawnFlickerCount() const { return respawnFlickerCount; }

    bool isFeared() const { return state == GhostState::FEAR; }

    void getEaten();

    bool isEaten() const { return state == GhostState::EATEN; }

    void setEatenRespawnPosition(float x, float y);

    float getEatenRespawnX() const { return eatenRespawnX; }

    float getEatenRespawnY() const { return eatenRespawnY; }

    void startExitingSpawn();

    void resetToSpawn(float delay);

    void setSpeed(float newSpeed);

    bool isGhost() const override { return true; }

    float getSpeed() const { return speed; }

    float getTargetSpeed() const { return targetSpeed; }
};
} // namespace logic

#endif // PACMANGAME_GHOSTMODEL_H

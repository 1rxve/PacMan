#ifndef PACMANGAME_PACMANMODEL_H
#define PACMANGAME_PACMANMODEL_H

#include "EntityModel.h"

namespace logic {

/**
 * Represents the player-controlled PacMan character.
 *
 * Key mechanics:
 * - Buffered input: nextDirection applied when valid (responsive controls)
 * - Center-locking: auto-aligns to grid perpendicular to movement direction
 * - Death animation: 2-second animation before respawn
 * - Lives system: 3 lives, respawn on death until depleted
 */
class PacManModel : public EntityModel {
private:
    float speed;
    int lives;

    Direction currentDirection;
    Direction nextDirection;

    float cellWidth;
    float cellHeight;

    bool isDying;
    float deathTimer;

public:
    PacManModel(float x, float y, float width, float height, float speed = 0.5f);

    void update(float deltaTime) override;

    int getLives() const;

    void loseLife();

    float getSpeed() const;

    Direction getCurrentDirection() const;

    Direction getNextDirection() const;

    void setSpeed(float speed);

    void setNextDirection(Direction direction);

    /**
     * Applies buffered next direction to current direction.
     *
     * Called by World when nextDirection becomes valid (no wall collision).
     * Enables responsive controls - player input buffered until executable.
     * Clears nextDirection buffer after applying.
     */
    void applyNextDirection();

    void stopMovement();

    /**
     * Sets grid cell dimensions for center-locking movement.
     *
     * Center-locking aligns PacMan to grid centerline perpendicular
     * to movement direction (prevents diagonal drift, smooth corridor navigation).
     *
     * @param cellW Width of one grid cell in normalized coordinates
     * @param cellH Height of one grid cell in normalized coordinates
     */
    void setCellDimensions(float cellW, float cellH);

    bool getIsDying() const { return isDying; }

    float getDeathTimer() const { return deathTimer; }

    void startDeath();

    void updateDeath(float deltaTime);

    void respawn(float spawnX, float spawnY);

    bool isPacMan() const override { return true; }
};
} // namespace logic

#endif // PACMANGAME_PACMANMODEL_H
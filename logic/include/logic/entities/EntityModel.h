#ifndef PACMANGAME_ENTITYMODEL_H
#define PACMANGAME_ENTITYMODEL_H

#include "logic/patterns/Subject.h"

namespace logic {

/**
 * Movement directions for entities in the game world.
 */
enum class Direction { NONE, LEFT, RIGHT, UP, DOWN };

/**
 * Base class for all game entities (PacMan, ghosts, walls, coins, etc.).
 *
 * Provides common functionality:
 * - Position and size in normalized coordinates [-1, 1]
 * - AABB collision detection
 * - Observer pattern integration (inherits from Subject)
 * - Type identification via virtual is*() methods
 *
 * All entities notify their observers on state changes for rendering.
 */
class EntityModel : public Subject {
protected:
    float x;
    float y;
    float width;
    float height;

public:
    EntityModel(float x, float y, float width, float height);

    virtual ~EntityModel();

    virtual void update(float deltaTime) = 0;

    float getX() const;

    float getY() const;

    float getWidth() const;

    float getHeight() const;

    void setPosition(float newX, float newY);

    /**
     * Axis-Aligned Bounding Box (AABB) collision detection.
     *
     * @param other Entity to check collision with
     * @return true if bounding boxes intersect
     */
    bool intersects(const EntityModel& other) const;

    virtual bool isPacMan() const { return false; }

    virtual bool isWall() const { return false; }

    virtual bool isCoin() const { return false; }

    virtual bool isGhost() const { return false; }

    virtual bool isDoor() const { return false; }

    virtual bool isNoEntry() const { return false; }

    virtual bool isFruit() const { return false; }
};
} // namespace logic
#endif // PACMANGAME_ENTITYMODEL_H

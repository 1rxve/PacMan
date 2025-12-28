#ifndef PACMANGAME_PACMANMODEL_H
#define PACMANGAME_PACMANMODEL_H

#include "EntityModel.h"

namespace logic {

    class PacManModel : public EntityModel {
    private:
        float speed;
        int lives;

        Direction currentDirection;
        Direction nextDirection;

        // Grid info voor center-locking
        float cellWidth;
        float cellHeight;

    public:
        PacManModel(float x, float y, float width, float height, float speed = 0.5f);

        void update(float deltaTime) override;

        int getLives() const;

        void loseLife();

        float getSpeed() const;

        Direction getCurrentDirection() const;

        Direction getNextDirection() const;  // NIEUW

        void setSpeed(float speed);

        void setNextDirection(Direction direction);

        void applyNextDirection();  // NIEUW

        void stopMovement();

        void setCellDimensions(float cellW, float cellH);

        bool isPacMan() const override { return true; }

    };
}

#endif //PACMANGAME_PACMANMODEL_H
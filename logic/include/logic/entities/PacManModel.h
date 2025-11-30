#ifndef PACMANGAME_PACMANMODEL_H
#define PACMANGAME_PACMANMODEL_H

#include "EntityModel.h"

namespace logic {
    enum Direction {
        NONE,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    class PacManModel : public EntityModel {
    private:
        float speed;
        int lives;

        Direction currentDirection;
        Direction nextDirection;

        float prevX, prevY;

    public:
        PacManModel(float x, float y, float width, float height, float speed = 0.5f);

        void update(float deltaTime) override;

        int getLives() const;

        void loseLife();

        float getSpeed() const;

        Direction getCurrentDirection() const;

        void setSpeed(float speed);

        void setNextDirection(Direction direction);

        void restorePreviousPosition();

        void stopMovement();
    };
}


#endif //PACMANGAME_PACMANMODEL_H

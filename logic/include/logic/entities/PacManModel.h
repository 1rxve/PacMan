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

public:
    PacManModel(float x, float y, float width, float height, float speed = 0.5f);

    Direction getCurrentDirection() const;
    void setNextDirection(Direction direction);

    int getLives() const;
    void loseLife();

    float getSpeed() const;

    void setSpeed(float speed);

    void update(float deltaTime) override;
};

}


#endif //PACMANGAME_PACMANMODEL_H

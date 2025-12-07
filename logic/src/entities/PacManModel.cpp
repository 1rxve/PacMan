#include "logic/entities/PacManModel.h"

namespace logic {
    PacManModel::PacManModel(float x, float y, float width, float height, float speed)
            : EntityModel(x, y, width, height),
              speed(speed),
              lives(3),
              currentDirection(Direction::NONE),
              nextDirection(Direction::NONE) {
    }

    void PacManModel::update(float deltaTime) {
        // Als er een nieuwe richting is aangevraagd, gebruik die
        if (nextDirection != Direction::NONE) {
            currentDirection = nextDirection;
        }

        // Bereken hoeveel we willen bewegen (World checkt of dit safe is)
        float moveDistance = speed * deltaTime;
        float newX = x;
        float newY = y;

        switch (currentDirection) {
            case Direction::LEFT:
                newX -= moveDistance;
                break;
            case Direction::RIGHT:
                newX += moveDistance;
                break;
            case Direction::UP:
                newY -= moveDistance;
                break;
            case Direction::DOWN:
                newY += moveDistance;
                break;
            case Direction::NONE:
                break;
        }

        // Zet nieuwe positie (World heeft dit al gecheckt)
        setPosition(newX, newY);

        notify();
    }

    int PacManModel::getLives() const {
        return lives;
    }

    void PacManModel::loseLife() {
        lives--;
    }

    float PacManModel::getSpeed() const {
        return speed;
    }

    Direction PacManModel::getCurrentDirection() const {
        return currentDirection;
    }

    void PacManModel::setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void PacManModel::setNextDirection(Direction direction) {
        nextDirection = direction;
    }

    void PacManModel::stopMovement() {
        currentDirection = Direction::NONE;
        nextDirection = Direction::NONE;
    }
}
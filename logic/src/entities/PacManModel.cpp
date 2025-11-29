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
        // Als er een nieuwe richting is aangevraagd, probeer die te gebruiken
        if (nextDirection != Direction::NONE) {
            currentDirection = nextDirection;
            // nextDirection = Direction::NONE;  // Optioneel: reset na gebruik
        }

        float newX = getX();
        float newY = getY();

        switch (currentDirection) {
            case Direction::LEFT:
                newX -= speed * deltaTime;
                break;
            case Direction::RIGHT:
                newX += speed * deltaTime;
                break;
            case Direction::UP:
                newY -= speed * deltaTime;
                break;
            case Direction::DOWN:
                newY += speed * deltaTime;
                break;
            case Direction::NONE:
                break;
        }

        setPosition(newX, newY);
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

    void PacManModel::setSpeed(float speed) {
        PacManModel::speed = speed;
    }

    void PacManModel::setNextDirection(Direction direction) {
        nextDirection = direction;
    }
}

#include "logic/entities/PacManModel.h"
#include <cmath>

namespace logic {
    PacManModel::PacManModel(float x, float y, float width, float height, float speed)
            : EntityModel(x, y, width, height),
              speed(speed),
              lives(3),
              currentDirection(Direction::NONE),
              nextDirection(Direction::NONE),
              cellWidth(0.0f),
              cellHeight(0.0f) {
    }

    void PacManModel::update(float deltaTime) {
        if (nextDirection != Direction::NONE) {
            currentDirection = nextDirection;
        }

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

        // CENTER-LOCKING
        if (cellWidth > 0.0f && cellHeight > 0.0f) {
            if (currentDirection == Direction::LEFT || currentDirection == Direction::RIGHT) {
                float gridY = std::floor((newY + 1.0f) / cellHeight);
                float centerY = -1.0f + cellHeight / 2.0f + gridY * cellHeight;
                newY = centerY;

            } else if (currentDirection == Direction::UP || currentDirection == Direction::DOWN) {
                float gridX = std::floor((newX + 1.0f) / cellWidth);
                float centerX = -1.0f + cellWidth / 2.0f + gridX * cellWidth;
                newX = centerX;
            }
        }

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

    void PacManModel::setCellDimensions(float cellW, float cellH) {
        cellWidth = cellW;
        cellHeight = cellH;
    }
}
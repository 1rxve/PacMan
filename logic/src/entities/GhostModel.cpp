#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <cmath>
#include <vector>  // ← ADD

namespace logic {
    GhostModel::GhostModel(float x, float y, float width, float height,
                           GhostType type, float spawnDelay)
            : EntityModel(x, y, width, height),
              type(type),
              state(GhostState::SPAWNING),
              spawnDelay(spawnDelay),
              spawnTimer(0.0f),
              speed(0.5f),
              currentDirection(Direction::NONE),
              cellWidth(0.0f),
              cellHeight(0.0f) {
    }

    void GhostModel::update(float deltaTime) {
        // SPAWNING state - wacht op delay
        if (state == GhostState::SPAWNING) {
            spawnTimer += deltaTime;

            if (spawnTimer >= spawnDelay) {
                state = GhostState::CHASING;

                // BLIJF Direction::NONE - World zal viable direction kiezen
                // (We kunnen hier niet checken - ghost heeft geen toegang tot walls)
            }

            notify();
            return;
        }

        // CHASING state - basic movement
        if (state == GhostState::CHASING) {
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

            // CENTER-LOCKING (keep this - het helpt)
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
            return;
        }

        notify();
    }

    void GhostModel::setCellDimensions(float cellW, float cellH) {
        cellWidth = cellW;
        cellHeight = cellH;
    }

    // ← ADD PUBLIC METHOD
    void GhostModel::setDirection(Direction dir) {
        currentDirection = dir;
    }

    void GhostModel::stopMovement() {  // ← ADD
        currentDirection = Direction::NONE;
    }
}
#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace logic {
GhostModel::GhostModel(float x, float y, float width, float height, GhostType type, float spawnDelay)
    : EntityModel(x, y, width, height), type(type), state(GhostState::SPAWNING), spawnDelay(spawnDelay),
      spawnTimer(0.0f), speed(0.5f), targetSpeed(0.5f), currentDirection(Direction::NONE), cellWidth(0.0f),
      cellHeight(0.0f), hasExitedSpawn(false), eatenRespawnX(x), eatenRespawnY(y), exitStepCounter(0), fearTimer(0.0f),
      respawnFlickerTimer(0.0f), respawnFlickerCount(0) {}

void GhostModel::update(float deltaTime) {
    if (state == GhostState::SPAWNING) {
        spawnTimer += deltaTime;

        if (spawnTimer >= spawnDelay) {
            if (type == GhostType::RED && hasExitedSpawn) {
                state = GhostState::CHASING;
            } else {
                startExitingSpawn();
            }
        }
        return;
    }

    if (state == GhostState::EATEN) {
        float moveDistance = speed * deltaTime;
        float newX = x;
        float newY = y;

        float dx = eatenRespawnX - x;
        float dy = eatenRespawnY - y;

        // Threshold prevents oscillation when approaching spawn point
        const float SPAWN_THRESHOLD = 0.05f;
        if (std::abs(dx) < SPAWN_THRESHOLD && std::abs(dy) < SPAWN_THRESHOLD) {
            setPosition(eatenRespawnX, eatenRespawnY);
            state = GhostState::RESPAWNING;
            respawnFlickerTimer = 0.0f;
            respawnFlickerCount = 0;
            return;
        }

        // Simple greedy pathfinding (dominant axis first)
        if (std::abs(dx) > std::abs(dy)) {
            if (dx > 0) {
                currentDirection = Direction::RIGHT;
                newX += moveDistance;
            } else {
                currentDirection = Direction::LEFT;
                newX -= moveDistance;
            }
        } else {
            if (dy > 0) {
                currentDirection = Direction::DOWN;
                newY += moveDistance;
            } else {
                currentDirection = Direction::UP;
                newY -= moveDistance;
            }
        }

        setPosition(newX, newY);
        return;
    }

    if (state == GhostState::RESPAWNING) {
        respawnFlickerTimer += deltaTime;

        // 6 flickers × 0.3s = 1.8s visual feedback before exiting
        const float FLICKER_DURATION = 0.3f;

        if (respawnFlickerTimer >= FLICKER_DURATION) {
            respawnFlickerTimer = 0.0f;
            respawnFlickerCount++;

            if (respawnFlickerCount >= 6) {
                startExitingSpawn();
            }
        }
        return;
    }

    // Hardcoded exit routes per ghost type (spawn area navigation)
    if (state == GhostState::EXITING_SPAWN) {
        float moveDistance = speed * deltaTime;
        float newX = x;
        float newY = y;

        if (type == GhostType::RED) {
            if (currentDirection == Direction::NONE) {
                currentDirection = Direction::UP;
            }

            if (currentDirection == Direction::UP) {
                exitStepCounter++;
                if (exitStepCounter > 15) {
                    currentDirection = Direction::LEFT;
                    exitStepCounter = 0;
                }
            }

            if (currentDirection == Direction::LEFT) {
                exitStepCounter++;
                if (exitStepCounter > 10) {
                    state = GhostState::CHASING;
                    hasExitedSpawn = true;
                    exitStepCounter = 0;
                }
            }
        }

        if (type == GhostType::PINK) {
            if (currentDirection == Direction::NONE) {
                currentDirection = Direction::UP;
            }

            if (currentDirection == Direction::UP) {
                exitStepCounter++;
                if (exitStepCounter > 15) {
                    currentDirection = Direction::LEFT;
                    exitStepCounter = 0;
                }
            }

            if (currentDirection == Direction::LEFT) {
                exitStepCounter++;
                if (exitStepCounter > 10) {
                    state = GhostState::CHASING;
                    hasExitedSpawn = true;
                    exitStepCounter = 0;
                }
            }
        }

        if (type == GhostType::BLUE) {
            if (currentDirection == Direction::NONE) {
                currentDirection = Direction::RIGHT;
            }

            if (currentDirection == Direction::UP) {
                exitStepCounter++;
                if (exitStepCounter > 15) {
                    currentDirection = Direction::RIGHT;
                    exitStepCounter = 0;
                }
            }

            if (currentDirection == Direction::RIGHT && exitStepCounter > 0) {
                exitStepCounter++;
                if (exitStepCounter > 10) {
                    state = GhostState::CHASING;
                    hasExitedSpawn = true;
                    exitStepCounter = 0;
                }
            }
        }

        if (type == GhostType::ORANGE) {
            if (currentDirection == Direction::NONE) {
                currentDirection = Direction::LEFT;
            }

            if (currentDirection == Direction::LEFT && exitStepCounter > 0) {
                exitStepCounter++;
                if (exitStepCounter > 10) {
                    state = GhostState::CHASING;
                    hasExitedSpawn = true;
                    exitStepCounter = 0;
                }
            }

            if (currentDirection == Direction::UP && exitStepCounter == 0) {
                exitStepCounter = 1;
            }
        }

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

        setPosition(newX, newY);
        return;
    }

    if (state == GhostState::CHASING || state == GhostState::FEAR) {
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

        // CENTER-LOCKING: Auto-align to grid perpendicular to movement direction
        // Prevents diagonal drift, ensures clean corridor navigation
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
        return;
    }
}

void GhostModel::setCellDimensions(float cellW, float cellH) {
    cellWidth = cellW;
    cellHeight = cellH;
}

void GhostModel::setDirection(Direction dir) { currentDirection = dir; }

void GhostModel::stopMovement() { currentDirection = Direction::NONE; }

Direction GhostModel::getReverseDirection(Direction dir) const {
    switch (dir) {
    case Direction::UP:
        return Direction::DOWN;
    case Direction::DOWN:
        return Direction::UP;
    case Direction::LEFT:
        return Direction::RIGHT;
    case Direction::RIGHT:
        return Direction::LEFT;
    case Direction::NONE:
        return Direction::NONE;
    }
    return Direction::NONE;
}

bool GhostModel::needsDirectionDecision(const std::vector<Direction>& viableDirections) const {
    Direction reverse = getReverseDirection(currentDirection);

    int optionCount = 0;
    bool currentStillViable = false;

    for (Direction dir : viableDirections) {
        if (dir == reverse)
            continue;
        optionCount++;
        if (dir == currentDirection) {
            currentStillViable = true;
        }
    }

    return (optionCount >= 2) || (!currentStillViable && optionCount > 0);
}

void GhostModel::makeDirectionDecision(const std::vector<Direction>& viableDirections, float targetX, float targetY,
                                       Direction pacmanDirection) {
    if (viableDirections.empty()) {
        currentDirection = Direction::NONE;
        return;
    }

    Direction reverse = getReverseDirection(currentDirection);

    // No 180° turns - prevents ping-ponging behavior
    std::vector<Direction> validOptions;
    for (Direction dir : viableDirections) {
        if (dir != reverse) {
            validOptions.push_back(dir);
        }
    }

    if (validOptions.empty()) {
        currentDirection = Direction::NONE;
        return;
    }

    // FEAR MODE: Maximize distance from PacMan (flee behavior)
    if (state == GhostState::FEAR) {
        Direction bestDirection = Direction::NONE;
        float maxDistance = -1.0f;

        for (Direction dir : validOptions) {
            float testX = x;
            float testY = y;

            switch (dir) {
            case Direction::LEFT:
                testX -= cellWidth;
                break;
            case Direction::RIGHT:
                testX += cellWidth;
                break;
            case Direction::UP:
                testY -= cellHeight;
                break;
            case Direction::DOWN:
                testY += cellHeight;
                break;
            case Direction::NONE:
                break;
            }

            float distance = std::abs(testX - targetX) + std::abs(testY - targetY);

            // Tie-breaking: prefer current direction (smooth movement)
            const float EPSILON = 0.01f;
            bool isTie = std::abs(distance - maxDistance) < EPSILON;

            if (distance > maxDistance || (isTie && dir == currentDirection)) {
                maxDistance = distance;
                bestDirection = dir;
            }
        }

        currentDirection = bestDirection;
        return;
    }

    // RED: 50% keep direction, 50% random (unpredictable movement)
    if (type == GhostType::RED) {
        float roll = Random::getInstance().getFloat(0.0f, 1.0f);

        if (roll < 0.5f) {
            auto it = std::find(validOptions.begin(), validOptions.end(), currentDirection);
            if (it != validOptions.end()) {
                return;
            }
        }

        int randomIndex = Random::getInstance().getInt(0, static_cast<int>(validOptions.size()) - 1);
        currentDirection = validOptions[randomIndex];
        return;
    }

    // ORANGE: Direct chase (minimize distance to current PacMan position)
    if (type == GhostType::ORANGE) {
        Direction bestDirection = Direction::NONE;
        float minDistance = 999999.0f;

        for (Direction dir : validOptions) {
            float testX = x;
            float testY = y;

            switch (dir) {
            case Direction::LEFT:
                testX -= cellWidth;
                break;
            case Direction::RIGHT:
                testX += cellWidth;
                break;
            case Direction::UP:
                testY -= cellHeight;
                break;
            case Direction::DOWN:
                testY += cellHeight;
                break;
            case Direction::NONE:
                break;
            }

            float distance = std::abs(testX - targetX) + std::abs(testY - targetY);

            const float EPSILON = 0.01f;
            bool isTie = std::abs(distance - minDistance) < EPSILON;

            if (distance < minDistance || (isTie && dir == currentDirection)) {
                minDistance = distance;
                bestDirection = dir;
            }
        }

        currentDirection = bestDirection;
        return;
    }

    // PINK: Predictive chase (target 4 tiles ahead of PacMan)
    if (type == GhostType::PINK) {
        float predictedX = targetX;
        float predictedY = targetY;

        switch (pacmanDirection) {
        case Direction::UP:
            predictedY -= 4 * cellHeight;
            break;
        case Direction::DOWN:
            predictedY += 4 * cellHeight;
            break;
        case Direction::LEFT:
            predictedX -= 4 * cellWidth;
            break;
        case Direction::RIGHT:
            predictedX += 4 * cellWidth;
            break;
        case Direction::NONE:
            break;
        }

        Direction bestDirection = Direction::NONE;
        float minDistance = 999999.0f;

        for (Direction dir : validOptions) {
            float testX = x;
            float testY = y;

            switch (dir) {
            case Direction::LEFT:
                testX -= cellWidth;
                break;
            case Direction::RIGHT:
                testX += cellWidth;
                break;
            case Direction::UP:
                testY -= cellHeight;
                break;
            case Direction::DOWN:
                testY += cellHeight;
                break;
            case Direction::NONE:
                break;
            }

            // Manhattan distance to PREDICTED position (not current)
            float distance = std::abs(testX - predictedX) + std::abs(testY - predictedY);

            const float EPSILON = 0.01f;
            bool isTie = std::abs(distance - minDistance) < EPSILON;

            if (distance < minDistance || (isTie && dir == currentDirection)) {
                minDistance = distance;
                bestDirection = dir;
            }
        }

        currentDirection = bestDirection;
        return;
    }

    // BLUE: Same predictive behavior as PINK
    if (type == GhostType::BLUE) {
        float predictedX = targetX;
        float predictedY = targetY;

        switch (pacmanDirection) {
        case Direction::UP:
            predictedY -= 4 * cellHeight;
            break;
        case Direction::DOWN:
            predictedY += 4 * cellHeight;
            break;
        case Direction::LEFT:
            predictedX -= 4 * cellWidth;
            break;
        case Direction::RIGHT:
            predictedX += 4 * cellWidth;
            break;
        case Direction::NONE:
            break;
        }

        Direction bestDirection = Direction::NONE;
        float minDistance = 999999.0f;

        for (Direction dir : validOptions) {
            float testX = x;
            float testY = y;

            switch (dir) {
            case Direction::LEFT:
                testX -= cellWidth;
                break;
            case Direction::RIGHT:
                testX += cellWidth;
                break;
            case Direction::UP:
                testY -= cellHeight;
                break;
            case Direction::DOWN:
                testY += cellHeight;
                break;
            case Direction::NONE:
                break;
            }

            float distance = std::abs(testX - predictedX) + std::abs(testY - predictedY);

            const float EPSILON = 0.01f;
            bool isTie = std::abs(distance - minDistance) < EPSILON;

            if (distance < minDistance || (isTie && dir == currentDirection)) {
                minDistance = distance;
                bestDirection = dir;
            }
        }

        currentDirection = bestDirection;
        return;
    }
}

void GhostModel::enterFearMode() {
    if (state == GhostState::CHASING) {
        state = GhostState::FEAR;
        speed = 0.25f;
        currentDirection = getReverseDirection(currentDirection);
    }
}

void GhostModel::exitFearMode() {
    if (state == GhostState::FEAR || state == GhostState::EATEN) {
        state = GhostState::CHASING;
        speed = targetSpeed;
    }
}

void GhostModel::getEaten() {
    // Eyes navigate back to spawn (no teleport)
    state = GhostState::EATEN;
    speed = 1.0f;
    hasExitedSpawn = false; // Can pass through door
}

void GhostModel::setEatenRespawnPosition(float x, float y) {
    eatenRespawnX = x;
    eatenRespawnY = y;
}

void GhostModel::startExitingSpawn() {
    state = GhostState::EXITING_SPAWN;
    speed = targetSpeed;
    currentDirection = Direction::NONE;
    exitStepCounter = 0;
    hasExitedSpawn = false;
}

void GhostModel::resetToSpawn(float delay) {
    state = GhostState::SPAWNING;
    spawnTimer = 0.0f;
    spawnDelay = delay;
    speed = targetSpeed;
    currentDirection = Direction::NONE;
    exitStepCounter = 0;
    hasExitedSpawn = false;
}

void GhostModel::setSpeed(float newSpeed) {
    speed = newSpeed;
    targetSpeed = newSpeed;
}
} // namespace logic
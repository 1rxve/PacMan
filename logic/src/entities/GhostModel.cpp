#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

namespace logic {
    GhostModel::GhostModel(float x, float y, float width, float height,
                           GhostType type, float spawnDelay)
            : EntityModel(x, y, width, height),
              type(type),
              state(GhostState::SPAWNING),
              spawnDelay(spawnDelay),
              spawnTimer(0.0f),
              speed(0.5f),
              targetSpeed(0.5f),
              currentDirection(Direction::NONE),
              cellWidth(0.0f),
              cellHeight(0.0f),
              hasExitedSpawn(false),
              eatenRespawnX(x),
              eatenRespawnY(y),
              exitStepCounter(0),
              fearTimer(0.0f),
              respawnFlickerTimer(0.0f),
              respawnFlickerCount(0){
    }

    void GhostModel::update(float deltaTime) {
        static int counter = 0;
        if (counter++ % 60 == 0 && (state == GhostState::CHASING || state == GhostState::EXITING_SPAWN)) {
            std::cout << "Ghost " << (int)type << " state=" << (int)state
                      << " speed=" << speed << " target=" << targetSpeed << std::endl;
        }
        // SPAWNING state - wacht op delay
        if (state == GhostState::SPAWNING) {
            spawnTimer += deltaTime;

            // ← ADD: Debug voor BLUE
            if (type == GhostType::BLUE) {
                std::cout << "BLUE SPAWNING - timer: " << spawnTimer << "/" << spawnDelay << std::endl;
            }

            if (spawnTimer >= spawnDelay) {
                // RED initial spawn - already outside, marked as exited
                if (type == GhostType::RED && hasExitedSpawn) {
                    state = GhostState::CHASING;
                    std::cout << "RED initial spawn -> CHASING (already outside)" << std::endl;
                }
                    // All other cases - start exit sequence (including RED after eaten)
                else {
                    startExitingSpawn();
                    std::cout << "Ghost type " << (int)type << " -> EXITING_SPAWN" << std::endl;
                }
            }

            return;
        }

        // RESPAWNING state - flicker effect in spawn
        if (state == GhostState::RESPAWNING) {
            respawnFlickerTimer += deltaTime;

            const float FLICKER_DURATION = 0.3f;

            if (respawnFlickerTimer >= FLICKER_DURATION) {
                respawnFlickerTimer = 0.0f;
                respawnFlickerCount++;

                if (respawnFlickerCount >= 6) {
                    startExitingSpawn();
                    std::cout << "Flicker complete - starting exit" << std::endl;
                }
            }

            return;
        }

        // EXITING_SPAWN state - per-ghost hardcoded exit routes
        if (state == GhostState::EXITING_SPAWN) {
            float moveDistance = speed * deltaTime;
            float newX = x;
            float newY = y;

            // RED: Direct UP exit (spawns in center, no LEFT needed)
            if (type == GhostType::RED) {
                if (currentDirection == Direction::NONE) {
                    currentDirection = Direction::UP;  // ← CHANGE: Start UP, not LEFT
                    std::cout << "RED starting UP exit" << std::endl;
                }

                // Count UP frames
                if (currentDirection == Direction::UP) {
                    exitStepCounter++;

                    // After moving UP through door, switch to LEFT
                    if (exitStepCounter > 15) {
                        currentDirection = Direction::LEFT;
                        exitStepCounter = 0;
                    }
                }

                // Count LEFT frames after exiting
                if (currentDirection == Direction::LEFT) {
                    exitStepCounter++;

                    if (exitStepCounter > 10) {
                        state = GhostState::CHASING;
                        hasExitedSpawn = true;
                        exitStepCounter = 0;
                        std::cout << "RED exit complete -> CHASING" << std::endl;
                    }
                }
            }

            // PINK: Direct UP exit (spawns in center like RED)
            if (type == GhostType::PINK) {
                if (currentDirection == Direction::NONE) {
                    currentDirection = Direction::UP;
                    std::cout << "PINK starting UP exit" << std::endl;
                }

                // Count UP frames
                if (currentDirection == Direction::UP) {
                    exitStepCounter++;

                    // After moving UP through door, switch to LEFT
                    if (exitStepCounter > 15) {
                        currentDirection = Direction::LEFT;
                        exitStepCounter = 0;
                    }
                }

                // Count LEFT frames after exiting
                if (currentDirection == Direction::LEFT) {
                    exitStepCounter++;

                    if (exitStepCounter > 10) {
                        state = GhostState::CHASING;
                        hasExitedSpawn = true;
                        exitStepCounter = 0;
                        std::cout << "PINK exit complete -> CHASING" << std::endl;
                    }
                }
            }

            // BLUE: RIGHT → UP → RIGHT exit
            if (type == GhostType::BLUE) {
                if (currentDirection == Direction::NONE) {
                    currentDirection = Direction::RIGHT;
                    std::cout << "BLUE starting RIGHT exit" << std::endl;
                }

                // Phase 1: RIGHT until collision
                if (currentDirection == Direction::RIGHT && exitStepCounter == 0) {
                    // Collision handler will switch to UP
                }

                // Phase 2: UP through door
                if (currentDirection == Direction::UP) {
                    exitStepCounter++;

                    // After moving UP through door, switch to RIGHT
                    if (exitStepCounter > 15) {
                        currentDirection = Direction::RIGHT;
                        exitStepCounter = 0;
                    }
                }

                // Phase 3: RIGHT until free
                if (currentDirection == Direction::RIGHT && exitStepCounter > 0) {
                    exitStepCounter++;

                    if (exitStepCounter > 10) {
                        state = GhostState::CHASING;
                        hasExitedSpawn = true;
                        exitStepCounter = 0;
                        std::cout << "BLUE exit complete -> CHASING" << std::endl;
                    }
                }
            }

            // ORANGE: Collision-driven (LEFT until barrier, then UP)
            if (type == GhostType::ORANGE) {
                if (currentDirection == Direction::NONE) {
                    currentDirection = Direction::LEFT;
                    std::cout << "ORANGE starting LEFT" << std::endl;
                }

                // Count LEFT frames after switching from UP
                if (currentDirection == Direction::LEFT && exitStepCounter > 0) {
                    exitStepCounter++;

                    if (exitStepCounter > 10) {
                        state = GhostState::CHASING;
                        hasExitedSpawn = true;
                        exitStepCounter = 0;
                        std::cout << "ORANGE exit complete -> CHASING" << std::endl;
                    }
                }

                // Mark when switched to UP
                if (currentDirection == Direction::UP && exitStepCounter == 0) {
                    exitStepCounter = 1;
                }
            }

            // Apply movement
            switch (currentDirection) {
                case Direction::LEFT:  newX -= moveDistance; break;
                case Direction::RIGHT: newX += moveDistance; break;
                case Direction::UP:    newY -= moveDistance; break;
                case Direction::DOWN:  newY += moveDistance; break;
                case Direction::NONE:  break;
            }

            setPosition(newX, newY);
            return;
        }

        // CHASING OR FEAR state - basic movement
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
            return;
        }
    }

    void GhostModel::setCellDimensions(float cellW, float cellH) {
        cellWidth = cellW;
        cellHeight = cellH;
    }

    void GhostModel::setDirection(Direction dir) {
        currentDirection = dir;
    }

    void GhostModel::stopMovement() {
        currentDirection = Direction::NONE;
    }

    Direction GhostModel::getReverseDirection(Direction dir) const {
        switch (dir) {
            case Direction::UP: return Direction::DOWN;
            case Direction::DOWN: return Direction::UP;
            case Direction::LEFT: return Direction::RIGHT;
            case Direction::RIGHT: return Direction::LEFT;
            case Direction::NONE: return Direction::NONE;
        }
        return Direction::NONE;
    }

    bool GhostModel::needsDirectionDecision(const std::vector<Direction>& viableDirections) const {
        Direction reverse = getReverseDirection(currentDirection);

        int optionCount = 0;
        bool currentStillViable = false;

        for (Direction dir : viableDirections) {
            if (dir == reverse) continue;
            optionCount++;
            if (dir == currentDirection) {
                currentStillViable = true;
            }
        }

        return (optionCount >= 2) || (!currentStillViable && optionCount > 0);
    }

    void GhostModel::makeDirectionDecision(const std::vector<Direction>& viableDirections,
                                           float targetX, float targetY, Direction pacmanDirection) {
        if (viableDirections.empty()) {
            currentDirection = Direction::NONE;
            return;
        }

        Direction reverse = getReverseDirection(currentDirection);

        // Filter out backwards direction
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

        // FEAR MODE - Flee behavior (maximize distance)
        if (state == GhostState::FEAR) {
            Direction bestDirection = Direction::NONE;
            float maxDistance = -1.0f;

            for (Direction dir : validOptions) {
                float testX = x;
                float testY = y;

                switch (dir) {
                    case Direction::LEFT:  testX -= cellWidth; break;
                    case Direction::RIGHT: testX += cellWidth; break;
                    case Direction::UP:    testY -= cellHeight; break;
                    case Direction::DOWN:  testY += cellHeight; break;
                    case Direction::NONE:  break;
                }

                float distance = std::abs(testX - targetX) + std::abs(testY - targetY);

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

        // RED GHOST AI: 50/50 locked random
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

        // ORANGE GHOST AI: Direct chase (minimize distance to PacMan)
        if (type == GhostType::ORANGE) {
            Direction bestDirection = Direction::NONE;
            float minDistance = 999999.0f;

            for (Direction dir : validOptions) {
                float testX = x;
                float testY = y;

                switch (dir) {
                    case Direction::LEFT:  testX -= cellWidth; break;
                    case Direction::RIGHT: testX += cellWidth; break;
                    case Direction::UP:    testY -= cellHeight; break;
                    case Direction::DOWN:  testY += cellHeight; break;
                    case Direction::NONE:  break;
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

        // PINK GHOST AI: Predictive chase (4 tiles ahead of PacMan)
        if (type == GhostType::PINK) {
            // Calculate target position 4 tiles ahead of PacMan's direction
            float predictedX = targetX;
            float predictedY = targetY;

            // Predict 4 tiles ahead based on PacMan's facing direction
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
                    // PacMan not moving - target current position
                    break;
            }

            // Minimize distance to predicted position
            Direction bestDirection = Direction::NONE;
            float minDistance = 999999.0f;

            for (Direction dir : validOptions) {
                float testX = x;
                float testY = y;

                switch (dir) {
                    case Direction::LEFT:  testX -= cellWidth; break;
                    case Direction::RIGHT: testX += cellWidth; break;
                    case Direction::UP:    testY -= cellHeight; break;
                    case Direction::DOWN:  testY += cellHeight; break;
                    case Direction::NONE:  break;
                }

                // Manhattan distance to PREDICTED position
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

        // BLUE GHOST AI: Same as PINK (predictive chase)
        if (type == GhostType::BLUE) {
            // Calculate target position 4 tiles ahead of PacMan's direction
            float predictedX = targetX;
            float predictedY = targetY;

            // Predict 4 tiles ahead based on PacMan's facing direction
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
                    // PacMan not moving - target current position
                    break;
            }

            // Minimize distance to predicted position
            Direction bestDirection = Direction::NONE;
            float minDistance = 999999.0f;

            for (Direction dir : validOptions) {
                float testX = x;
                float testY = y;

                switch (dir) {
                    case Direction::LEFT:  testX -= cellWidth; break;
                    case Direction::RIGHT: testX += cellWidth; break;
                    case Direction::UP:    testY -= cellHeight; break;
                    case Direction::DOWN:  testY += cellHeight; break;
                    case Direction::NONE:  break;
                }

                // Manhattan distance to PREDICTED position
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
        setPosition(eatenRespawnX, eatenRespawnY);

        state = GhostState::RESPAWNING;
        speed = targetSpeed;
        hasExitedSpawn = false;

        respawnFlickerTimer = 0.0f;
        respawnFlickerCount = 0;

        std::cout << "Ghost eaten - respawning in spawn" << std::endl;
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

        std::cout << "Ghost type " << (int)type << " starting exit sequence" << std::endl;
    }

    void GhostModel::resetToSpawn(float delay) {
        state = GhostState::SPAWNING;
        spawnTimer = 0.0f;
        spawnDelay = delay;
        speed = targetSpeed;
        currentDirection = Direction::NONE;
        exitStepCounter = 0;
        hasExitedSpawn = false;

        std::cout << "Ghost type " << (int)type << " reset to SPAWNING with delay " << delay << std::endl;
    }

    void GhostModel::setSpeed(float newSpeed) {
        speed = newSpeed;
        targetSpeed = newSpeed;
    }
}
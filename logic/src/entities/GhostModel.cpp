#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <cmath>
#include <vector>  // ← ADD
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
              currentDirection(Direction::NONE),
              cellWidth(0.0f),
              cellHeight(0.0f),
              hasExitedSpawn(false) {
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

    // ← ADD: Check if ghost needs to make decision
    bool GhostModel::needsDirectionDecision(const std::vector<Direction>& viableDirections) const {
        // Ghost needs decision if:
        // 1. Has 2+ viable options (excluding backwards)
        // 2. Current direction is blocked

        Direction reverse = getReverseDirection(currentDirection);

        // Count options excluding backwards
        int optionCount = 0;
        bool currentStillViable = false;

        for (Direction dir : viableDirections) {
            if (dir == reverse) continue;  // Skip backwards
            optionCount++;
            if (dir == currentDirection) {
                currentStillViable = true;
            }
        }

        // Need decision if: 2+ options OR current blocked
        return (optionCount >= 2) || (!currentStillViable && optionCount > 0);
    }

    // ← ADD: Make direction decision based on AI type
    void GhostModel::makeDirectionDecision(const std::vector<Direction>& viableDirections) {
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
            // No options - shouldn't happen but safety
            currentDirection = Direction::NONE;
            return;
        }

        // ← ADD: FEAR MODE - Flee behavior (maximize distance)
        // Note: Requires PacMan position - will be passed in next step
        // For now, just pick random direction (temporary)
        if (state == GhostState::FEAR) {
            int randomIndex = Random::getInstance().getInt(0, static_cast<int>(validOptions.size()) - 1);
            currentDirection = validOptions[randomIndex];
            return;
        }

        // RED GHOST AI: 50/50 locked random
        if (type == GhostType::RED) {
            float roll = Random::getInstance().getFloat(0.0f, 1.0f);

            if (roll < 0.5f) {
                // Try to keep current direction
                auto it = std::find(validOptions.begin(), validOptions.end(), currentDirection);
                if (it != validOptions.end()) {
                    // Current direction still viable - keep it
                    return;  // Don't change currentDirection
                }
            }

            // Pick random from valid options
            int randomIndex = Random::getInstance().getInt(0, static_cast<int>(validOptions.size()) - 1);
            currentDirection = validOptions[randomIndex];
        }

        // TODO: Other ghost types (PINK, BLUE, ORANGE) later
    }

    void GhostModel::enterFearMode() {
        if (state == GhostState::CHASING) {
            state = GhostState::FEAR;
            speed = 0.25f;  // Slower speed (half of normal 0.5f)

            // Reverse direction when entering fear
            currentDirection = getReverseDirection(currentDirection);
        }
    }

    void GhostModel::exitFearMode() {
        if (state == GhostState::FEAR || state == GhostState::EATEN) {  // ← ADD EATEN check
            state = GhostState::CHASING;
            speed = 0.5f;  // Normal speed
        }
    }

    void GhostModel::getEaten() {
        state = GhostState::EATEN;
        speed = 1.0f;  // Faster return to spawn

        std::cout << "Ghost entered EATEN state" << std::endl;
    }
}
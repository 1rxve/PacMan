// logic/src/world/World.cpp
#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <vector>
#include "logic/entities/DoorModel.h"

namespace logic {
    World::~World() {
        scoreSubject.detach(&score);

        // CORRECT ORDER:
        // 1. Clear raw pointer caches
        pacman = nullptr;
        walls.clear();
        coins.clear();
        ghosts.clear();
        doors.clear();

        // 2. Destroy observers (views) BEFORE subjects (entities)
        views.clear();

        // 3. Finally destroy entities
        entities.clear();
    }

    void World::update(float deltaTime) {
        score.update(deltaTime);

        for (const auto &entity: entities) {
            if (entity->isPacMan()) {
                PacManModel* pm = static_cast<PacManModel*>(entity.get());

                // Buffered input handling
                Direction nextDir = pm->getNextDirection();
                if (nextDir != Direction::NONE && isDirectionValid(nextDir)) {
                    pm->applyNextDirection();
                }

                // Predictive collision
                float oldX = pm->getX();
                float oldY = pm->getY();

                pm->update(deltaTime);

                float newX = pm->getX();
                float newY = pm->getY();

                // Tunnel wraparound
                const float WORLD_LEFT = -1.0f;
                const float WORLD_RIGHT = 1.0f;
                const float TUNNEL_THRESHOLD = 0.02f;

                if (newX < WORLD_LEFT - TUNNEL_THRESHOLD) {
                    pm->setPosition(WORLD_RIGHT - TUNNEL_THRESHOLD, newY);
                }
                else if (newX > WORLD_RIGHT + TUNNEL_THRESHOLD) {
                    pm->setPosition(WORLD_LEFT + TUNNEL_THRESHOLD, newY);
                }

                // Check BOTH wall AND door collision together
                bool collided = false;

                for (WallModel* wall : walls) {
                    if (pm->intersects(*wall)) {
                        collided = true;
                        break;
                    }
                }

                // NEW: Door collision for PacMan (always blocked)
                if (!collided) {
                    for (DoorModel* door : doors) {
                        if (pm->intersects(*door)) {
                            collided = true;
                            break;
                        }
                    }
                }

                if (collided) {
                    pm->setPosition(oldX, oldY);
                    pm->stopMovement();
                }

                // Coin collision (unchanged)
                for (CoinModel* coin : coins) {
                    if (!coin->isCollected() && pm->intersects(*coin)) {
                        coin->collect();
                        coinsCollected++;

                        score.setEvent(ScoreEvent::COIN_COLLECTED);
                        scoreSubject.notify();
                    }
                }
            }
            else if (entity->isGhost()) {
                GhostModel* ghost = static_cast<GhostModel*>(entity.get());

                // If ghost just entered CHASING and has no direction, pick one
                if (ghost->getState() == GhostState::CHASING &&
                    ghost->getCurrentDirection() == Direction::NONE) {

                    // RED ghost always starts going LEFT
                    if (ghost->getType() == GhostType::RED) {
                        ghost->setDirection(Direction::LEFT);
                    } else {
                        Direction viableDir = getViableDirectionForGhost(ghost);
                        ghost->setDirection(viableDir);
                    }
                }

                // Check if ghost needs direction decision
                if (ghost->getState() == GhostState::CHASING &&
                    ghost->getCurrentDirection() != Direction::NONE) {

                    std::vector<Direction> viableDirections = getViableDirectionsForGhost(ghost);

                    if (ghost->needsDirectionDecision(viableDirections)) {
                        ghost->makeDirectionDecision(viableDirections);
                    }
                }

                // Store old position
                float oldX = ghost->getX();
                float oldY = ghost->getY();

                // Update ghost (will move)
                ghost->update(deltaTime);

                // Check wall collision
                bool wallCollision = false;
                for (WallModel* wall : walls) {
                    if (ghost->intersects(*wall)) {
                        wallCollision = true;
                        break;
                    }
                }

                // Check door collision
                bool doorCollision = false;
                for (DoorModel* door : doors) {
                    if (ghost->intersects(*door)) {
                        if (ghost->hasExited()) {
                            // Ghost already outside - door is a WALL
                            doorCollision = true;
                            break;
                        } else {
                            // Ghost still inside spawn - door is PASSABLE
                            // Mark as exited so next time it's a wall
                            ghost->markAsExited();
                            // No collision this frame (ghost passes through)
                        }
                    }
                }

                // Handle collision
                if (wallCollision || doorCollision) {
                    ghost->setPosition(oldX, oldY);
                    ghost->stopMovement();
                    Direction viableDir = getViableDirectionForGhost(ghost);
                    ghost->setDirection(viableDir);
                }
            }
            else {
                entity->update(deltaTime);
            }
        }
    }

    void World::setFactory(AbstractFactory* factory) {
        this->factory = factory;
    }

    void World::addEntity(std::unique_ptr<EntityModel> entity) {
        entities.push_back(std::move(entity));
    }

    void World::loadMap(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open map file: " << filename << std::endl;
            return;
        }

        clearWorld();

        std::vector<std::string> mapLines;
        std::string line;
        while (std::getline(file, line)) {
            mapLines.push_back(line);
        }
        file.close();

        if (mapLines.empty()) {
            return;
        }

        int height = static_cast<int>(mapLines.size());
        int width = static_cast<int>(mapLines[0].length());

        float cellWidth = 2.0f / width;
        float cellHeight = 2.0f / height;

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                char symbol = mapLines[row][col];

                float normalizedX = -1.0f + cellWidth / 2.0f + col * cellWidth;
                float normalizedY = -1.0f + cellHeight / 2.0f + row * cellHeight;

                switch (symbol) {
                    case '#': {
                        if (factory) {
                            auto result = factory->createWall(normalizedX, normalizedY, cellWidth, cellHeight);
                            if (result.model->isWall()) {
                                WallModel* wallPtr = static_cast<WallModel*>(result.model.get());
                                walls.push_back(wallPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'D': {
                        if (factory) {
                            auto result = factory->createDoor(normalizedX, normalizedY, cellWidth, cellHeight);
                            if (result.model->isDoor()) {
                                DoorModel* doorPtr = static_cast<DoorModel*>(result.model.get());
                                doors.push_back(doorPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'C': {
                        if (factory) {
                            auto result = factory->createPacMan(normalizedX, normalizedY,
                                                                cellWidth * 0.9f, cellHeight * 0.9f, 0.5f);
                            if (result.model->isPacMan()) {
                                pacman = static_cast<PacManModel*>(result.model.get());
                                pacman->setCellDimensions(cellWidth, cellHeight);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'R': {  // RED ghost
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::RED, 0.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghostPtr->markAsExited();  // ← ADD: RED spawns outside, already exited
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'P': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::PINK, 0.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'B': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::BLUE, 5.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'O': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::ORANGE, 10.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case '.': {
                        if (factory) {
                            auto result = factory->createCoin(normalizedX, normalizedY,
                                                              cellWidth * 0.15f, cellHeight * 0.15f);
                            if (result.model->isCoin()) {
                                CoinModel* coinPtr = static_cast<CoinModel*>(result.model.get());
                                coins.push_back(coinPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case '*':  // ← VOEG TOE: sterretjes = leeg
                        break;

                    default:
                        break;
                }
            }
        }
    }

    PacManModel* World::getPacMan() {
        for (auto& entity : entities) {
            if (entity->isPacMan()) {
                return static_cast<PacManModel*>(entity.get());
            }
        }
        return nullptr;
    }

    std::pair<int, int> World::getMapDimensions(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return {0, 0};
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        if (lines.empty()) {
            return {0, 0};
        }

        int height = static_cast<int>(lines.size());
        int width = static_cast<int>(lines[0].length());

        return {width, height};
    }

    bool World::isDirectionValid(Direction direction) const {
        if (!pacman) return false;
        if (direction == Direction::NONE) return true;

        const float TEST_DISTANCE = 0.1f;

        float testX = pacman->getX();
        float testY = pacman->getY();

        switch (direction) {
            case Direction::LEFT:
                testX -= TEST_DISTANCE;
                break;
            case Direction::RIGHT:
                testX += TEST_DISTANCE;
                break;
            case Direction::UP:
                testY -= TEST_DISTANCE;
                break;
            case Direction::DOWN:
                testY += TEST_DISTANCE;
                break;
            case Direction::NONE:
                break;
        }

        float width = pacman->getWidth();
        float height = pacman->getHeight();

        float left = testX - width / 2.0f;
        float right = testX + width / 2.0f;
        float top = testY - height / 2.0f;
        float bottom = testY + height / 2.0f;

        for (const WallModel* wall : walls) {
            float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
            float wallRight = wall->getX() + wall->getWidth() / 2.0f;
            float wallTop = wall->getY() - wall->getHeight() / 2.0f;
            float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

            if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                return false;
            }
        }

        for (const DoorModel* door : doors) {
            float doorLeft = door->getX() - door->getWidth() / 2.0f;
            float doorRight = door->getX() + door->getWidth() / 2.0f;
            float doorTop = door->getY() - door->getHeight() / 2.0f;
            float doorBottom = door->getY() + door->getHeight() / 2.0f;

            if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
                return false;  // Door blocks PacMan
            }
        }

        return true;
    }

    Direction World::getViableDirectionForGhost(GhostModel* ghost) const {
        if (!ghost) return Direction::NONE;

        const float TEST_DISTANCE = 0.1f;

        std::vector<Direction> viableDirections;

        // Test all 4 directions
        for (Direction dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
            float testX = ghost->getX();
            float testY = ghost->getY();

            switch (dir) {
                case Direction::LEFT:
                    testX -= TEST_DISTANCE;
                    break;
                case Direction::RIGHT:
                    testX += TEST_DISTANCE;
                    break;
                case Direction::UP:
                    testY -= TEST_DISTANCE;
                    break;
                case Direction::DOWN:
                    testY += TEST_DISTANCE;
                    break;
                case Direction::NONE:
                    break;
            }

            float width = ghost->getWidth();
            float height = ghost->getHeight();

            float left = testX - width / 2.0f;
            float right = testX + width / 2.0f;
            float top = testY - height / 2.0f;
            float bottom = testY + height / 2.0f;

            bool hitObstacle = false;

            // Check walls
            for (const WallModel* wall : walls) {
                float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
                float wallRight = wall->getX() + wall->getWidth() / 2.0f;
                float wallTop = wall->getY() - wall->getHeight() / 2.0f;
                float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

                if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                    hitObstacle = true;
                    break;
                }
            }

            // Check doors
            if (!hitObstacle) {
                for (const DoorModel* door : doors) {
                    float doorLeft = door->getX() - door->getWidth() / 2.0f;
                    float doorRight = door->getX() + door->getWidth() / 2.0f;
                    float doorTop = door->getY() - door->getHeight() / 2.0f;
                    float doorBottom = door->getY() + door->getHeight() / 2.0f;

                    if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
                        // Would intersect door
                        if (ghost->hasExited()) {  // ← CHANGE: use flag instead of canPass()
                            hitObstacle = true;
                            break;
                        }
                    }
                }
            }

            if (!hitObstacle) {
                viableDirections.push_back(dir);
            }
        }

        // If no viable directions, return NONE
        if (viableDirections.empty()) {
            return Direction::NONE;
        }

        // Pick random viable direction
        int randomIndex = Random::getInstance().getInt(0, static_cast<int>(viableDirections.size()) - 1);
        return viableDirections[randomIndex];
    }

    std::vector<Direction> World::getViableDirectionsForGhost(GhostModel* ghost) const {
        if (!ghost) return {};

        const float TEST_DISTANCE = 0.1f;
        std::vector<Direction> viableDirections;

        // Test all 4 directions
        for (Direction dir : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
            float testX = ghost->getX();
            float testY = ghost->getY();

            switch (dir) {
                case Direction::LEFT:
                    testX -= TEST_DISTANCE;
                    break;
                case Direction::RIGHT:
                    testX += TEST_DISTANCE;
                    break;
                case Direction::UP:
                    testY -= TEST_DISTANCE;
                    break;
                case Direction::DOWN:
                    testY += TEST_DISTANCE;
                    break;
                case Direction::NONE:
                    break;
            }

            float width = ghost->getWidth();
            float height = ghost->getHeight();

            float left = testX - width / 2.0f;
            float right = testX + width / 2.0f;
            float top = testY - height / 2.0f;
            float bottom = testY + height / 2.0f;

            bool hitObstacle = false;

            // Check walls
            for (const WallModel* wall : walls) {
                float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
                float wallRight = wall->getX() + wall->getWidth() / 2.0f;
                float wallTop = wall->getY() - wall->getHeight() / 2.0f;
                float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

                if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                    hitObstacle = true;
                    break;
                }
            }

            // Check doors
            if (!hitObstacle) {
                for (const DoorModel* door : doors) {
                    float doorLeft = door->getX() - door->getWidth() / 2.0f;
                    float doorRight = door->getX() + door->getWidth() / 2.0f;
                    float doorTop = door->getY() - door->getHeight() / 2.0f;
                    float doorBottom = door->getY() + door->getHeight() / 2.0f;

                    if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
                        // Would intersect door
                        if (ghost->hasExited()) {  // ← CHANGE: use flag instead of canPass()
                            hitObstacle = true;
                            break;
                        }
                    }
                }
            }

            if (!hitObstacle) {
                viableDirections.push_back(dir);
            }
        }

        return viableDirections;
    }

    void World::clearWorld() {
        std::cout << "=== CLEARING WORLD ===" << std::endl;
        std::cout << "BEFORE clear - doors: " << doors.size() << ", entities: " << entities.size() << std::endl;

        walls.clear();
        ghosts.clear();
        doors.clear();
        coins.clear();
        pacman = nullptr;

        views.clear();
        entities.clear();

        coinsCollected = 0;

        std::cout << "AFTER clear - doors: " << doors.size() << ", entities: " << entities.size() << std::endl;
        std::cout << "===================\n" << std::endl;
    }
}

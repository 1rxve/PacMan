// logic/src/world/World.cpp
#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/utils/Random.h"
#include <vector>

namespace logic {
    World::~World() {
        scoreSubject.detach(&score);
        views.clear();
        entities.clear();
        pacman = nullptr;
        walls.clear();
        coins.clear();
        ghosts.clear();
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

                // Wall collision
                bool collided = false;
                for (WallModel* wall : walls) {
                    if (pm->intersects(*wall)) {
                        collided = true;
                        break;
                    }
                }

                if (collided) {
                    pm->setPosition(oldX, oldY);
                    pm->stopMovement();
                }

                // Coin collision
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
                    Direction viableDir = getViableDirectionForGhost(ghost);
                    ghost->setDirection(viableDir);
                }

                // Store old position
                float oldX = ghost->getX();
                float oldY = ghost->getY();

                // Update ghost (will move)
                ghost->update(deltaTime);

                // Check wall collision
                bool collided = false;
                for (WallModel* wall : walls) {
                    if (ghost->intersects(*wall)) {
                        collided = true;
                        break;
                    }
                }

                if (collided) {
                    // Rollback to old position
                    ghost->setPosition(oldX, oldY);
                    ghost->stopMovement();  // ← ADD - Stop trying to move into wall
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
            return;
        }

        std::vector<std::string> mapLines;
        std::string line;
        while (std::getline(file, line)) {
            mapLines.push_back(line);
        }
        file.close();

        if (mapLines.empty()) {
            return;
        }

        int height = mapLines.size();
        int width = mapLines[0].length();

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
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);  // ← ADD
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'P': {  // PINK ghost (in spawn box)
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::PINK, 0.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);  // ← ADD THIS LINE
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'B': {  // BLUE ghost (in spawn box)
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::BLUE, 5.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);  // ← ADD THIS LINE
                                ghosts.push_back(ghostPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'O': {  // ORANGE ghost (in spawn box)
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.9f, cellHeight * 0.9f,
                                                               GhostType::ORANGE, 10.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);  // ← ADD THIS LINE
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

                    case ' ':
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

            bool hitWall = false;
            for (const WallModel* wall : walls) {
                float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
                float wallRight = wall->getX() + wall->getWidth() / 2.0f;
                float wallTop = wall->getY() - wall->getHeight() / 2.0f;
                float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

                if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                    hitWall = true;
                    break;
                }
            }

            if (!hitWall) {
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
}

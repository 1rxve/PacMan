#include "logic/world/World.h"
#include "logic/entities/CoinModel.h"
#include "logic/entities/DoorModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/WallModel.h"
#include "logic/utils/Random.h"
#include <vector>

namespace logic {
World::World()
    : factory(nullptr), pacman(nullptr), coinsCollected(0), pacmanSpawnX(0.0f), pacmanSpawnY(0.0f),
      hasJustRespawned(false), fearModeActive(false), fearModeTimer(0.0f), currentLevel(1), baseGhostSpeed(0.5f),
      baseFearDuration(7.0f) {}

World::~World() {
    scoreSubject.detach(&score);

    pacman = nullptr;
    walls.clear();
    coins.clear();
    ghosts.clear();
    doors.clear();
    noEntries.clear();

    wallViews.clear();
    coinViews.clear();
    fruitViews.clear();
    doorViews.clear();
    ghostViews.clear();
    pacmanView.reset();

    entities.clear();
}

void World::update(float deltaTime) {
    score.update(deltaTime);

    if (fearModeActive) {
        fearModeTimer -= deltaTime;

        for (auto& ghost : ghosts) {
            ghost->setFearTimer(fearModeTimer);
        }

        if (fearModeTimer <= 0.0f) {
            fearModeActive = false;

            for (auto& ghost : ghosts) {
                ghost->exitFearMode();
            }
        }
    }

    // Death animation pauses all gameplay
    if (pacman && pacman->getIsDying()) {
        pacman->updateDeath(deltaTime);

        if (!pacman->getIsDying()) {
            resetAfterDeath();
        }

        renderInOrder();
        return;
    }

    for (const auto& entity : entities) {
        if (entity->isPacMan()) {
            auto pm = std::dynamic_pointer_cast<PacManModel>(entity);

            // Apply buffered input when valid (responsive controls)
            Direction nextDir = pm->getNextDirection();
            if (nextDir != Direction::NONE && isDirectionValid(nextDir)) {
                pm->applyNextDirection();
            }

            // Predictive collision: test new position before applying
            float oldX = pm->getX();
            float oldY = pm->getY();

            pm->update(deltaTime);

            float newX = pm->getX();
            float newY = pm->getY();

            // Tunnel wraparound at world edges
            const float WORLD_LEFT = -1.0f;
            const float WORLD_RIGHT = 1.0f;
            const float TUNNEL_THRESHOLD = 0.02f; // Prevents instant re-wrap

            if (newX < WORLD_LEFT - TUNNEL_THRESHOLD) {
                pm->setPosition(WORLD_RIGHT - TUNNEL_THRESHOLD, newY);
            } else if (newX > WORLD_RIGHT + TUNNEL_THRESHOLD) {
                pm->setPosition(WORLD_LEFT + TUNNEL_THRESHOLD, newY);
            }

            bool collided = false;

            for (auto& wall : walls) {
                if (pm->intersects(*wall)) {
                    collided = true;
                    break;
                }
            }

            if (!collided) {
                for (auto& door : doors) {
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

            for (auto& ghost : ghosts) {
                if (ghost->getState() == GhostState::CHASING && pm->intersects(*ghost)) {
                    handlePacManDeath();
                    return;
                }

                if (ghost->getState() == GhostState::FEAR && pm->intersects(*ghost)) {
                    ghost->getEaten();

                    score.setEvent(ScoreEvent::GHOST_EATEN);
                    scoreSubject.notify();
                }
            }

            for (auto& coin : coins) {
                if (!coin->isCollected() && pm->intersects(*coin)) {
                    coin->collect();
                    coinsCollected++;

                    score.setEvent(ScoreEvent::COIN_COLLECTED);
                    scoreSubject.notify();
                }
            }

            for (auto& fruit : fruits) {
                if (!fruit->isCollected() && pm->intersects(*fruit)) {
                    fruit->collect();

                    score.setEvent(ScoreEvent::FRUIT_EATEN);
                    scoreSubject.notify();

                    activateFearMode();
                }
            }
        } else if (entity->isGhost()) {
            auto ghost = std::dynamic_pointer_cast<GhostModel>(entity);

            if ((ghost->getState() == GhostState::CHASING || ghost->getState() == GhostState::FEAR) &&
                ghost->getCurrentDirection() == Direction::NONE) {

                if (ghost->getType() == GhostType::RED) {
                    ghost->setDirection(Direction::LEFT);
                } else {
                    Direction viableDir = getViableDirectionForGhost(ghost);
                    ghost->setDirection(viableDir);
                }
            }

            if ((ghost->getState() == GhostState::CHASING || ghost->getState() == GhostState::FEAR) &&
                ghost->getCurrentDirection() != Direction::NONE) {

                std::vector<Direction> viableDirections = getViableDirectionsForGhost(ghost);

                if (ghost->needsDirectionDecision(viableDirections)) {
                    float pacmanX = pacman ? pacman->getX() : 0.0f;
                    float pacmanY = pacman ? pacman->getY() : 0.0f;
                    Direction pacmanDir = pacman ? pacman->getCurrentDirection() : Direction::NONE;
                    ghost->makeDirectionDecision(viableDirections, pacmanX, pacmanY, pacmanDir);
                }
            }

            float oldX = ghost->getX();
            float oldY = ghost->getY();

            ghost->update(deltaTime);

            if (ghost->getState() == GhostState::EATEN) {
                continue;
            }

            bool wallCollision = false;
            for (auto& wall : walls) {
                if (ghost->intersects(*wall)) {
                    wallCollision = true;
                    break;
                }
            }

            bool doorCollision = false;
            for (auto& door : doors) {
                if (ghost->intersects(*door)) {
                    if (ghost->getState() == GhostState::EATEN) {
                        continue;
                    }

                    if (ghost->getState() == GhostState::EXITING_SPAWN) {
                        ghost->markAsExited();
                    } else if (ghost->hasExited()) {
                        doorCollision = true;
                        break;
                    } else {
                        ghost->markAsExited();
                    }
                }
            }

            bool noEntryCollision = false;
            for (auto& noEntry : noEntries) {
                if (ghost->intersects(*noEntry) && noEntry->blocksGhostType(ghost->getType())) {
                    noEntryCollision = true;
                    break;
                }
            }

            if (wallCollision || doorCollision || noEntryCollision) {
                ghost->setPosition(oldX, oldY);

                // Hardcoded spawn exit collision responses
                if (ghost->getState() == GhostState::EXITING_SPAWN) {
                    if (ghost->getType() == GhostType::ORANGE && ghost->getCurrentDirection() == Direction::LEFT) {
                        ghost->setDirection(Direction::UP);
                    } else if (ghost->getType() == GhostType::BLUE && ghost->getCurrentDirection() == Direction::RIGHT) {
                        ghost->setDirection(Direction::UP);
                    } else if (ghost->getCurrentDirection() == Direction::UP) {
                        if (ghost->getType() == GhostType::BLUE) {
                            ghost->setDirection(Direction::RIGHT);
                        } else {
                            ghost->setDirection(Direction::LEFT);
                        }
                    } else {
                        ghost->stopMovement();
                    }
                } else {
                    ghost->stopMovement();
                    Direction viableDir = getViableDirectionForGhost(ghost);
                    ghost->setDirection(viableDir);
                }
            }
        } else {
            entity->update(deltaTime);
        }
    }
    renderInOrder();
}

void World::setFactory(AbstractFactory* factory) { this->factory = factory; }

void World::addEntity(std::unique_ptr<EntityModel> entity) { entities.push_back(std::move(entity)); }

void World::loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR: Cannot open map file: " + filename);
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

    // Convert world space [-1, 1] to grid cells
    float cellWidth = 2.0f / width;
    float cellHeight = 2.0f / height;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            char symbol = mapLines[row][col];

            // Center of cell in normalized coordinates
            float normalizedX = -1.0f + cellWidth / 2.0f + col * cellWidth;
            float normalizedY = -1.0f + cellHeight / 2.0f + row * cellHeight;

            // ASCII map symbols:
            // # = wall, C = PacMan, R/P/B/O = ghosts, . = coin, F = fruit
            // D = door, N = NoEntry barrier, * = empty space
            switch (symbol) {
            case '#': {
                if (factory) {
                    auto result = factory->createWall(normalizedX, normalizedY, cellWidth, cellHeight);

                    if (auto wallPtr = std::dynamic_pointer_cast<WallModel>(result.model)) {
                        walls.push_back(wallPtr);
                    }

                    entities.push_back(result.model);
                    wallViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'D': {
                if (factory) {
                    auto result = factory->createDoor(normalizedX, normalizedY, cellWidth, cellHeight);

                    if (auto doorPtr = std::dynamic_pointer_cast<DoorModel>(result.model)) {
                        doors.push_back(doorPtr);
                    }

                    entities.push_back(result.model);
                    doorViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'C': {
                if (factory) {
                    auto result = factory->createPacMan(normalizedX, normalizedY, cellWidth * 0.9f, cellHeight * 0.9f, 0.5f);

                    pacman = std::dynamic_pointer_cast<PacManModel>(result.model);
                    if (pacman) {
                        pacman->setCellDimensions(cellWidth, cellHeight);
                    }

                    pacmanSpawnX = normalizedX;
                    pacmanSpawnY = normalizedY;

                    entities.push_back(result.model);
                    pacmanView = std::move(result.view);
                }
                break;
            }

            case 'R': {
                if (factory) {
                    auto result = factory->createGhost(normalizedX, normalizedY, cellWidth * 0.85f, cellHeight * 0.85f,
                                                       GhostType::RED, 0.0f);

                    if (auto ghostPtr = std::dynamic_pointer_cast<GhostModel>(result.model)) {
                        ghostPtr->setCellDimensions(cellWidth, cellHeight);
                        ghostPtr->markAsExited(); // RED spawns outside
                        ghosts.push_back(ghostPtr);

                        ghostSpawnPositions.push_back({normalizedX, normalizedY});

                        // Hardcoded: center of spawn room (column 9, row 9)
                        float eatenSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                        float eatenSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                        ghostPtr->setEatenRespawnPosition(eatenSpawnX, eatenSpawnY);
                    }

                    entities.push_back(result.model);
                    ghostViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'P': {
                if (factory) {
                    auto result = factory->createGhost(normalizedX, normalizedY, cellWidth * 0.85f, cellHeight * 0.85f,
                                                       GhostType::PINK, 0.0f);

                    if (auto ghostPtr = std::dynamic_pointer_cast<GhostModel>(result.model)) {
                        ghostPtr->setCellDimensions(cellWidth, cellHeight);
                        ghosts.push_back(ghostPtr);

                        ghostSpawnPositions.push_back({normalizedX, normalizedY});

                        float centerSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                        float centerSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                        ghostPtr->setEatenRespawnPosition(centerSpawnX, centerSpawnY);
                    }

                    entities.push_back(result.model);
                    ghostViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'B': {
                if (factory) {
                    auto result = factory->createGhost(normalizedX, normalizedY, cellWidth * 0.85f, cellHeight * 0.85f,
                                                       GhostType::BLUE, 5.0f); // 5s spawn delay

                    if (auto ghostPtr = std::dynamic_pointer_cast<GhostModel>(result.model)) {
                        ghostPtr->setCellDimensions(cellWidth, cellHeight);
                        ghosts.push_back(ghostPtr);
                        ghostSpawnPositions.push_back({normalizedX, normalizedY});

                        // NoEntry barrier blocks ORANGE from BLUE's spawn position
                        auto barrierResult = factory->createNoEntry(normalizedX, normalizedY, cellWidth, cellHeight);
                        if (auto barrier = std::dynamic_pointer_cast<NoEntryModel>(barrierResult.model)) {
                            barrier->clearBlockedGhostTypes();
                            barrier->addBlockedGhostType(GhostType::ORANGE);
                            noEntries.push_back(barrier);
                        }
                        entities.push_back(barrierResult.model);
                    }

                    entities.push_back(result.model);
                    ghostViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'O': {
                if (factory) {
                    auto result = factory->createGhost(normalizedX, normalizedY, cellWidth * 0.85f, cellHeight * 0.85f,
                                                       GhostType::ORANGE, 10.0f); // 10s spawn delay

                    if (auto ghostPtr = std::dynamic_pointer_cast<GhostModel>(result.model)) {
                        ghostPtr->setCellDimensions(cellWidth, cellHeight);
                        ghosts.push_back(ghostPtr);

                        ghostSpawnPositions.push_back({normalizedX, normalizedY});

                        float centerSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                        float centerSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                        ghostPtr->setEatenRespawnPosition(centerSpawnX, centerSpawnY);

                        // NoEntry barrier blocks BLUE from ORANGE's spawn position
                        auto barrierResult = factory->createNoEntry(normalizedX, normalizedY, cellWidth, cellHeight);
                        if (auto barrier = std::dynamic_pointer_cast<NoEntryModel>(barrierResult.model)) {
                            barrier->clearBlockedGhostTypes();
                            barrier->addBlockedGhostType(GhostType::BLUE);
                            noEntries.push_back(barrier);
                        }
                        entities.push_back(barrierResult.model);
                    }

                    entities.push_back(result.model);
                    ghostViews.push_back(std::move(result.view));
                }
                break;
            }

            case '.': {
                if (factory) {
                    auto result = factory->createCoin(normalizedX, normalizedY, cellWidth * 0.15f, cellHeight * 0.15f);

                    if (auto coinPtr = std::dynamic_pointer_cast<CoinModel>(result.model)) {
                        coins.push_back(coinPtr);
                    }

                    entities.push_back(result.model);
                    coinViews.push_back(std::move(result.view));
                }
                break;
            }

            case 'N': {
                if (factory) {
                    auto result = factory->createNoEntry(normalizedX, normalizedY, cellWidth, cellHeight);

                    if (auto noEntryPtr = std::dynamic_pointer_cast<NoEntryModel>(result.model)) {
                        noEntries.push_back(noEntryPtr);
                    }

                    entities.push_back(result.model);
                }
                break;
            }

            case 'F': {
                if (factory) {
                    auto result = factory->createFruit(normalizedX, normalizedY, cellWidth * 0.3f, cellHeight * 0.3f);

                    if (auto fruitPtr = std::dynamic_pointer_cast<FruitModel>(result.model)) {
                        fruits.push_back(fruitPtr);
                    }

                    entities.push_back(result.model);
                    fruitViews.push_back(std::move(result.view));
                }
                break;
            }
            case '*':
                break;

            default:
                break;
            }
        }
    }
}

std::shared_ptr<PacManModel> World::getPacMan() {
    for (auto& entity : entities) {
        if (entity->isPacMan()) {
            return std::dynamic_pointer_cast<PacManModel>(entity);
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
    if (!pacman)
        return false;
    if (direction == Direction::NONE)
        return true;

    // Predictive collision: test position 0.1 units ahead
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

    for (auto& wall : walls) {
        float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
        float wallRight = wall->getX() + wall->getWidth() / 2.0f;
        float wallTop = wall->getY() - wall->getHeight() / 2.0f;
        float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

        if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
            return false;
        }
    }

    for (auto& door : doors) {
        float doorLeft = door->getX() - door->getWidth() / 2.0f;
        float doorRight = door->getX() + door->getWidth() / 2.0f;
        float doorTop = door->getY() - door->getHeight() / 2.0f;
        float doorBottom = door->getY() + door->getHeight() / 2.0f;

        if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
            return false;
        }
    }

    return true;
}

Direction World::getViableDirectionForGhost(std::shared_ptr<GhostModel> ghost) const {
    if (!ghost)
        return Direction::NONE;

    const float TEST_DISTANCE = 0.1f;

    std::vector<Direction> viableDirections;

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

        for (auto& wall : walls) {
            float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
            float wallRight = wall->getX() + wall->getWidth() / 2.0f;
            float wallTop = wall->getY() - wall->getHeight() / 2.0f;
            float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

            if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                hitObstacle = true;
                break;
            }
        }

        if (!hitObstacle) {
            for (auto& door : doors) {
                float doorLeft = door->getX() - door->getWidth() / 2.0f;
                float doorRight = door->getX() + door->getWidth() / 2.0f;
                float doorTop = door->getY() - door->getHeight() / 2.0f;
                float doorBottom = door->getY() + door->getHeight() / 2.0f;

                if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
                    if (ghost->hasExited()) {
                        hitObstacle = true;
                        break;
                    }
                }
            }
        }

        if (!hitObstacle) {
            for (auto& noEntry : noEntries) {
                float neLeft = noEntry->getX() - noEntry->getWidth() / 2.0f;
                float neRight = noEntry->getX() + noEntry->getWidth() / 2.0f;
                float neTop = noEntry->getY() - noEntry->getHeight() / 2.0f;
                float neBottom = noEntry->getY() + noEntry->getHeight() / 2.0f;

                if (!(right < neLeft || left > neRight || bottom < neTop || top > neBottom)) {
                    hitObstacle = true;
                    break;
                }
            }
        }

        if (!hitObstacle) {
            viableDirections.push_back(dir);
        }
    }

    if (viableDirections.empty()) {
        return Direction::NONE;
    }

    int randomIndex = Random::getInstance().getInt(0, static_cast<int>(viableDirections.size()) - 1);
    return viableDirections[randomIndex];
}

std::vector<Direction> World::getViableDirectionsForGhost(std::shared_ptr<GhostModel> ghost) const {
    if (!ghost)
        return {};

    const float TEST_DISTANCE = 0.1f;
    std::vector<Direction> viableDirections;

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

        for (auto& wall : walls) {
            float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
            float wallRight = wall->getX() + wall->getWidth() / 2.0f;
            float wallTop = wall->getY() - wall->getHeight() / 2.0f;
            float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

            if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                hitObstacle = true;
                break;
            }
        }

        if (!hitObstacle) {
            for (auto& door : doors) {
                float doorLeft = door->getX() - door->getWidth() / 2.0f;
                float doorRight = door->getX() + door->getWidth() / 2.0f;
                float doorTop = door->getY() - door->getHeight() / 2.0f;
                float doorBottom = door->getY() + door->getHeight() / 2.0f;

                if (!(right < doorLeft || left > doorRight || bottom < doorTop || top > doorBottom)) {
                    if (ghost->hasExited()) {
                        hitObstacle = true;
                        break;
                    }
                }
            }
        }

        if (!hitObstacle) {
            for (auto& noEntry : noEntries) {
                float neLeft = noEntry->getX() - noEntry->getWidth() / 2.0f;
                float neRight = noEntry->getX() + noEntry->getWidth() / 2.0f;
                float neTop = noEntry->getY() - noEntry->getHeight() / 2.0f;
                float neBottom = noEntry->getY() + noEntry->getHeight() / 2.0f;

                if (!(right < neLeft || left > neRight || bottom < neTop || top > neBottom)) {
                    hitObstacle = true;
                    break;
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
    walls.clear();
    ghosts.clear();
    doors.clear();
    coins.clear();
    noEntries.clear();
    fruits.clear();
    pacman = nullptr;

    wallViews.clear();
    coinViews.clear();
    fruitViews.clear();
    doorViews.clear();
    ghostViews.clear();
    pacmanView.reset();

    entities.clear();

    coinsCollected = 0;

    ghostSpawnPositions.clear();
}

void World::handlePacManDeath() {
    if (!pacman)
        return;

    score.setEvent(ScoreEvent::PACMAN_DEATH_ANIM);
    scoreSubject.notify();

    pacman->startDeath();
    pacman->loseLife();

    score.setEvent(ScoreEvent::PACMAN_DIED);
    scoreSubject.notify();
}

void World::resetAfterDeath() {
    if (!pacman)
        return;

    pacman->respawn(pacmanSpawnX, pacmanSpawnY);

    for (size_t i = 0; i < ghosts.size() && i < ghostSpawnPositions.size(); i++) {
        auto& ghost = ghosts[i];
        float spawnX = ghostSpawnPositions[i].first;
        float spawnY = ghostSpawnPositions[i].second;

        ghost->setPosition(spawnX, spawnY);
        ghost->stopMovement();

        if (ghost->getType() == GhostType::RED) {
            ghost->resetToSpawn(0.0f);
        } else {
            float delay = 0.0f;
            if (ghost->getType() == GhostType::ORANGE)
                delay = 10.0f;
            if (ghost->getType() == GhostType::BLUE)
                delay = 5.0f;
            if (ghost->getType() == GhostType::PINK)
                delay = 0.0f;

            ghost->resetToSpawn(delay);
        }
    }

    hasJustRespawned = true;
}

void World::notifyViewsOnly() {
    for (const auto& entity : entities) {
        entity->notify();
    }
}

void World::renderInOrder() {
    // Render order: doors → walls → coins → fruits → ghosts → pacman
    // Ensures proper layering (PacMan always on top)
    for (auto& view : doorViews) {
        view->onNotify();
    }
    for (auto& view : wallViews) {
        view->onNotify();
    }
    for (auto& view : coinViews) {
        view->onNotify();
    }
    for (auto& view : fruitViews) {
        view->onNotify();
    }
    for (auto& view : ghostViews) {
        view->onNotify();
    }
    if (pacmanView) {
        pacmanView->onNotify();
    }
}

void World::activateFearMode() {
    fearModeActive = true;
    fearModeTimer = baseFearDuration;

    score.setEvent(ScoreEvent::GHOST_FEAR_MODE);
    scoreSubject.notify();

    for (auto& ghost : ghosts) {
        ghost->enterFearMode();
    }
}

void World::nextLevel() {
    currentLevel++;

    for (auto& coin : coins) {
        coin->uncollect();
        coin->notify();
    }

    for (auto& fruit : fruits) {
        fruit->uncollect();
        fruit->notify();
    }

    coinsCollected = 0;

    if (pacman) {
        pacman->setPosition(pacmanSpawnX, pacmanSpawnY);
        pacman->stopMovement();
        pacman->notify();
    }

    for (size_t i = 0; i < ghosts.size() && i < ghostSpawnPositions.size(); i++) {
        auto& ghost = ghosts[i];
        float spawnX = ghostSpawnPositions[i].first;
        float spawnY = ghostSpawnPositions[i].second;

        ghost->setPosition(spawnX, spawnY);

        if (ghost->getType() == GhostType::RED) {
            ghost->resetToSpawn(0.0f);
        } else if (ghost->getType() == GhostType::PINK) {
            ghost->resetToSpawn(0.0f);
        } else if (ghost->getType() == GhostType::BLUE) {
            ghost->resetToSpawn(5.0f);
        } else if (ghost->getType() == GhostType::ORANGE) {
            ghost->resetToSpawn(10.0f);
        }

        // Level progression: +0.01 speed per level
        float newSpeed = baseGhostSpeed + (currentLevel - 1) * 0.01f;
        ghost->setSpeed(newSpeed);
    }

    // Level progression: -0.5s fear duration per level (min 3s)
    baseFearDuration = std::max(3.0f, 7.0f - (currentLevel - 1) * 0.5f);
}
} // namespace logic

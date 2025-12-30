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
    World::World()
            : factory(nullptr),
              pacman(nullptr),
              coinsCollected(0),
              pacmanSpawnX(0.0f),
              pacmanSpawnY(0.0f),
              hasJustRespawned(false),
              fearModeActive(false),     // ← ADD
              fearModeTimer(0.0f),
              currentLevel(1),              // ← ADD
              baseGhostSpeed(0.5f),         // ← ADD
              baseFearDuration(7.0f) {}



    World::~World() {
        scoreSubject.detach(&score);

        pacman = nullptr;
        walls.clear();
        coins.clear();
        ghosts.clear();
        doors.clear();
        noEntries.clear();

        // ← REPLACE
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

        // Fear mode timer
        if (fearModeActive) {
            fearModeTimer -= deltaTime;

            for (GhostModel* ghost : ghosts) {
                ghost->setFearTimer(fearModeTimer);
            }

            if (fearModeTimer <= 0.0f) {
                fearModeActive = false;

                // Exit fear mode for all ghosts
                for (GhostModel* ghost : ghosts) {
                    ghost->exitFearMode();
                }

                std::cout << "FEAR MODE ENDED!" << std::endl;
            }
        }

        // Check if PacMan is dying - pause all gameplay
        if (pacman && pacman->getIsDying()) {
            pacman->updateDeath(deltaTime);

            if (!pacman->getIsDying()) {
                resetAfterDeath();
            }

            renderInOrder();
            return;
        }

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

                bool collided = false;

                for (WallModel* wall : walls) {
                    if (pm->intersects(*wall)) {
                        collided = true;
                        break;
                    }
                }

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

                // Ghost collision check
                for (GhostModel* ghost : ghosts) {
                    if (ghost->getState() == GhostState::CHASING && pm->intersects(*ghost)) {
                        handlePacManDeath();
                        return;  // Stop update immediately
                    }

                    // Eat ghost during fear mode
                    if (ghost->getState() == GhostState::FEAR && pm->intersects(*ghost)) {
                        ghost->getEaten();  // ← Teleports + starts RESPAWNING state
                        std::cout << "GHOST EATEN! Respawning in spawn" << std::endl;
                    }
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

                // Fruit collision
                for (FruitModel* fruit : fruits) {
                    if (!fruit->isCollected() && pm->intersects(*fruit)) {
                        fruit->collect();
                        activateFearMode();
                    }
                }
            }
            else if (entity->isGhost()) {
                GhostModel* ghost = static_cast<GhostModel*>(entity.get());

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

                bool wallCollision = false;
                for (WallModel* wall : walls) {
                    if (ghost->intersects(*wall)) {
                        wallCollision = true;
                        break;
                    }
                }

                bool doorCollision = false;
                for (DoorModel* door : doors) {
                    if (ghost->intersects(*door)) {
                        // ← ADD: Allow EXITING_SPAWN ghosts through door
                        if (ghost->getState() == GhostState::EXITING_SPAWN) {
                            // Ghost exiting - don't block, mark as exited when through
                            ghost->markAsExited();
                        }
                        else if (ghost->hasExited()) {
                            doorCollision = true;
                            break;
                        } else {
                            ghost->markAsExited();
                        }
                    }
                }

                bool noEntryCollision = false;
                for (NoEntryModel* noEntry : noEntries) {
                    if (ghost->intersects(*noEntry)) {
                        // Check if this barrier blocks this ghost type
                        if (noEntry->blocksGhostType(ghost->getType())) {
                            std::cout << "Ghost type " << (int)ghost->getType()
                                      << " BLOCKED by NoEntry at ("
                                      << noEntry->getX() << ", " << noEntry->getY() << ")" << std::endl;
                            noEntryCollision = true;
                            break;
                        } else {
                            std::cout << "Ghost type " << (int)ghost->getType()
                                      << " PASSES THROUGH NoEntry at ("
                                      << noEntry->getX() << ", " << noEntry->getY() << ")" << std::endl;
                        }
                    }
                }

                if (wallCollision || doorCollision || noEntryCollision) {
                    ghost->setPosition(oldX, oldY);

                    if (ghost->getState() == GhostState::EXITING_SPAWN) {
                        // ORANGE: LEFT hit barrier → UP
                        if (ghost->getType() == GhostType::ORANGE &&
                            ghost->getCurrentDirection() == Direction::LEFT) {
                            ghost->setDirection(Direction::UP);
                            std::cout << "ORANGE hit barrier - switching to UP" << std::endl;
                        }
                            // BLUE: RIGHT hit barrier → UP
                        else if (ghost->getType() == GhostType::BLUE &&
                                 ghost->getCurrentDirection() == Direction::RIGHT) {
                            ghost->setDirection(Direction::UP);
                            std::cout << "BLUE hit barrier - switching to UP" << std::endl;
                        }
                            // Any ghost: UP hit wall → LEFT or RIGHT
                        else if (ghost->getCurrentDirection() == Direction::UP) {
                            // BLUE goes RIGHT after exiting, others go LEFT
                            if (ghost->getType() == GhostType::BLUE) {
                                ghost->setDirection(Direction::RIGHT);
                                std::cout << "BLUE hit wall - switching to RIGHT" << std::endl;
                            } else {
                                ghost->setDirection(Direction::LEFT);
                                std::cout << "Ghost hit wall - switching to LEFT" << std::endl;
                            }
                        }
                        else {
                            ghost->stopMovement();
                        }
                    }
                    else {
                        // Normal collision handling
                        ghost->stopMovement();
                        Direction viableDir = getViableDirectionForGhost(ghost);
                        ghost->setDirection(viableDir);
                    }
                }
            }
            else {
                entity->update(deltaTime);
            }
        }
        renderInOrder();
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
                            wallViews.push_back(std::move(result.view));                         }
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
                            doorViews.push_back(std::move(result.view));  // ← CHANGE
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

                            // ← ADD THESE 2 LINES
                            pacmanSpawnX = normalizedX;
                            pacmanSpawnY = normalizedY;

                            entities.push_back(std::move(result.model));
                            pacmanView = std::move(result.view);  // ← CHANGE (unique_ptr, geen push_back)
                        }
                        break;
                    }

                    case 'R': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.85f, cellHeight * 0.85f,
                                                               GhostType::RED, 0.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghostPtr->markAsExited();
                                ghosts.push_back(ghostPtr);

                                ghostSpawnPositions.push_back({normalizedX, normalizedY});

                                // ← ADD: Set eaten respawn to center of spawn box (row 9, col 9)
                                float eatenSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                                float eatenSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                                ghostPtr->setEatenRespawnPosition(eatenSpawnX, eatenSpawnY);
                            }
                            entities.push_back(std::move(result.model));
                            ghostViews.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'P': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.85f, cellHeight * 0.85f,
                                                               GhostType::PINK, 0.0f);  // 0 second delay
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);

                                ghostSpawnPositions.push_back({normalizedX, normalizedY});

                                // Set eaten respawn to CENTER of spawn (Col 9)
                                float centerSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                                float centerSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                                ghostPtr->setEatenRespawnPosition(centerSpawnX, centerSpawnY);
                            }
                            entities.push_back(std::move(result.model));
                            ghostViews.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'B': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.85f, cellHeight * 0.85f,
                                                               GhostType::BLUE, 5.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);
                                ghostSpawnPositions.push_back({normalizedX, normalizedY});

                                // ← ADD: Spawn invisible ORANGE-only barrier at BLUE position
                                auto barrierResult = factory->createNoEntry(normalizedX, normalizedY,
                                                                            cellWidth, cellHeight);
                                if (barrierResult.model->isNoEntry()) {
                                    NoEntryModel* barrier = static_cast<NoEntryModel*>(barrierResult.model.get());

                                    // Configure: ONLY block ORANGE
                                    barrier->clearBlockedGhostTypes();  // ← NOW PUBLIC
                                    barrier->addBlockedGhostType(GhostType::ORANGE);

                                    noEntries.push_back(barrier);
                                }
                                entities.push_back(std::move(barrierResult.model));
                                // No view needed - invisible barrier
                            }
                            entities.push_back(std::move(result.model));
                            ghostViews.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'O': {
                        if (factory) {
                            auto result = factory->createGhost(normalizedX, normalizedY,
                                                               cellWidth * 0.85f, cellHeight * 0.85f,
                                                               GhostType::ORANGE, 10.0f);
                            if (result.model->isGhost()) {
                                GhostModel* ghostPtr = static_cast<GhostModel*>(result.model.get());
                                ghostPtr->setCellDimensions(cellWidth, cellHeight);
                                ghosts.push_back(ghostPtr);

                                ghostSpawnPositions.push_back({normalizedX, normalizedY});

                                // Set eaten respawn to CENTER of spawn (Col 9)
                                float centerSpawnX = -1.0f + cellWidth / 2.0f + 9 * cellWidth;
                                float centerSpawnY = -1.0f + cellHeight / 2.0f + 9 * cellHeight;
                                ghostPtr->setEatenRespawnPosition(centerSpawnX, centerSpawnY);

                                // ← ADD: Spawn invisible BLUE-only barrier at ORANGE position
                                auto barrierResult = factory->createNoEntry(normalizedX, normalizedY,
                                                                            cellWidth, cellHeight);
                                if (barrierResult.model->isNoEntry()) {
                                    NoEntryModel* barrier = static_cast<NoEntryModel*>(barrierResult.model.get());

                                    // Configure: ONLY block BLUE
                                    barrier->clearBlockedGhostTypes();
                                    barrier->addBlockedGhostType(GhostType::BLUE);

                                    noEntries.push_back(barrier);
                                }
                                entities.push_back(std::move(barrierResult.model));
                                // No view needed - invisible barrier
                            }
                            entities.push_back(std::move(result.model));
                            ghostViews.push_back(std::move(result.view));
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
                            coinViews.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'N': {  // No Entry (ghosts blocked, PacMan can pass)
                        if (factory) {
                            auto result = factory->createNoEntry(normalizedX, normalizedY, cellWidth, cellHeight);
                            if (result.model->isNoEntry()) {
                                NoEntryModel* noEntryPtr = static_cast<NoEntryModel*>(result.model.get());
                                noEntries.push_back(noEntryPtr);
                            }
                            entities.push_back(std::move(result.model));
                        }
                        break;
                    }

                    case 'F': {  // Fruit
                        if (factory) {
                            auto result = factory->createFruit(normalizedX, normalizedY,
                                                               cellWidth * 0.3f, cellHeight * 0.3f);
                            if (result.model->isFruit()) {
                                FruitModel* fruitPtr = static_cast<FruitModel*>(result.model.get());
                                fruits.push_back(fruitPtr);
                            }
                            entities.push_back(std::move(result.model));
                            fruitViews.push_back(std::move(result.view));
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
                for (const NoEntryModel* noEntry : noEntries) {
                    float neLeft = noEntry->getX() - noEntry->getWidth() / 2.0f;
                    float neRight = noEntry->getX() + noEntry->getWidth() / 2.0f;
                    float neTop = noEntry->getY() - noEntry->getHeight() / 2.0f;
                    float neBottom = noEntry->getY() + noEntry->getHeight() / 2.0f;

                    if (!(right < neLeft || left > neRight || bottom < neTop || top > neBottom)) {
                        hitObstacle = true;  // NoEntry always blocks ghosts
                        break;
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
                for (const NoEntryModel* noEntry : noEntries) {
                    float neLeft = noEntry->getX() - noEntry->getWidth() / 2.0f;
                    float neRight = noEntry->getX() + noEntry->getWidth() / 2.0f;
                    float neTop = noEntry->getY() - noEntry->getHeight() / 2.0f;
                    float neBottom = noEntry->getY() + noEntry->getHeight() / 2.0f;

                    if (!(right < neLeft || left > neRight || bottom < neTop || top > neBottom)) {
                        hitObstacle = true;  // NoEntry always blocks ghosts
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

        // ← REPLACE
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
        if (!pacman) return;

        pacman->startDeath();
        pacman->loseLife();
    }

    void World::resetAfterDeath() {
        if (!pacman) return;

        pacman->respawn(pacmanSpawnX, pacmanSpawnY);

        for (size_t i = 0; i < ghosts.size() && i < ghostSpawnPositions.size(); i++) {
            GhostModel* ghost = ghosts[i];
            float spawnX = ghostSpawnPositions[i].first;
            float spawnY = ghostSpawnPositions[i].second;

            ghost->setPosition(spawnX, spawnY);
            ghost->stopMovement();

            // ← ADD: Reset ghost state based on type
            if (ghost->getType() == GhostType::RED) {
                // RED spawns outside - direct to CHASING
                ghost->resetToSpawn(0.0f);  // Will immediately go to CHASING
            } else {
                // ORANGE/PINK/BLUE spawn inside - wait + exit
                float delay = 0.0f;
                if (ghost->getType() == GhostType::ORANGE) delay = 10.0f;
                if (ghost->getType() == GhostType::BLUE) delay = 5.0f;
                if (ghost->getType() == GhostType::PINK) delay = 0.0f;

                ghost->resetToSpawn(delay);
            }
        }

        hasJustRespawned = true;
    }

    void World::notifyViewsOnly() {
        for (const auto &entity: entities) {
            entity->notify();
        }
    }

    void World::renderInOrder() {
        for (auto& view : doorViews) {     // ← MOVE TO FIRST
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

        // Put all ghosts in fear mode
        for (GhostModel* ghost : ghosts) {
            ghost->enterFearMode();
        }

    }

    void World::nextLevel() {
        currentLevel++;

        // Respawn coins
        for (CoinModel* coin : coins) {
            coin->uncollect();
            coin->notify();
        }

        // Respawn fruits
        for (FruitModel* fruit : fruits) {
            fruit->uncollect();
            fruit->notify();
        }

        // Reset collected count
        coinsCollected = 0;

        // Reset PacMan
        if (pacman) {
            pacman->setPosition(pacmanSpawnX, pacmanSpawnY);
            pacman->stopMovement();
            pacman->notify();
        }

        // Reset ghosts
        for (size_t i = 0; i < ghosts.size() && i < ghostSpawnPositions.size(); i++) {
            GhostModel* ghost = ghosts[i];
            float spawnX = ghostSpawnPositions[i].first;
            float spawnY = ghostSpawnPositions[i].second;

            ghost->setPosition(spawnX, spawnY);

            // Reset to SPAWNING with original delays
            if (ghost->getType() == GhostType::RED) {
                ghost->resetToSpawn(0.0f);
            } else if (ghost->getType() == GhostType::PINK) {
                ghost->resetToSpawn(0.0f);
            } else if (ghost->getType() == GhostType::BLUE) {
                ghost->resetToSpawn(5.0f);
            } else if (ghost->getType() == GhostType::ORANGE) {
                ghost->resetToSpawn(10.0f);
            }

            // Increase ghost speed
            float newSpeed = baseGhostSpeed + (currentLevel - 1) * 0.01f;
            std::cout << "About to set speed for ghost type " << (int)ghost->getType() << " to " << newSpeed << std::endl;  // ← ADD

            ghost->setSpeed(newSpeed);

            std::cout << "Ghost type " << (int)ghost->getType()
                      << " speed=" << ghost->getSpeed()
                      << " target=" << ghost->getTargetSpeed() << std::endl;
        }

        // Decrease fear duration
        baseFearDuration = std::max(3.0f, 7.0f - (currentLevel - 1) * 0.5f);

        std::cout << "=== LEVEL " << currentLevel << " ===" << std::endl;
        std::cout << "Ghost speed: " << (baseGhostSpeed + (currentLevel - 1) * 0.1f) << std::endl;
        std::cout << "Fear duration: " << baseFearDuration << std::endl;
    }
}

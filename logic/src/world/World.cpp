// logic/src/world/World.cpp
#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"

namespace logic {
    void World::update(float deltaTime) {
        for (const auto &entity: entities) {
            PacManModel* pm = dynamic_cast<PacManModel*>(entity.get());

            if (pm) {
                // BUFFERED INPUT HANDLING
                Direction nextDir = pm->getNextDirection();
                if (nextDir != Direction::NONE && isDirectionValid(nextDir)) {
                    pm->applyNextDirection();
                }

                // PREDICTIVE COLLISION
                float oldX = pm->getX();
                float oldY = pm->getY();

                pm->update(deltaTime);

                // Check wall collision
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

                // ← NIEUW: Check coin collision
                for (CoinModel* coin : coins) {
                    if (!coin->isCollected() && pm->intersects(*coin)) {
                        coin->collect();
                        coinsCollected++;
                        std::cout << "Coin collected! Total: " << coinsCollected
                                  << "/" << coins.size() << std::endl;
                    }
                }

            } else {
                entity->update(deltaTime);
            }
        }
    }

    void World::setFactory(AbstractFactory* factory) {
        this->factory = factory;
    }

    void World::addEntity(std::unique_ptr<logic::EntityModel> entity) {
        entities.push_back(std::move(entity));
    }

    void World::loadMap(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open map file: " << filename << std::endl;
            return;
        }

        std::vector<std::string> mapLines;
        std::string line;
        while (std::getline(file, line)) {
            mapLines.push_back(line);
        }
        file.close();

        if (mapLines.empty()) {
            std::cerr << "ERROR: Map file is empty!" << std::endl;
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
                            WallModel* wallPtr = dynamic_cast<WallModel*>(result.model.get());
                            if (wallPtr) {
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
                            pacman = dynamic_cast<PacManModel*>(result.model.get());

                            if (pacman) {
                                pacman->setCellDimensions(cellWidth, cellHeight);
                            }

                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                        // ← NIEUW: Coin symbol
                    case '.': {
                        if (factory) {
                            auto result = factory->createCoin(normalizedX, normalizedY,
                                                              cellWidth * 0.15f, cellHeight * 0.15f);
                            CoinModel* coinPtr = dynamic_cast<CoinModel*>(result.model.get());
                            if (coinPtr) {
                                coins.push_back(coinPtr);
                            }
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case ' ':  // ← NIEUW: Empty space
                        break;

                    default:
                        std::cerr << "WARNING: Unknown symbol '" << symbol
                                  << "' at row " << row << ", col " << col << std::endl;
                        break;
                }
            }
        }

        std::cout << "Map loaded: " << width << "x" << height
                  << " (" << entities.size() << " entities, "
                  << coins.size() << " coins)" << std::endl;
    }

    PacManModel* World::getPacMan() {
        for (auto& entity : entities) {
            auto* pacman = dynamic_cast<PacManModel*>(entity.get());
            if (pacman) return pacman;
        }
        return nullptr;
    }

    std::pair<int, int> World::getMapDimensions(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open map file: " << filename << std::endl;
            return {0, 0};
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        if (lines.empty()) {
            std::cerr << "ERROR: Map file is empty!" << std::endl;
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
}
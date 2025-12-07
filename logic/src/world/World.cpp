#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"

namespace logic {
    void World::update(float deltaTime) {
        // Update alle entities, maar met predictive collision voor PacMan
        for (const auto &entity: entities) {
            // Check of dit PacMan is
            PacManModel* pm = dynamic_cast<PacManModel*>(entity.get());

            if (pm) {
                // PREDICTIVE COLLISION voor PacMan

                // Sla oude positie op
                float oldX = pm->getX();
                float oldY = pm->getY();

                // Laat PacMan berekenen waar hij naartoe wil
                pm->update(deltaTime);

                // Check of nieuwe positie collides met walls
                bool collided = false;
                for (WallModel* wall : walls) {
                    if (pm->intersects(*wall)) {
                        collided = true;
                        break;
                    }
                }

                if (collided) {
                    // Collision detected - restore oude positie en stop movement
                    pm->setPosition(oldX, oldY);
                    pm->stopMovement();
                }
                // Else: geen collision, PacMan blijft op nieuwe positie

            } else {
                // Andere entities updaten normaal (walls, ghosts later, etc.)
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

        // Cell dimensions
        float cellWidth = 2.0f / width;
        float cellHeight = 2.0f / height;

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                char symbol = mapLines[row][col];

                // Cell center positioned so edges align with [-1, 1] bounds
                float normalizedX = -1.0f + cellWidth / 2.0f + col * cellWidth;
                float normalizedY = -1.0f + cellHeight / 2.0f + row * cellHeight;

                switch (symbol) {
                    case '#': {
                        if (factory) {
                            // Walls are 100% of cell size
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
                            // PacMan hitbox is 95% of cell size
                            auto result = factory->createPacMan(normalizedX, normalizedY,cellWidth * 0.95f, cellHeight * 0.95f,0.5f);
                            pacman = dynamic_cast<PacManModel*>(result.model.get());

                            if (pacman) {
                                pacman->setCellDimensions(cellWidth, cellHeight);
                            }

                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case '.':
                        break;

                    default:
                        std::cerr << "WARNING: Unknown symbol '" << symbol
                                  << "' at row " << row << ", col " << col << std::endl;
                        break;
                }
            }
        }

        std::cout << "Map loaded: " << width << "x" << height
                  << " (" << entities.size() << " entities)" << std::endl;
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

        // Bereken waar PacMan zou zijn na een kleine stap in die richting
        const float TEST_DISTANCE = 0.01f;  // Kleine stap om te testen

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

        // Maak een tijdelijke EntityModel om collision te testen
        // We gebruiken PacMan's huidige size
        float width = pacman->getWidth();
        float height = pacman->getHeight();

        // Check of deze positie zou colliden met walls
        // We berekenen de bounds handmatig (zoals intersects() doet)
        float left = testX - width / 2.0f;
        float right = testX + width / 2.0f;
        float top = testY - height / 2.0f;
        float bottom = testY + height / 2.0f;

        for (const WallModel* wall : walls) {
            float wallLeft = wall->getX() - wall->getWidth() / 2.0f;
            float wallRight = wall->getX() + wall->getWidth() / 2.0f;
            float wallTop = wall->getY() - wall->getHeight() / 2.0f;
            float wallBottom = wall->getY() + wall->getHeight() / 2.0f;

            // AABB collision check
            if (!(right < wallLeft || left > wallRight || bottom < wallTop || top > wallBottom)) {
                return false;  // Zou colliden - richting is NIET valide
            }
        }

        return true;  // Geen collision - richting is valide
    }
}
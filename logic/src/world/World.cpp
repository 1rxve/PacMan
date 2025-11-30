#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"

namespace logic {
    void World::update(float deltaTime) {
        for (const auto &entity: entities) {
            entity->update(deltaTime);
        }
    }

    void World::setFactory(AbstractFactory* factory) {
        this->factory = factory;
    }

    void World::addEntity(std::unique_ptr<logic::EntityModel> entity) {
        entities.push_back(std::move(entity));
    }

    void World::checkCollisions() {
        // TODO: Implement collision detection
        for (size_t i = 0; i < entities.size(); i++) {
            for (size_t j = i + 1; j < entities.size(); j++) {
                if (entities[i]->intersects(*entities[j])) {
                    // Handle collision
                }
            }
        }
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
                            std::cout << "Creating wall at row=" << row << " col=" << col
                                      << " -> normalized(" << normalizedX << ", " << normalizedY << ")"
                                      << " size(" << cellWidth << ", " << cellHeight << ")" << std::endl;

                            auto result = factory->createWall(normalizedX, normalizedY, cellWidth, cellHeight);
                            entities.push_back(std::move(result.model));
                            views.push_back(std::move(result.view));
                        }
                        break;
                    }

                    case 'C': {
                        if (factory) {
                            auto result = factory->createPacMan(normalizedX, normalizedY, cellWidth, cellHeight, 0.5f);
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
}

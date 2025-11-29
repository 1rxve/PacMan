#include "logic/world/World.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"

namespace logic {
    void World::update(float deltaTime) {
        for (const auto &entity: entities) {
            entity->update(deltaTime);
        }
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

        // Lees alle regels in
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

        // Bereken map dimensies
        int height = mapLines.size();
        int width = mapLines[0].length();

        // Parse de map
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                char symbol = mapLines[row][col];

                // Bereken normalized coordinates
                float normalizedX = -1.0f + (static_cast<float>(col) / (width - 1)) * 2.0f;
                float normalizedY = 1.0f - (static_cast<float>(row) / (height - 1)) * 2.0f;

                // Cell size (voor bounding box)
                float cellWidth = 2.0f / width;
                float cellHeight = 2.0f / height;

                // Maak entities op basis van symbol
                switch (symbol) {
                    case '#':  // Wall
                        addEntity(std::make_unique<WallModel>(
                                normalizedX, normalizedY, cellWidth, cellHeight
                        ));
                        break;

                    case 'C':  // PacMan spawn
                        addEntity(std::make_unique<PacManModel>(
                                normalizedX, normalizedY, cellWidth, cellHeight, 0.5f
                        ));
                        break;

                    case '.':  // Empty space
                        // Niets doen
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
}

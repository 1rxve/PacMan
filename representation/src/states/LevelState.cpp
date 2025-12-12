#include "representation/states/LevelState.h"
#include "logic/entities/PacManModel.h"
#include <iostream>

namespace representation {
    LevelState::LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                           const Camera* cam, StateManager* sm,
                           const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile) {

        // Create World
        world = std::make_unique<logic::World>();
        world->setFactory(factory);
        world->loadMap(mapFile);

        // Setup PacMan cell dimensions (voor center-locking)
        logic::PacManModel* pacman = world->getPacMan();
        if (!pacman) {
            std::cerr << "ERROR: PacMan not found in map!" << std::endl;
            return;
        }

        // Calculate cell dimensions
        auto [mapWidth, mapHeight] = logic::World::getMapDimensions(mapFile);
        float cellWidth = 2.0f / mapWidth;
        float cellHeight = 2.0f / mapHeight;
        pacman->setCellDimensions(cellWidth, cellHeight);

        std::cout << "LevelState initialized with map: " << mapFile << std::endl;
    }

    void LevelState::update(float deltaTime) {
        world->update(deltaTime);

        // TODO: Check win/lose conditions
        // if (world->getCoinsCollected() == world->getTotalCoins()) { ... }
        // if (pacman->getLives() <= 0) { ... }
    }

    void LevelState::render() {
        // World entities tekenen automatisch via Observer pattern
        // Views roepen draw() aan bij notify()

        // TODO: Later UI toevoegen (score, lives text)
    }

    void LevelState::handleEvent(const sf::Event& event) {
        logic::PacManModel* pacman = world->getPacMan();
        if (!pacman) return;

        // Input handling - EXACT zoals main.cpp deed
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                if (pacman->getCurrentDirection() != logic::Direction::UP) {
                    pacman->setNextDirection(logic::Direction::UP);
                }
            } else if (event.key.code == sf::Keyboard::Down) {
                if (pacman->getCurrentDirection() != logic::Direction::DOWN) {
                    pacman->setNextDirection(logic::Direction::DOWN);
                }
            } else if (event.key.code == sf::Keyboard::Left) {
                if (pacman->getCurrentDirection() != logic::Direction::LEFT) {
                    pacman->setNextDirection(logic::Direction::LEFT);
                }
            } else if (event.key.code == sf::Keyboard::Right) {
                if (pacman->getCurrentDirection() != logic::Direction::RIGHT) {
                    pacman->setNextDirection(logic::Direction::RIGHT);
                }
            }
        }
    }
}
#include <iostream>
#include "representation/ConcreteFactory.h"
#include "representation/views/WallView.h"
#include "representation/views/PacManView.h"
#include "representation/views/CoinView.h"
#include "representation/views/GhostView.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"
#include "logic/entities/GhostModel.h"
#include "representation/views/DoorView.h"
#include "logic/entities/DoorModel.h"
#include "logic/entities/NoEntryModel.h"
#include "representation/views/FruitView.h"
#include "logic/entities/FruitModel.h"

namespace representation {
    ConcreteFactory::ConcreteFactory(sf::RenderWindow* window, const Camera* camera)
            : window(window), camera(camera) {

        sharedTexture = std::make_shared<sf::Texture>();
        sharedTexture->loadFromFile("resources/sprites/pacman_sprites.png");

        doorTexture = std::make_shared<sf::Texture>();
        if (!doorTexture->loadFromFile("resources/sprites/door_sprite.png")) {
            std::cerr << "ERROR: Failed to load door_sprite.png!" << std::endl;
        }
    }

    logic::EntityCreationResult ConcreteFactory::createWall(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::WallModel>(x, y, w, h);
        auto view = std::make_unique<WallView>(model.get(), window, camera);
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createPacMan(float x, float y, float w, float h, float speed) {
        auto model = std::make_unique<logic::PacManModel>(x, y, w, h, speed);
        auto view = std::make_unique<PacManView>(model.get(), window, camera, sharedTexture);
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createCoin(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::CoinModel>(x, y, w, h);
        auto view = std::make_unique<CoinView>(model.get(), window, camera, sharedTexture);
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createGhost(float x, float y, float w, float h, logic::GhostType type, float spawnDelay) {
        auto model = std::make_unique<logic::GhostModel>(x, y, w, h, type, spawnDelay);
        auto view = std::make_unique<GhostView>(model.get(), window, camera, sharedTexture);
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createDoor(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::DoorModel>(x, y, w, h);
        auto view = std::make_unique<DoorView>(model.get(), window, camera, doorTexture);  // ← USE doorTexture
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createNoEntry(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::NoEntryModel>(x, y, w, h);
        // No view needed - invisible barrier
        return {std::move(model), nullptr};
    }

    logic::EntityCreationResult ConcreteFactory::createFruit(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::FruitModel>(x, y, w, h);
        auto view = std::make_unique<FruitView>(model.get(), window, camera, sharedTexture);
        return {std::move(model), std::move(view)};
    }
}

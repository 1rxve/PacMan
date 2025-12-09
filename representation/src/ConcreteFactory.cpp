#include "representation/ConcreteFactory.h"
#include "representation/views/WallView.h"
#include "representation/views/PacManView.h"
#include "representation/views/CoinView.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"
#include <iostream>

namespace representation {
    ConcreteFactory::ConcreteFactory(sf::RenderWindow* window, const Camera* camera)
            : window(window), camera(camera) {

        sharedTexture = std::make_shared<sf::Texture>();

        if (!sharedTexture->loadFromFile("resources/sprites/pacman_sprites.png")) {
            std::cerr << "ERROR: Could not load sprite sheet!" << std::endl;
        }

        std::cout << "Sprite sheet loaded once (shared by all Views)" << std::endl;
    }

    logic::EntityCreationResult ConcreteFactory::createWall(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::WallModel>(x, y, w, h);
        auto view = std::make_unique<WallView>(model.get(), window, camera);
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createPacMan(float x, float y, float w, float h, float speed) {
        auto model = std::make_unique<logic::PacManModel>(x, y, w, h, speed);
        auto view = std::make_unique<PacManView>(model.get(), window, camera, sharedTexture);  // ← SHARED_PTR
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createCoin(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::CoinModel>(x, y, w, h);
        auto view = std::make_unique<CoinView>(model.get(), window, camera, sharedTexture);  // ← SHARED_PTR
        return {std::move(model), std::move(view)};
    }
}